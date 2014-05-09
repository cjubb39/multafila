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
int are_equivalent(ast_type i, ast_type j){
	if (i == j){
		return 1;
	}else{
		return 0;
	}
}


void check_fucn_list(ast *a){
		are_equivalent();
}

void check_func_def(ast *a){

}

void check_expr(ast *a){

}

void check_func_call(ast *a){

	ast *args = a->data.func_call.arguments;
	
	ast_type t = 
}

/* binary node checker */
void check_bin(ast *a){
	ast *a left = a->data.bin.left;
	ast *a right = a->data.bin.right;
	symtab_entry *s1 = a->data.bin.left.symtab_ptr;
	ast_type t1 = symtab_entry_get_type(s1);
	
	symtab_entry *s2 = a->data.bin.right.symtab_ptr;
	ast_type t2 = symtab_entry_get_type(s2);
	
	/* if t2 is a AST_NULL type, check to see if it's another bin_node or func_call */
	if( t2 == AST_NULL){
		ast_node_type t2n = symtab_entry_get_node_type(s2);
		if (t2n == AST_NODE_BIN){
		check_bin(right);
		} else if (t2n == AST_NODE_FUNTION_CALL){
				check_func_call(right);
			}
	}
	
	if (are_equivalent(t1, t2) == 0){
		printf("binary node error, not an integer");
	} 
}

/* unary node checker */
void check_unary(ast *a){
	symtab_entry *s = a->data.unary.operand.symtab_ptr;
	ast_type t = symbtab_entry_get_type(s);
	
	if (t == AST_NULL){
		
	}
	
	if( t != AST_INT){
		printf("unary operator error, not an integer");
	}
}

/* while loop checker */
void check_while(ast *a){
	
	char *c = a->data.while_statement.conditional_statement.bin.op;
	if( c != "==" || c != "!=" || c != ">" || c != "<" || c != ">=" || c != "<="){
		printf("condition in while loop is not a boolean expression");
		}
}

/* if statement checker */
void check_conditional(ast *a){
	char *c = a->data.conditional_statement.conditional_statement.bin.op;
	if( c != "==" || c != "!=" || c != ">" || c != "<" || c != ">=" || c != "<="){
		printf("condition in if statement is not a boolean expression");
		}
}

/*check return statement types */
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