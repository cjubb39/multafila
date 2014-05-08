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


void check_ast(ast *a);

/* helper func that checks if two node are of the same type */
int are_equivalent(){

}

void check_fucn_list(ast *a){
		are_equivalent();
}

void check_func_def(ast *a){

}

void check_expr(ast *a){


}

void check_while(ast *a){
	a->data.while_statement.conditional_statement
	if(ast_get_node_type(a->data.while_statement.conditional_statement) != BOOLTYPE){
		printf("while loop condition is not a boolean expression");
	}

/* have to create these scope stacks somehow */
	push(breakstack, 1);
	push(continuestack, 1);
	
	check_expr(ast *a);
	
	pop(breakstack);
	pop(continuestack);

}

void check_return(ast *a){
	if ( != NULL){
		rettype = ;
	}
	
	if (funtype != rettype){
		error
	}

}


/* check the main ast */
void check_ast(ast *){
	if (a == NULL){
		return;
	}
	
	switch(ast_get_node_type(a)){
		case 
	}

}