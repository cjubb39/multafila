#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/wait.h> 
#include <assert.h>

#include "include/global_config.h"
#include "include/error_handling.h"
#include "include/mem_manage.h"
#include "include/ast.h"
#include "include/symtab.h"
#include "include/threadtab.h"

#include "include/ast_structs.h"
#include "include/symtab_structs.h"

/* comment to send straight to gcc (./MFtest); uncomment to stdout */
#define PRINT_TO_STDOUT
//#define SIMPLE_OUTPUT
//#define GEN_TEST_DEBUG

#define THREADSNAME "global_threads"

extern char *exe_out_name;

void print_ast(ast *);
char* get_ast_type(ast_type);

void print_function_prototype(ast *a){
	printf("%s", get_ast_type(symtab_entry_get_type(a->data.func_def.func_symtab)));
	printf( " %s ( ", a->data.func_def.func_symtab->name);

	struct ast_list_s *tmp = a->data.func_def.arguments;
	while (tmp != NULL){
		print_ast(tmp->data);
		tmp = tmp->next;

		if (tmp != NULL)
			printf( ", ");
	}

	printf( " )");
}

void print_headers(ast *a){    
	assert(a->node_type == AST_NODE_FUNCTION_LIST);
	
	/* standard headers */
	printf( "#include <stdio.h>\n#include <pthread.h>\n" \
		"void printOut(char *m){printf(\"%%s\\n\", m);}\n\n");

	/* headers for our functions */
	ast *tmp = a;
	while(tmp != NULL){
		print_function_prototype(tmp->data.func_list.cur_func);
		printf(";\n");
		tmp = tmp->data.func_list.next_func;
	}
	printf("\n");
}

void print_threadtab_func(ast *a){
	#ifdef GEN_TEST_DEBUG
	printf("<printing THREADTAB_FUNC>");
	#endif

	if (a == NULL) return;

	struct ast_spawn_node *asn = (struct ast_spawn_node *)(a->data.func_def.assoc_spawn_info);
	struct ast_spawn_vars *asv = &asn->vars;
	const int t_index = asn->thread->offset;

	/* create struct for passing args*/
	ast_list *args_struct = asv->old_vars;
	ast *tmp;
	symtab_entry *se;
	printf("struct " SPAWN_ARGS_FORMAT "{\n", t_index);
	while((tmp = args_struct->data) != NULL){
		se = tmp->data.symtab_ptr;
		printf("\t%s", get_ast_type(symtab_entry_get_type(se)));
		printf(" * arg_t%d_%s;\n", \
			t_index, symtab_entry_get_name(se));

		args_struct = args_struct->next;
	}
	printf("};\n\n");

	/* large buffer for generated code */
	size_t large_buff_size = 1024 * 1024;
	char *buffer = malloc_checked_string(large_buff_size);

	char buffer2[1024]; /* smaller buffer for intermediate code */
	memset(buffer, 0, sizeof buffer);
	memset(buffer2, 0, sizeof large_buff_size);

	/* generate transfer from struct */
	args_struct = asv->old_vars;

	snprintf(buffer2, sizeof buffer2, "struct " SPAWN_ARGS_FORMAT
		"* args_t%d = (struct " SPAWN_ARGS_FORMAT "*) args_" SPAWN_FUNC_FORMAT ";\n",
		t_index,
		t_index,
		t_index,
		t_index
	);

	strncat(buffer, buffer2, large_buff_size);

	while ((tmp = args_struct->data) != NULL){
		se = tmp->data.symtab_ptr;

		snprintf(buffer2, sizeof buffer2, "%s * %s = args_t%d->arg_t%d_%s;\n",
			get_ast_type(symtab_entry_get_type(se)),
			symtab_entry_get_name(se),
			t_index,
			t_index,
			symtab_entry_get_name(se)
		);

		size_t tmp_strlen = strlen(buffer);
		strncat(buffer, buffer2, large_buff_size - tmp_strlen - 1);

		args_struct = args_struct->next;
	}

	ast *native = ast_add_internal_node(buffer, NULL, AST_NODE_NATIVE_CODE, NULL, NULL);

	ast_insert_native_code(a, native);

	print_ast(a);

	/* buffer freed on AST disassembly */
}

void print_threadtab(threadtab *tb){
	#ifdef GEN_TEST_DEBUG
	printf("<printing THREADTAB>");
	#endif

	printf("pthread_t " THREADSNAME " [%d];\n", (uint32_t) tb->length);

	struct thread_data *td = tb->head;
	while (td != NULL){
		print_threadtab_func(td->assoc_ast);
		td = td->next;
	}
}

void print_native_code(ast *a){
	printf("%s", a->data.string);
}

char* get_ast_type(ast_type at){
	assert(at != AST_NULL);

	#ifdef GEN_TEST_DEBUG
	printf("<printing AST_TYPE>");
	#endif

	switch(at){
		case AST_STRING:
			return "char *";
			break;

		case AST_STRINGLITERAL:
			return "char *";
			break;

		case AST_INT:
			return "int";
			break;

		case AST_VOID_STAR:
			return "void *";
			break;

		case AST_VOID:
			return "void";
			break;

		case AST_THREAD:
			return "";
			break; /* taken care of separately */

		default:
			return "";
			break;
	}
}


void print_func_list(ast *a){
	#ifdef GEN_TEST_DEBUG
	printf("<printing FUNC_LIST>");
	#endif
	print_ast(a->data.func_list.cur_func);
	print_ast(a->data.func_list.next_func);
}

void print_func_def_nominal(ast *a){
	#ifdef GEN_TEST_DEBUG
	printf("<printing FUNC_DEF_NOMINAL>");
	#endif

	print_function_prototype(a);

	printf( "\n");
	print_ast(a->data.func_def.body);
	printf( "\n");
}

void print_func_def_thread_gen(ast *a){
	#ifdef GEN_TEST_DEBUG
	printf("<printing FUNC_DEF_THREAD_GEN>");
	#endif
	
	/* print opening statement */
	printf("%s", get_ast_type(symtab_entry_get_type(a->data.func_def.func_symtab)));
	printf( " %s ( ", a->data.func_def.func_symtab->name);
	printf( " void * args_%s", symtab_entry_get_name(a->data.func_def.func_symtab));
	printf( " )\n{\n");

	/*  */

	print_ast(a->data.func_def.body);
	printf( "\n}\n");
}

void print_func_def(ast *a){
	#ifdef GEN_TEST_DEBUG
	printf("<printing FUNC_DEF>");
	#endif
	if (a->data.func_def.thread_generated == 0){
		print_func_def_nominal(a);
	} else {
		print_func_def_thread_gen(a);
	}
}

void print_func_call(ast *a){
	#ifdef GEN_TEST_DEBUG
	printf("<printing FUNC_CALL>");
	#endif

	printf( "%s( ", a->data.func_call.func_symtab->name);

	struct ast_list_s *tmp = a->data.func_call.arguments;
	while (tmp != NULL){
		print_ast(tmp->data);
		tmp = tmp->next;
		
		if (tmp != NULL)
			printf( ", ");
	}
	printf( " )");
}

void print_dec(ast *a){
	#ifdef GEN_TEST_DEBUG
	printf("<printing DEC>");
	#endif

	printf( "%s  ", get_ast_type(a->type));
	print_ast(a->data.dec.var);
}

void print_bin(ast *a){
	#ifdef GEN_TEST_DEBUG
	printf("<printing BINARY>");
	#endif

	print_ast(a->data.bin.left);
	printf( " %s ", a->data.bin.op);
	print_ast(a->data.bin.right);
}

void print_stmt(ast *a){
	#ifdef GEN_TEST_DEBUG
	printf("<printing STMT>");
	#endif

	if(a->data.stmt.braced == 0){
		print_ast(a->data.stmt.body);
		printf( " ;\n");
		print_ast(a->data.stmt.next);
	} else {
		printf("{\n");
		print_ast(a->data.stmt.body);
		printf("}\n");
	}
}

void print_unary(ast *a){
	#ifdef GEN_TEST_DEBUG
	printf("<printing UNARY>");
	#endif
	print_ast(a->data.unary.operand);
	printf( " %s ", a->data.unary.op);
	printf( " ;\n");
}

void print_spawn(ast *a){
	#ifdef GEN_TEST_DEBUG
	printf("<printing SPAWN>");
	#endif	

	struct thread_data *td = a->data.spawn.thread;
	struct ast_spawn_vars *asv = &a->data.spawn.vars;
	int t_index = td->offset;

	td->started = 1;
	td->completed = 0;

	/* create struct for passing */
	ast_list *args_struct = asv->old_vars;
	ast *tmp;
	symtab_entry *se;


	printf("struct " SPAWN_ARGS_FORMAT " args_t%d_struct;\n", t_index, t_index);
	
	while((tmp = args_struct->data) != NULL){
		se = tmp->data.symtab_ptr;
		printf("args_t%d_struct.arg_t%d_%s = &%s;\n",
			t_index, t_index, symtab_entry_get_name(se), symtab_entry_get_name(se));

/*
		printf(" %s", get_ast_type(symtab_entry_get_type(se)));
		printf(" * arg_t%d_%s;\n", \
			t_index, symtab_entry_get_name(se));*/

		args_struct = args_struct->next;
	}

	// int pthread_create(pthread_t *restrict thread, const pthread_attr_t *restrict attr, void *(*start_routine)(void*), void *restrict arg);
	printf( "pthread_create(");
	printf("&" THREADSNAME "[%d],", t_index);
	printf( " NULL, ");
	printf( SPAWN_FUNC_FORMAT ", " , t_index);
	printf("& args_t%d_struct);\n", t_index);
}

void print_barrier(ast *a){
	#ifdef GEN_TEST_DEBUG
	printf("<printing SPAWN>");
	#endif

	struct thread_data *td = a->data.barrier.thread_table->head;
	while (td != NULL){
		if (td->completed == 0 && td->started == 1){
			printf("pthread_join(" THREADSNAME "[%d], NULL);\n", td->offset);
			td->completed = 1;
			td->started = 0;
		}
		td = td->next;
	}
}

void print_while(ast *a){
	#ifdef GEN_TEST_DEBUG
	printf("<printing WHILE>");
	#endif
	printf( "while(");
	print_ast(a->data.while_statement.conditional_statement);
	printf( " )\n{\n");
	print_ast(a->data.while_statement.body);
	printf( "\n}\n");
}

void print_con(ast *a){
	#ifdef GEN_TEST_DEBUG
	printf("<printing IF>");
	#endif
	printf( "if(");
	print_ast(a->data.conditional_statement.conditional_statement);
	printf( " )\n");
	print_ast(a->data.conditional_statement.if_statement);
	printf( "\n");

	//print else statment if there is one
	if(a->data.conditional_statement.else_statement != NULL){
		printf( "else\n");
		print_ast(a->data.conditional_statement.else_statement);
	}
}

void print_leaf(ast *a){
	#ifdef GEN_TEST_DEBUG
	printf("<printing LEAF>");
	#endif

	if (a->flag == 1){
		printf(" %s ", get_ast_type(a->type));
	}

	switch(a->type){
		case AST_STRINGLITERAL:
			printf( "%s", a->data.string);
			break;

		case AST_STRING:
			if(a->data.convert_to_ptr == 1) printf("* ");
			printf( "%s", symtab_entry_get_name(a->data.symtab_ptr));
			break;

		case AST_INTLITERAL:
			printf( "%d", (a->data.integer));
			break;

		case AST_INT:
			if(a->data.convert_to_ptr == 1) printf("* ");
			printf( "%s", symtab_entry_get_name(a->data.symtab_ptr));
			break;

		case AST_CHARLITERAL:
			printf( "%s", a->data.string);
			break;

		case AST_CHAR:
			if(a->data.convert_to_ptr == 1) printf("* ");
			printf( "%s", symtab_entry_get_name(a->data.symtab_ptr));
			break;
			
		default:
			assert(1);
			break;
	}
}

void print_ast(ast *a){
	#ifdef GEN_TEST_DEBUG
	printf("<printing AST_MAIN>");
	#endif

	if (a == NULL){
		return;
	}

	switch(ast_get_node_type(a)){
		case AST_NODE_FUNCTION_LIST:
			print_func_list(a);
			break;

		case AST_NODE_FUNCTION_DEF:
			print_func_def(a);
			break;

		case AST_NODE_FUNCTION_CALL:
			print_func_call(a);
			break;

		case AST_NODE_DECLARATION:
			print_dec(a);
			break;

		case AST_NODE_BINARY:
			print_bin(a);
			break;

		case AST_NODE_STATEMENT:
			print_stmt(a);
			break;

		case AST_NODE_LEAF:
			print_leaf(a);
			break;

		case AST_NODE_CONDITIONAL:
			print_con(a);
			break;

		case AST_NODE_WHILE:
			print_while(a);
			break;

		case AST_NODE_SPAWN:
			print_spawn(a);
			break;	

		case AST_NODE_BARRIER:
			print_barrier(a);
			break;

		case AST_NODE_UNARY:
			print_unary(a);
			break;

		case AST_NODE_NATIVE_CODE:
			print_native_code(a);
			break;

		default:
			assert(0 == 1);
			break;
	}
}

void gen_code(ast *a, symtab *st, threadtab *tb){

#ifdef SIMPLE_OUTPUT
	printf("\n/*==========OUTPUT CODE BELOW==========*/\n");
	print_headers(a);
	print_threadtab(tb);
	print_ast(a);
#else
	pid_t pid = fork();

	if (pid == 0) {

		int pipeFileDescriptors[2];

		pipe(pipeFileDescriptors);

		if (fork() == 0){

			close(pipeFileDescriptors[1]);

			if (dup2(pipeFileDescriptors[0], STDIN_FILENO) != STDIN_FILENO)
	        die("dup2 error: ls to pipe stdout");

	#ifdef PRINT_TO_STDOUT
			char buffer[1024]; int n;
      while ((n = read(pipeFileDescriptors[0], buffer, sizeof buffer - 1)) != 0){
          buffer[n] = '\0';
        printf("%s", buffer);
      }
      exit(0);
	#else
			execlp("gcc", "gcc", "-g", "-pthread", "-o", exe_out_name, "-xc", "-", NULL);
	#endif

		} else {


			/* close read end of pipe */
	    close(pipeFileDescriptors[0]);

	    /* redirect to-be ls output (stdout and stderr) to pipe */
	    if (dup2(pipeFileDescriptors[1], STDOUT_FILENO) != STDOUT_FILENO)
	        die("dup2 error: ls to pipe stdout");
	  /*  if (dup2(pipeFileDescriptors[1], STDERR_FILENO) != STDERR_FILENO)
	        die("dup2 error: ls to pipe stderr");*/
	    close(pipeFileDescriptors[1]);

			printf("\n/*==========OUTPUT CODE BELOW==========*/\n");
			print_headers(a);
			print_threadtab(tb);
			print_ast(a);
			exit(0);
		}

	} else {
		waitpid(pid, NULL, 0);
	}
#endif
}
