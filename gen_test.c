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

void print_ast(ast *a);

void print_header(){
	printf( "#include <stdio.h>\nvoid printOut(char *m){printf(\"%%s\\n\", m);}\n\n");
}


void print_ast_type(ast_type at){
	#ifdef GEN_TEST_DEBUG
	printf("<printing AST_TYPE>");
	#endif
	switch(at){
		case AST_STRING:
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

void print_leaf(ast *a){
	#ifdef GEN_TEST_DEBUG
	printf("<printing LEAF>");
	#endif

	switch(a->type){
		case AST_STRINGLITERAL:
			printf( "%s", a->data.string);
			break;

		default:
			printf( "%s", a->data.symtab_ptr->name);
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

		default:
			assert(0 == 1);
			break;
	}
}

void gen_test(ast *a){
	
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
}
