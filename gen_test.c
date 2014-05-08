#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h> 
#include <assert.h>

#include "include/global_config.h"
#include "include/error_handling.h"
#include "include/ast.h"
#include "include/symtab.h"

#include "include/ast_structs.h"
#include "include/symtab_structs.h"

/* comment to send straight to gcc (./MFtest); uncomment to stdout */
#define PRINT_TO_STDOUT
//#define GEN_TEST_DEBUG
#define SIMPLE_OUTPUT


void print_ast(ast *a);

void print_header(){     
	printf( "#include <stdio.h>\nvoid printOut(char *m){printf(\"%%s\\n\", m);}\n\n");
}


void print_ast_type(ast_type at){
	assert(at!=AST_NULL);

	#ifdef GEN_TEST_DEBUG
	printf("<printing AST_TYPE>");
	#endif
	switch(at){
		case AST_STRING:
			printf( "char *");
			break;

		case AST_STRINGLITERAL:
			printf( "char *");
			break;

		case AST_INT:
			printf( "int");
			break;

		default:
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

void print_func_def(ast *a){
	#ifdef GEN_TEST_DEBUG
	printf("<printing FUNC_DEF>");
	#endif
	print_ast_type(symtab_entry_get_type(a->data.func_def.func_symtab));
	printf( " %s ( ", a->data.func_def.func_symtab->name);

	struct ast_list_s *tmp = a->data.func_def.arguments;
	while (tmp != NULL){
		print_ast(tmp->data);
		tmp = tmp->next;

		if (tmp != NULL)
			printf( ", ");
	}

	printf( " )\n{\n");
	print_ast(a->data.func_def.body);
	printf( "\n}\n");
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

	print_ast_type(a->type);
	printf( "  ");
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
	print_ast(a->data.stmt.body);
	printf( " ;\n");
	print_ast(a->data.stmt.next);
}

void print_unary(ast *a){
	#ifdef GEN_TEST_DEBUG
	printf("<printing UNARY>");
	#endif
	print_ast(a->data.unary.operand);
	printf( " %s ", a->data.unary.op);
	printf( " ;\n");
}

void print_barrier(ast *a){
	#ifdef GEN_TEST_DEBUG
	printf("<printing BARRIER>");
	#endif	
    
    // int pthread_join(pthread_t thread, void **value_ptr);
	printf( "pthread_join(");
	printf( "thread_%d,", threadcount);
	printf( "NULL");
	printf( " );");
}

void print_spawn(ast *a){
	#ifdef GEN_TEST_DEBUG
	printf("<printing SPAWN>");
	#endif	

	// SPAWN LPAREN IDENTIFIER RPAREN statement_list
	// I think we need to access the threadtable to get related information for thread, I dont know where I can get threadcount.
	printf( "pthread_t* thread_%d;", threadcount);

	// int pthread_create(pthread_t *restrict thread, const pthread_attr_t *restrict attr, void *(*start_routine)(void*), void *restrict arg);
	printf( "pthread_create(");
	printf( "thread_%d,", threadcount);
	printf( "NULL");
	printf( "(void *) &thread_fuction_%d",threadcount);
	printf( "(void *) &");
	print_ast(a->data.spawn.arguments);
	printf( " );");

	// function def need to go to the top. How do we achieve this?
	printf( "void thread_fuction_%d (void* args)", threadcount);
	
	// thread fuction 
	printf( "void thread_fuction_%d (void* args)", threadcount);
	printf( "\n{\n");
	print_ast(a->data.spawn.body);
	printf( "pthread_exit(0);");
	printf( "\n}\n");
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
	printf( " )\n{\n");
	print_ast(a->data.conditional_statement.if_statement);
	printf( "\n}\n");

	//print else statment if there is one
	if(a->data.conditional_statement.else_statement != NULL){
		printf( "\n{\n");
		print_ast(a->data.conditional_statement.else_statement);
		printf( "\n}\n");
	}
}

void print_leaf(ast *a){
	#ifdef GEN_TEST_DEBUG
	printf("<printing LEAF>");
	#endif

	switch(a->type){
		case AST_STRINGLITERAL:
			printf( "%s", a->data.string);
			break;

		case AST_STRING:
			printf( "%s", a->data.string);
			break;

		case AST_INTLITERAL:
			printf( "%d", *(a->data.integer));
			break;

		case AST_INT:
			printf( "%s", a->data.string);
			break;

		case AST_CHARLITERAL:
			printf( "%s", a->data.string);
			break;

		case AST_CHAR:
			printf( "%s", a->data.character);
			break;

		default:
			printf( "%s", a->data.symtab_ptr->name);
			break;

		// AST_THREAD,
		// AST_CHARARRAY,
		// AST_CHARARRAYLITERAL,
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

		case AST_NODE_CONDITIONAL:
			print_con(a);
			break;

		case AST_NODE_WHILE:
			print_while(a);
			break;

		case AST_NODE_UNARY:
			print_unary(a);
			break;

		case AST_NODE_SPAWN:
			print_spawn(a);
			break;	

		case AST_BARRIER:
			print_barrier(a);
			break;

		case AST_NODE_LEAF:
			print_leaf(a);
			break;

		default:
			assert(0 == 1);
			break;
	}
}

void gen_test(ast *a){

#ifdef SIMPLE_OUTPUT
	fprintf(stderr, "\n==========OUTPUT CODE BELOW==========\n");
	print_header();
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
			execlp("gcc", "gcc", "-o", "MFtest", "-xc", "-", NULL);
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


			fprintf(stderr, "\n==========OUTPUT CODE BELOW==========\n");
			print_header();
			print_ast(a);
			exit(0);
		}

	} else {
		waitpid(pid, NULL, 0);
	}
#endif
}
