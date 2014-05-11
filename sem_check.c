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
#include "include/sem_check.h"

void check_ast (ast *a);
void check_stmt (ast *a);

int errorcount = 0;
ast* fuction_def_node = NULL;
ast* function_list = NULL;


ast* getfunction(char *name){
	ast* func = function_list->data.func_list.cur_func;
	symtab_entry *s = func->data.func_def.func_symtab;
	char *function_name = symtab_entry_get_name(s);
	if(strcmp(name,function_name)){
		return func;
	}

	func = function_list->data.func_list.next_func;
	while(func->data.func_list.cur_func != NULL){
		s = func->data.func_list.cur_func->data.func_def.func_symtab;
		function_name = symtab_entry_get_name(s);
	
		if(strcmp(name,function_name)){
			return func->data.func_list.cur_func;
		}
		func = func->data.func_list.next_func;
	}
	return NULL;
}

/* helper func that checks if two node are of the same type */
int are_equivalent(ast_type i, ast_type j){
	if (i == j){
		return 1;
	}else{
		return 0;
	}
}

/* returns 0 if arg lists are not of the same size or have different types */
int arglist_compare(struct ast_list_s* a, struct ast_list_s* b) {
	while (1) {
		symtab_entry *s = a->data->data.symtab_ptr;
		symtab_entry *s1 = b->data->data.symtab_ptr;

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
		s =  a->next->data->data.symtab_ptr;
		s1 = b->next->data->data.symtab_ptr;	
	}
}

void check_func_list(ast *a){
	check_ast(a->data.func_list.cur_func);
	check_ast(a->data.func_list.next_func);
	function_list = a;
}

void check_func_def(ast *a){
	//check the body statments
	check_ast(a->data.func_def.body);
	fuction_def_node = a;
}

/*check args are the correct type. check func exists. */
void check_func_call(ast *a){
	// check that function exists in the symbol table 
	// get symbol name
	symtab_entry *s = a->data.func_call.func_symtab;
	char *name = symtab_entry_get_name(s);
	symtab_entry *lookup = symtab_lookup(st, name, a->containing_scope);

	// check the return type matches the declaration type
	if (lookup == NULL) {
		printf("function used without being declared");
		errorcount++;
	}
	// check that the arguments in the function call match the declared function
	else {

		//get the fuction's node
		ast* fuction_node = getfunction(name);

		struct ast_list_s *declaredargs = fuction_node->data.func_call.arguments;
		struct ast_list_s *args = a->data.func_call.arguments;

		if (arglist_compare(declaredargs, args) != 1) {
			printf("argument types do not match function declaration");
			errorcount++;
		}
	}
}

void check_return(ast* a){
	symtab_entry *s = a->data.ret.value->data.symtab_ptr;
	//check the return type matches the fuction return type
		ast_type return_type = symtab_entry_get_type(s);
		ast_type fuction_type = symtab_entry_get_type(fuction_def_node->data.func_def.func_symtab);

		if(!are_equivalent(return_type, fuction_type)){
			printf("return type does not match with fuction type");
			errorcount++;
		}
}

void check_spawn(ast *a){
	ast *param = a->data.spawn.arguments;
	symtab_entry *s = param->data.symtab_ptr;
	char *name = s->name;
	if (symtab_lookup(st, name, a->containing_scope) == NULL) {
		printf("cannot spawn an undeclared thread");
		errorcount++;
	}
	ast *spawnbody = a->data.spawn.body;
	check_stmt(spawnbody);
}

/* binary node checker */
void check_bin(ast *a){
	ast *a_right = a->data.bin.right;
	symtab_entry *s1 = a->data.bin.left->data.symtab_ptr;
	ast_type t1 = symtab_entry_get_type(s1);

	symtab_entry *s2 = a->data.bin.right->data.symtab_ptr;
	ast_type t2 = symtab_entry_get_type(s2);

	/* if t2 is a AST_NULL type, check to see if it's another bin_node or func_call */
	if( t2 == AST_NULL){
		ast_node_type t2n = ast_get_node_type(a_right);
		if (t2n == AST_NODE_BINARY){
			check_bin(a_right);
		} else if (t2n == AST_NODE_FUNCTION_CALL){
			symtab_entry *s = a->data.func_call.func_symtab;
			/* check func return type, see if it matches lvalue */
			ast_type t3 = symtab_entry_get_type(s);
			if (are_equivalent(t1, t3) != 0){
				printf("return type does not match declaration");
				errorcount++;
			}
		}
	}

	if (are_equivalent(t1, t2) == 0){
		printf("binary node error, not an integer");
		errorcount++;
	}
}

/* unary node checker */
void check_unary(ast *a){
	symtab_entry *s = a->data.unary.operand->data.symtab_ptr;
	ast_type t = symtab_entry_get_type(s);

	if( t != AST_INT){
		printf("unary operator error, not an integer");
		errorcount++;
	}
}

/* while loop checker */
void check_while(ast *a){

	char eq[] = "==";
	char neq[] = "!=";
	char gt[] = ">";
	char lt [] = "<";
	char geq [] = ">=";
	char leq [] = "<=";

	/* should also check for funcs that return boolean?? */
	char *c = a->data.while_statement.conditional_statement->data.bin.op;
	ast *a_while_body = a->data.while_statement.body; /* get while symt body */

	if( strcmp(c, eq) == 0 || strcmp(c, neq) == 0 ||strcmp(c, gt) == 0 || strcmp(c, lt) == 0 || strcmp(c, geq) == 0 || strcmp(c, leq) == 0){
		printf("condition in while loop is not a boolean expression");
		errorcount++;
	} 
	check_stmt(a_while_body);
}

/* if statement checker */
void check_conditional(ast *a){
	/* should also check for funcs that return boolean??*/
	char *c = a->data.conditional_statement.conditional_statement->data.bin.op;
	ast *a_cond_stmt = a->data.conditional_statement.if_statement; /* get if stmt body */
	ast *a_else_stmt = a->data.conditional_statement.else_statement; /* get else stmt body */
	check_stmt(a_cond_stmt); /* check the validity of if statement body */	
	
	char eq[] = "==";
	char neq[] = "!=";
	char gt[] = ">";
	char lt [] = "<";
	char geq [] = ">=";
	char leq [] = "<=";
	
	if( strcmp(c, eq) == 0 || strcmp(c, neq) == 0 ||strcmp(c, gt) == 0 || strcmp(c, lt) == 0 || strcmp(c, geq) == 0 || strcmp(c, leq)== 0){
		printf("condition in if statement is not a boolean expression");
		errorcount++;
	}
	check_stmt(a_else_stmt);
}

/* checks if a stmt node is valid */
//checking inside fuction bodies
void check_stmt(ast *a){	
	/* check to see if it belongs to one of the stmt types */
	ast *body = a->data.stmt.body;
	/* symtab_entry *s = a->data.stmt.body.symtab_ptr; not sure if this is valid */
	ast_type t = ast_get_type(body);
	if (t == AST_NULL){
		ast_node_type t2n = ast_get_node_type(body);
		switch(t2n){

		case AST_NODE_BINARY:
			check_bin(body);
			break;

		case AST_NODE_UNARY:
			check_unary(body);
			break;

		case AST_NODE_FUNCTION_CALL:
			check_func_call(body);
			break;

		case AST_NODE_CONDITIONAL:
			check_conditional(body);
			break;

		case AST_NODE_WHILE:
			check_while(body);
			break;

		case AST_NODE_RETURN:
			check_return(body);
			break;

		 // spawn statement will be added 
		case AST_NODE_SPAWN:
			check_spawn(body);
			break;

		//  declaration statement checked in parser 

		//  barrier statement checked in parser 

		//  lock statement will be added 
		//  case AST_NODE_LOCK:
		// 	return check_lock(body);
		// 	break;

		//  pfor statement will be added 
		//  case AST_NODE_PFOR:
		// 	return check_pforbody);
		// 	break;

		default:
			break;		
		}
	}
}

/* check the main ast */
//checking outside fuction bodies
void check_ast(ast *a){
	if (a == NULL){
		return;
	}

	ast_type t = ast_get_type(a);
	if (t == AST_NULL){
		ast_node_type t2n = ast_get_node_type(a);
		switch(t2n){
			case AST_NODE_FUNCTION_LIST:
				check_func_list(a);
				break;

			case AST_NODE_FUNCTION_DEF: 
				check_func_def(a);
				break;

			case AST_NODE_STATEMENT:
				check_stmt(a);
				break;
			default:
				break;
		}
	}
}
void sem_check(ast *a){
	check_ast(a);
	if(errorcount == 0){
		printf("clean of semantic error \n");
	}else{
		printf("%d semantic error found \n", errorcount);
	}
}