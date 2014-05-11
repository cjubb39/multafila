#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h> 
#include <assert.h>

#include "include/global_config.h"
#include "include/error_handling.h"
#include "include/mem_manage.h"
#include "include/ast.h"
#include "include/symtab.h"

#include "include/ast_structs.h"
#include "include/symtab_structs.h"
#include "include/sem_check.h"

void check_ast (ast *a, symtab *st);
void check_stmt_level (ast *a, symtab *st);
int sem_check(ast *, symtab *st);


int errorcount = 0;
ast* function_def_node = NULL;
ast* function_list = NULL;

ast* getfunction(char *name){
	ast* func = function_list->data.func_list.cur_func;
	symtab_entry *s = func->data.func_def.func_symtab;
	char *function_name = symtab_entry_get_name(s);
	if(strcmp(name,function_name)==0){
		return func;
	}

	ast* func_list = function_list->data.func_list.next_func;
	while(func_list->data.func_list.cur_func != NULL){
		s = func_list->data.func_list.cur_func->data.func_def.func_symtab;
		function_name = symtab_entry_get_name(s);
		if(strcmp(name,function_name)==0){
			return func_list->data.func_list.cur_func;
		}
		func_list = func_list->data.func_list.next_func;
	}
	return NULL;
}

/* helper func that checks if two node are of the same type */
int are_equivalent(ast_type i, ast_type j){
	if (i == j){
		return 1;
	}else if(i == AST_INTLITERAL && j == AST_INT)
	{
		return 1;
	}
	else if(j == AST_INTLITERAL && i == AST_INT){
		return 1;
	}
	else if(j == AST_CHAR && i == AST_CHARLITERAL){
		return 1;
	}
	else if(i == AST_CHAR && j == AST_CHARLITERAL){
		return 1;
	}
	else if(i == AST_STRING && j == AST_STRINGLITERAL){
		return 1;
	}
	else if(j == AST_STRING && i == AST_STRINGLITERAL){
		return 1;
	}	
	else if(i == AST_THREAD && j == AST_THREAD){
		return 1;
	}
	else if(j == AST_THREAD && i == AST_THREAD){
		return 1;
	}
	else{
		return 0;
	}
}

/* returns 0 if arg lists are not of the same size or have different types */
int arglist_compare(heap_list* a, struct ast_list_s* b) {

	ast* a_node = a->data;
	ast* b_node = b->data;

	while (a_node != NULL) {

		if (b_node == NULL){  
			return 0; 
		}

		ast_type a_type = a_node->type;
		ast_type b_type = b_node->type;

		if (!are_equivalent(a_type, b_type)) {  
			return 0; 
		}

		a = a->next;
		b = b->next;

		if (a != NULL && b != NULL) {
			a_node = a->data;
			b_node = b->data;
		} else {
			break;
		}
	}

	if (a_node == NULL && b_node != NULL){  
		return 0; 
	}	
	return 1;
}

void check_func_list(ast *a, symtab *st){
	check_ast(a->data.func_list.cur_func, st);
	check_ast(a->data.func_list.next_func, st);
	function_list = a;
}

void check_func_def(ast *a, symtab *st){
	//check the body statments
	check_ast(a->data.func_def.body, st);
	function_def_node = a;
}

/*check args are the correct type. check func exists. */
void check_func_call(ast *a, symtab *st){
	// check that function exists in the symbol table 
	// get symbol name
	char *name = a->data.func_call.name;
	symtab_entry *lookup = symtab_lookup(st, name, a->containing_scope);

	// check the return type matches the declaration type
	if (lookup == NULL) {
		printf("function used without being declared\n");
		errorcount++;
	}
	// check that the arguments in the function call match the declared function
	else {
		// add symtab info to node
		a->data.func_call.func_symtab = lookup;
		a->type = symtab_entry_get_type(lookup);

		//get the function's node
		//ast* function_node = getfunction(name);
		heap_list_head *declaredargs = symtab_entry_get_ptr(lookup);// function_node->data.func_def.arguments;
		struct ast_list_s *args = a->data.func_call.arguments;

		if (arglist_compare(declaredargs->head, args) != 1) {
			printf("argument types do not match function declaration\n");
			errorcount++;
		}
	}
}

void check_return(ast* a){
	//symtab_entry *s = a->data.ret.value->data.symtab_ptr;
	ast *s = a->data.ret.value;
	//check the return type matches the function return type
		ast_type return_type = s->type;//symtab_entry_get_type(s);
		ast_type function_type = symtab_entry_get_type(function_def_node->data.func_def.func_symtab);

		if(!are_equivalent(return_type, function_type)){
			printf("return type does not match with function type\n");
			errorcount++;
		}
}

void check_spawn(ast *a, symtab *st){
	ast *param = a->data.spawn.arguments;
	symtab_entry *s = param->data.symtab_ptr;
	char *name = s->name;
	if (symtab_lookup(st, name, a->containing_scope) == NULL) {
		printf("cannot spawn an undeclared thread\n");
		errorcount++;
	}
	ast *spawnbody = a->data.spawn.body;
	check_stmt_level(spawnbody, st);
}

void check_lock(ast *a, symtab *st){
	struct ast_list_s *param = a->data.lock.params;
	ast* curparam = param->data;

	symtab_entry *s = curparam->data.symtab_ptr;
	char *name = s->name;
	if(symtab_lookup(st, name, curparam->containing_scope) == NULL){
		printf("cannot lock an undeclared variable\n");
		errorcount++;		
	}

	struct ast_list_s *nextparam = param->next;

	while(nextparam->data != NULL){
		symtab_entry *s = nextparam->data->data.symtab_ptr;
		char *name = s->name;
		if(symtab_lookup(st, name, nextparam->data->containing_scope) == NULL){
			printf("cannot lock an undeclared variable\n");
			errorcount++;		
		}
		nextparam = nextparam->next;
	}
}

/* binary node checker */
ast_type check_bin(ast *a, symtab *st){
	ast *a_right = a->data.bin.right;
	ast_node_type a_right_type = ast_get_node_type(a_right);


	ast_type right_side_evaled_type;
	if(a_right_type == AST_NODE_BINARY){

		right_side_evaled_type = check_bin(a_right, st);

	}else{
		right_side_evaled_type = a_right->type; 
	}

	ast_type t1 = a->data.bin.left->type;

	if (a_right_type == AST_NODE_FUNCTION_CALL){
		symtab_entry *s = a_right->data.func_call.func_symtab;
		/* check func return type, see if it matches lvalue */
		ast_type t3 = symtab_entry_get_type(s);
		if (are_equivalent(t1, t3) != 0){
			printf("return type does not match declaration\n");
			errorcount++;
		}
	}

	// /* if t2 is a AST_NULL type, check to see if it's another bin_node or func_call */
	// if( t2 == AST_NULL){
	// 	ast_node_type t2n = ast_get_node_type(a_right);
	// 	if (t2n == AST_NODE_BINARY){
	// 		check_bin(a_right, st);
	// 	} else if (t2n == AST_NODE_FUNCTION_CALL){
	// 		check_stmt_level(a, st);
	// 		symtab_entry *s = a->data.func_call.func_symtab;
	// 		/* check func return type, see if it matches lvalue */
	// 		ast_type t3 = symtab_entry_get_type(s);
	// 		if (are_equivalent(t1, t3) != 0){
	// 			printf("return type does not match declaration\n");
	// 			errorcount++;
	// 		}
	// 	}
	// }

	ast_type t2 = a_right->type;

	if(t2 == AST_NULL){
		t2 =right_side_evaled_type;
	}
	if (are_equivalent(t1, t2) == 0){
		printf("binary node error, type mismatch\n");
		errorcount++;
	}
		
	return right_side_evaled_type;
}

/* unary node checker */
void check_unary(ast *a){
	symtab_entry *s = a->data.unary.operand->data.symtab_ptr;
	ast_type t = symtab_entry_get_type(s);

	if( t != AST_INT){
		printf("unary operator error, not an integer\n");
		errorcount++;
	}
}

/* while loop checker */
void check_while(ast *a, symtab *st){

	// char eq[] = "==";
	// char neq[] = "!=";
	// char gt[] = ">";
	// char lt [] = "<";
	// char geq [] = ">=";
	// char leq [] = "<=";

	/* should also check for funcs that return boolean?? */
	// char *c = a->data.while_statement.conditional_statement->data.bin.op;
	ast *a_while_body = a->data.while_statement.body; /* get while symt body */

	// if( strcmp(c, eq) == 0 || strcmp(c, neq) == 0 ||strcmp(c, gt) == 0 || strcmp(c, lt) == 0 || strcmp(c, geq) == 0 || strcmp(c, leq) == 0){
	// 	printf("condition in while loop is not a boolean expression\n");
	// 	errorcount++;
	// } 
	check_stmt_level(a_while_body, st);
}

/* if statement checker */
void check_conditional(ast *a, symtab *st){
	/* should also check for funcs that return boolean??*/
	// char *c = a->data.conditional_statement.conditional_statement->data.bin.op;
	ast *a_cond_stmt = a->data.conditional_statement.if_statement; /* get if stmt body */
	ast *a_else_stmt = a->data.conditional_statement.else_statement; /* get else stmt body */
	check_stmt_level(a_cond_stmt, st); /* check the validity of if statement body */	
	
	// char eq[] = "==";
	// char neq[] = "!=";
	// char gt[] = ">";
	// char lt [] = "<";
	// char geq [] = ">=";
	// char leq [] = "<=";
	
	// if( strcmp(c, eq) == 0 || strcmp(c, neq) == 0 ||strcmp(c, gt) == 0 || strcmp(c, lt) == 0 || strcmp(c, geq) == 0 || strcmp(c, leq)== 0){
	// 	printf("condition in if statement is not a boolean expression\n");
	// 	errorcount++;
	// }
	check_stmt_level(a_else_stmt, st);
}

void check_stmt(ast *a, symtab *st){
		check_stmt_level(a->data.stmt.body, st);
		check_ast(a->data.stmt.next, st);
}

/* checks if a stmt node is valid */
//checking inside function bodies
void check_stmt_level(ast *a, symtab *st){	
	/* check to see if it belongs to one of the stmt types */
	ast *body = a;//->data.stmt.body;
	/* symtab_entry *s = a->data.stmt.body.symtab_ptr; not sure if this is valid */
	//ast_type t = ast_get_type(body);
	// if (t == AST_NULL){
	 	ast_node_type t2n = ast_get_node_type(body);
	 	printf("{%d}\n", t2n);
		switch(t2n){

		case AST_NODE_BINARY:
			check_bin(body,st);
			break;

		case AST_NODE_UNARY:
			check_unary(body);
			break;

		case AST_NODE_FUNCTION_CALL:
			check_func_call(body, st);
			break;

		case AST_NODE_CONDITIONAL:
			check_conditional(body, st);
			break;

		case AST_NODE_WHILE:
			check_while(body, st);
			break;

		case AST_NODE_RETURN:
			check_return(body);
			break;

		 // spawn statement will be added 
		case AST_NODE_SPAWN:
			check_spawn(body, st);
			break;

		case AST_NODE_DECLARATION:
			break;


		//  declaration statement checked in parser 

		//  barrier statement checked in parser 

		// lock statement will be added 
		 case AST_NODE_LOCK:
			return check_lock(body, st);
			break;

		case AST_NODE_STATEMENT:
			check_stmt(body, st);
			break;

		//  pfor statement will be added 
		//  case AST_NODE_PFOR:
		// 	return check_pforbody);
		// 	break;

		default:
			break;		
		}
	//}
}

/* check the main ast */
//checking outside function bodies
void check_ast(ast *a, symtab *st){

	if (a == NULL){
		return;
	}

	// ast_type t = ast_get_type(a);
	// if (t == AST_NULL){
		ast_node_type t2n = ast_get_node_type(a);

		switch(t2n){
			case AST_NODE_FUNCTION_LIST:
				check_func_list(a, st);
				break;

			case AST_NODE_FUNCTION_DEF: 
				check_func_def(a, st);
				break;

			case AST_NODE_STATEMENT:
				check_stmt(a, st);
				break;
			default:
				break;
		}
	// }
}
int sem_check(ast *a, symtab *st){
	check_ast(a, st);

	if(errorcount == 0){
		printf("clean of semantic error \n");
	}else{
		printf("%d semantic error found \n", errorcount);
	}

	return errorcount;
}