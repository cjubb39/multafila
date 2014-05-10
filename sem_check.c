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


int check_ast (ast *a);

/* helper func that checks if two node are of the same type */
int are_equivalent(ast_type i, ast_type j){
	if (i == j){
		return 1;
	}else{
		return 0;
	}
}



int check_func_def(ast *a){
	// check that function parameters are properly formed
	// check the return type matches the declaration type

	// switch statements
}

/*check args are the correct type. check func exists. */
void check_func_call(ast *a){
	// check that function exists in the symbol table 
	// get symbol name
	symtab_entry *s = a->data.func_call.func_symtab;
	char *name = s->name;
	symtab_entry *lookup = symtab_search_scope(a->containing_scope, name);
	if (lookup == NULL) {
		printf("function used without being declared");
	}
	// check that the arguments in the function call match the declared function
	else {
		ast_list *declaredargs = lookup->data.func_call.arguments;
		ast_list *args = a->data.func_call.arguments;

		if (arglist_compare(declaredargs, args) != 1) {
			printf("argument types do not match function declaration");
			

		}
	}
}


int check_expr(ast *a){

}

/* binary node checker */
int check_bin(ast *a){
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
		} else if (t2n == AST_NODE_FUNCTION_CALL){
			symtab_entry *s = a->data.func_call.func_symtab;
				/* check func return type, see if it matches lvalue */
			ast_type t3 = symtab_entry_get_type(s);
			if (are_equivalent(t1, t3) != 0){
				printf("return type does not match declaration");
				return 0;
			}
		}
	}

	if (are_equivalent(t1, t2) == 0){
		printf("binary node error, not an integer");
		return 0;
	}else{
		return 1;
	}

/* unary node checker */
	int check_unary(ast *a){
		symtab_entry *s = a->data.unary.operand.symtab_ptr;
		ast_type t = symbtab_entry_get_type(s);

		if( t != AST_INT){
			printf("unary operator error, not an integer");
			return 0;
		}else{
			return 1;
		}
	}

/* while loop checker */
	int check_while(ast *a){

	/* should also check for funcs that return boolean?? */
		char *c = a->data.while_statement.conditional_statement.bin.op;
		if( c != "==" || c != "!=" || c != ">" || c != "<" || c != ">=" || c != "<="){
			printf("condition in while loop is not a boolean expression");
			retrun 0;
		}

		ast *a while_body = a->data.while_statement.body;
		check_stmt(while_body);
	}

/* if statement checker */
	int check_conditional(ast *a){

	/* should also check for funcs that return boolean??*/
		char *c = a->data.conditional_statement.conditional_statement.bin.op;
		if( c != "==" || c != "!=" || c != ">" || c != "<" || c != ">=" || c != "<="){
			printf("condition in if statement is not a boolean expression");
			return 0;
		}

		ast *a cond_stmt = a->data.conditional_statement.if_statement;
		return(check_stmt(cond_stmt));

		ast *a else_stmt = a->data.conditional_statement.else_statement;
		return(check_stmt(else_stmt));

	}

/* checks if a stmt node is valid */
	int check_stmt(ast *a){	
	/* check to see if it belongs to one of the stmt types */
		ast *body = a->data.stmt.body;
	/* symtab_entry *s = a->data.stmt.body.symtab_ptr; not sure if this is valid */
		ast_type t = ast_get_type(body);
		if (t == AST_NULL){
			ast_node_type t2n = ast_get_node_type(body);
			switch(t2n){
				case AST_NODE_BIN:
				return check_bin(body);
				break;
				case AST_NODE_UNARY:
				return check_unary(body);
				break;
				case AST_NODE_FUNCTION_CALL:
				return check_func_call(body);
				break;
				case AST_NODE_CONDITIONAL:
				return check_conditional(body);
				break;
				case AST_NODE_WHILE:
				return check_while(body);
				break;
			/* spawn statement will be added 
			case AST_NODE_SPAWN:
				return check_spawn(body);
			*/	
			/* return will be added */
				default:
				return 0;
				break;		
			}
		}
	}

	/* returns 0 if arg lists are not of the same size or have different types */

	int arglist_compare(ast_list a, ast_list b) {
		while (1) {
			symtab_entry *s = a->data.symtab_ptr;
			symtab_entry *s1 = b->data.symtab_ptr;
			if(s == NULL && s1 == NULL)     
			{  
				return 1; 
			}
			if(s == NULL && s1 != NULL)  
			{  
				return 0; 
			}
			if(s != NULL && s1 == NULL)  
			{  
				return 0; 
			}
			ast_type t = symtab_entry_get_type(s);
			ast_type t1 = symtab_entry_get_type(s1);
			if(!are_equivalent(t, t1))
			{  
				return 0; 
			}
			s =  a->next;
			s1 = b->next;	
		}
	}

/* check the main ast */
	int check_ast(ast *){
		if (a == NULL){
			return;
		}

		switch(ast_get_node_type(t)){
			case func_def: 
			check_func_call();
			case 
		}

	}
