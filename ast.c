
/*
 *	Module for creating an abstract syntax tree for multafila, a language
 *	focusing on making multithreading easy and intuitive.
 *	
 *	Chae Jubb
 *	
 */

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "include/global_config.h"
#include "include/error_handling.h"
#include "include/mem_manage.h"
#include "include/ast.h"
#include "include/symtab.h"
#include "include/locktab.h"

//#define AST_DEBUG

extern heap_list_head *hList;
extern locktab *lt;

void ast_destroy_helper(struct ast_s *tree);
void blank_func(void *a, void *b, void *c){return;};

struct ast_spawn_var_ptr{
	struct ast_list_s *old_vars;
	struct ast_list_s *new_vars;
	struct ast_list_s *new_vars_top;
	struct ast_list_s *old_vars_top;
};

/* CREATING LEAVES HERE */

/*
 *	Create leaft note with value and type given in the given scope
 *	Returns NULL on error
 */
 ast **ast_create_leaf_string (ast **a, char *value, symtab *symbol_table, scope *cur_scope){
 	(*a)->data.symtab_ptr = symtab_lookup(symbol_table, value, cur_scope);
 	return a;
 }

 ast **ast_create_leaf_sl (ast **a, char *value){
 	(*a)->data.string = malloc_checked_string(strlen(value) + 1);
 	strcpy((*a)->data.string, value);
 	return a;
 }

 ast **ast_create_leaf_int (ast **a, char *value, symtab *symbol_table, scope *cur_scope){
 	(*a)->data.symtab_ptr = symtab_lookup(symbol_table, value, cur_scope);
 	return a;
 }

 ast **ast_create_leaf_il (ast **a, char *value){
	//(*a)->data.integer = malloc_checked_int(strlen(value) + 1);
 	(*a)->data.integer = atoi(value);
 	return a;
 }

 ast **ast_create_leaf_char (ast **a, char *value, symtab *symbol_table, scope *cur_scope){
 	(*a)->data.symtab_ptr = symtab_lookup(symbol_table, value, cur_scope);
 	return a;
 }

 ast **ast_create_leaf_cl (ast **a, char *value){
 	(*a)->data.character = malloc_checked_string(strlen(value) + 1);
 	strcpy((*a)->data.character, value);
 	return a;
 }


 ast **ast_create_leaf_bool (ast **a, char *value, symtab *symbol_table, scope *cur_scope){
 	(*a)->data.symtab_ptr = symtab_lookup(symbol_table, value, cur_scope);
 	return a;
 }

 ast **ast_create_leaf_boollit (ast **a, char *value){
 	char *t = "TRUE";
 	char *f = "FALSE";

 	if ( strcmp( value, t) == 0 ) {
 		(*a)->data.mfbool = 1;
 	} else if ( strcmp( value, f) == 0  ) {
 		(*a)->data.mfbool = 0;
 	}
 	return a;
 }

 ast **ast_create_leaf_thread ( ast **a, char *value, symtab *symbol_table, scope *cur_scope ){
 	(*a)->data.symtab_ptr = symtab_lookup( symbol_table, value, cur_scope );
 	return a;
 }

 ast *ast_create_leaf (char *value, ast_type type, symtab *symbol_table,
 		scope *cur_scope, int lineno) {
 	//assert(value != NULL);
 	assert(type != AST_NULL);
 	assert(symbol_table != NULL);
 	assert(cur_scope != NULL);

 	ast *new_leaf;
 	malloc_checked(new_leaf);

	/* fill out leaf */
 	new_leaf->flag = 0;
 	new_leaf->node_type = AST_NODE_LEAF;
 	new_leaf->type = type;
 	new_leaf->lineno = lineno;
 	new_leaf->arraysize = -1;
 	new_leaf->containing_scope = cur_scope;
	new_leaf->data.convert_to_ptr = 0; /* default no change */

 	if (value == NULL){
 		return new_leaf;
 	}

 	switch(type){
 		case AST_STRINGLITERAL:
	 		ast_create_leaf_sl(&new_leaf, value);
	 		break;

 		case AST_STRING:
	 		ast_create_leaf_string(&new_leaf, value, symbol_table, cur_scope);
	 		break;

 		case AST_INT:
	 		ast_create_leaf_int(&new_leaf, value, symbol_table, cur_scope );
	 		break;

 		case AST_INTLITERAL:
	 		ast_create_leaf_il(&new_leaf, value);
	 		break;

 		case AST_CHAR:
	 		ast_create_leaf_char(&new_leaf, value, symbol_table, cur_scope );
	 		break;

 		case AST_CHARLITERAL:
	 		ast_create_leaf_cl(&new_leaf, value);
	 		break;

 		case AST_THREAD:
	 		ast_create_leaf_thread(&new_leaf, value, symbol_table, cur_scope );
	 		break;

 		case AST_BOOLEAN:
 		ast_create_leaf_bool(&new_leaf, value, symbol_table, cur_scope );
 		break;

 		case AST_BOOLEANLITERAL:
 		ast_create_leaf_boollit(&new_leaf, value );
 		break;

		case AST_CHARARRAY:
		case AST_INTARRAY:
		case AST_THREADARRAY:
	 		free(new_leaf);
	 		new_leaf = ast_create_array_leaf(value, -1, type, symbol_table, cur_scope, lineno);
	 		break;

 		default:
 			break;
 	}

	#ifdef AST_DEBUG
 	fprintf(stderr, "leaf value: %s; addr: %p\n", value, new_leaf);
	#endif

 	return new_leaf;
 }


/* CREATING ARRAY LEAVES HERE */


 ast **ast_create_leaf_ca (ast **a, char *value, char *size, symtab *symbol_table, scope *cur_scope){
 	(*a)->data.symtab_ptr = symtab_lookup(symbol_table, value, cur_scope);
 	(*a)->arraysize = size;
 	return a;
 }

 ast **ast_create_leaf_ia (ast **a, char *value, char *size, symtab *symbol_table, scope *cur_scope){
 	(*a)->data.symtab_ptr = symtab_lookup(symbol_table, value, cur_scope);
 	(*a)->arraysize = size;
 	return a;
 }

 ast **ast_create_leaf_ta (ast **a, char *value, char *size, symtab *symbol_table, scope *cur_scope){
 	(*a)->data.symtab_ptr = symtab_lookup(symbol_table, value, cur_scope);
 	(*a)->arraysize = size;
 	return a;
 }

 ast *ast_create_array_leaf (char *value, char *size, ast_type type, 
 		symtab *symbol_table, scope *cur_scope, int lineno) {
 	assert(value != NULL);
	//assert(size != 0);
 	assert(type != AST_NULL);
 	assert(symbol_table != NULL);
 	assert(cur_scope != NULL);

 	ast *new_leaf;
 	malloc_checked(new_leaf);

	/* fill out leaf */
	new_leaf->flag=0;
 	new_leaf->node_type = AST_NODE_LEAF;
 	new_leaf->type = type;
 	new_leaf->lineno = lineno;
 	new_leaf->containing_scope = cur_scope;
 	new_leaf->data.convert_to_ptr=0;

 	switch(type){
 		case AST_CHARARRAY:
 		case AST_CHAR:
 		ast_create_leaf_ca(&new_leaf, value, size, symbol_table, cur_scope );
 		break;

 		case AST_INTARRAY:
 		case AST_INT:
 		ast_create_leaf_ia(&new_leaf, value, size, symbol_table, cur_scope );
 		break;

  	case AST_THREADARRAY:
  	case AST_THREAD:
 		ast_create_leaf_ca(&new_leaf, value, size, symbol_table, cur_scope );
 		break;

 		default:
 		break;
 	}

	#ifdef AST_DEBUG
 	fprintf(stderr, "leaf value: %s; addr: %p\n", value, new_leaf);
	#endif

 	return new_leaf;
 }



/* CREATING INTERNAL NODES HERE */

 ast **ast_create_node_conditional( ast **a, ast_list *children ){
 	(*a)->data.conditional_statement.conditional_statement = children->data;
 	(*a)->data.conditional_statement.if_statement = children->next->data;
 	(*a)->data.conditional_statement.else_statement = children->next->next->data;

 	return a;
 }

 ast **ast_create_node_while( ast **a, ast_list *children ) {
 	(*a)->data.while_statement.conditional_statement = children->data;
 	(*a)->data.while_statement.body = children->next->data;

 	return a;
 }

 ast **ast_create_node_for(ast **a, ast_list *children) {
 	(*a)->data.for_statement.assignment = children->data;
 	(*a)->data.for_statement.relexpr = children->next->data;
 	(*a)->data.for_statement.unary = children->next->next->data;
 	(*a)->data.for_statement.body = children->next->next->next->data;
 	return a;
 }

 void ast_spawn_add_old_var(ast *a, struct ast_spawn_var_ptr *vars, ast *spawn){
 	if (a->node_type != AST_NODE_LEAF || a == NULL) return;

 	if (a->type == AST_STRINGLITERAL || a->type == AST_CHARLITERAL || a->type == AST_INTLITERAL)
 		return;

	/* check if in new_vars list first */
 	struct ast_list_s *tmp = vars->new_vars_top;
 	while(tmp->data != NULL && a->data.symtab_ptr != tmp->data->data.symtab_ptr){
 		tmp = tmp->next;
 	}
	if(tmp->data != NULL) return; /* a match */

	if (spawn->data.spawn.vars.counter != NULL && 
			a->data.symtab_ptr != spawn->data.spawn.vars.counter->data.symtab_ptr){
		a->data.convert_to_ptr = 1; /* inside spawn */
	} else {
		a->data.convert_to_ptr = 0; /* counter variable (for pfor) */
	}

	/* check if already in old list */
 	tmp = vars->old_vars_top;
 	while(tmp->data != NULL && a->data.symtab_ptr != tmp->data->data.symtab_ptr){
 		tmp = tmp->next;
 	}
	if(tmp->data != NULL) return; /* a match */

	/* add to list */
 	ast_list *new_entry;
 	heap_list_malloc(hList, new_entry);
 	new_entry->data = NULL;
 	new_entry->next = NULL;

 	(vars->old_vars)->next = (vars->old_vars)->next;

 	(vars->old_vars)->next = new_entry;
 	(vars->old_vars)->data = a;

 	vars->old_vars = new_entry;
 }

 void ast_spawn_add_new_var(ast *a, struct ast_spawn_var_ptr *vars){
 	if (a->node_type != AST_NODE_LEAF || a == NULL) return;

 	if (a->type == AST_STRINGLITERAL || a->type == AST_CHARLITERAL || a->type == AST_INTLITERAL)
 		return;

	/* check if in new_vars list first */
 	struct ast_list_s *tmp = vars->new_vars_top;
 	while(tmp->data != NULL && a->data.symtab_ptr != tmp->data->data.symtab_ptr){
 		tmp = tmp->next;
 	}
	if(tmp->data != NULL) return; /* a match */

	/* add to list */
 	ast_list *new_entry;
 	heap_list_malloc(hList, new_entry);
 	new_entry->data = NULL;
 	new_entry->next = NULL;

 	(vars->new_vars)->next = new_entry;
 	(vars->new_vars)->data = a;

 	vars->new_vars = new_entry;
 }

 void ast_spawn_var_check(ast *a, struct ast_spawn_var_ptr *vars, ast* spawn){
 	if (a == NULL) return;

 	switch(a->node_type){
 		case AST_NODE_BINARY:
			//ast_spawn_add_old_var(a->data.bin.left, vars);
 			break;

 		case AST_NODE_DECLARATION:
	 		ast_spawn_add_new_var(a->data.dec.var, vars);
	 		break;

 		case AST_NODE_LEAF:
	 		ast_spawn_add_old_var(a, vars, spawn);
	 		break;


 		default:
 			break;
 	}
 }

 ast **ast_create_node_spawn( ast **a, ast_list *children,
 	ast *counter, symtab *st, scope *cur_scope ) {
 	(*a)->data.spawn.body = NULL;//children->data;
 	(*a)->data.spawn.arguments = children->next->data;
 	(*a)->data.spawn.native_spawn = 1;

	/* get pointer to thread being used */
 	struct thread_data * td;
 	if(children->next->next == NULL){
 		td = threadtab_lookup(symtab_get_threadtab(st),
 			symtab_entry_get_name(children->next->data->data.symtab_ptr));
 	} else {
 		td = (struct thread_data *) children->next->next;
 	}

 	assert (td != NULL);

 	(*a)->data.spawn.thread = td;

	/* prepare function call */
 	char func_name[MAX_IDENT_LENGTH + 1];
 	snprintf(func_name, sizeof func_name, SPAWN_FUNC_FORMAT, td->offset);
 	symtab_insert(st, func_name, AST_VOID_STAR, ST_STATIC_DEC, NULL);

 	ast_list *body;
 	ast_list *arguments;
 	heap_list_malloc(hList, body);
 	heap_list_malloc(hList, arguments);

 	body->data = children->data;
 	body->next = arguments;
 	arguments->data = NULL;
 	arguments->next = NULL;

 	ast *new_func = ast_add_internal_node(func_name, body, AST_NODE_FUNCTION_DEF,
 		st, cur_scope, (*a)->lineno);

 	new_func->data.func_def.thread_generated = 1;
 	new_func->data.func_def.convert_to_ptr = 1;

 	threadtab_add_assoc_func(td, new_func);
 	(*a)->data.spawn.body = new_func;

	/* find variables that need to be passed as arguments */
 	ast_list *old_vars;
 	ast_list *new_vars;
 	heap_list_malloc(hList, old_vars);
 	heap_list_malloc(hList, new_vars);

 	(*a)->data.spawn.vars.old_vars = old_vars;
 	(*a)->data.spawn.vars.new_vars = new_vars;
 	(*a)->data.spawn.vars.counter = counter;

	/* initialize */
 	old_vars->data = NULL;
 	old_vars->next = NULL;
 	new_vars->data = NULL;
 	new_vars->next = NULL;

 	struct ast_spawn_var_ptr *svp;
 	malloc_checked(svp);

 	ast_list **old_vars_tracker = &old_vars;
 	ast_list **new_vars_tracker = &new_vars;

 	svp->old_vars = *old_vars_tracker;
 	svp->new_vars = *new_vars_tracker;
 	svp->new_vars_top = new_vars;
 	svp->old_vars_top = old_vars;

 	ast_walker(new_func, svp, *a,
 		(void (*)(struct ast_s *, void*, void*)) &ast_spawn_var_check,
		//(void (*)(struct ast_s *, void*)) &blank_func,
 		(void (*)(struct ast_list_s *, void*, void*)) &blank_func,
 		(void (*)(struct ast_s *, void*, void*)) &ast_spawn_var_check
		//(void (*)(struct ast_s *, void*)) &blank_func
 		);


/*	ast_list *tmp = old_vars;
	while(tmp->next != NULL){
		printf("SPAWN SYMBOL SAVED OLD: %s\n", symtab_entry_get_name(tmp->data->data.symtab_ptr));
		tmp = tmp->next;
	}

	tmp = new_vars;
	while(tmp->next != NULL){
		printf("SPAWN SYMBOL SAVED NEW: %s\n", symtab_entry_get_name(tmp->data->data.symtab_ptr));
		tmp = tmp->next;
	}*/

	/* let function def know about var status (for code gen purposes) */
		new_func->data.func_def.assoc_spawn_info = &(*a)->data.spawn;

		free(svp);

		return a;
	}

	ast **ast_create_node_barrier(ast **a, symtab *st){
		(*a)->data.barrier.thread_table = symtab_get_threadtab(st);
		return a;
	}

	/* we essentially create a for loop of spawns */
	ast **ast_create_node_pfor(ast **a, ast_list *children, char *value, symtab *st, scope *cur_scope){
		/* braced statement list of spawns followed by a barrier */
		ast *t_ident = children->data;
		ast *i_ident = children->next->data;

		/* get pointer in thread table */
		struct thread_data *td = threadtab_lookup(symtab_get_threadtab(st),
			symtab_entry_get_name(t_ident->data.symtab_ptr));

		long iterations = td->length;

		ast *hold_stmt = NULL;
		ast *top_spawn_stmt = NULL;
		long i;
		for (i = 0; i < iterations; ++i){
			/* increment counter */
      ast *increment_ident;
      malloc_checked(increment_ident);
      memcpy(increment_ident, i_ident, sizeof(*increment_ident));
		
			ast_list *unary_operand;
	    heap_list_malloc(hList, unary_operand);
	    unary_operand->data = increment_ident;
	    unary_operand->next = NULL;

	    ast *unary = ast_add_internal_node("++", unary_operand, AST_NODE_UNARY, st, cur_scope, (*a)->lineno );

	    ast_list *u_stmt;
      ast_list *u_stmt_n;
      heap_list_malloc(hList, u_stmt);
      heap_list_malloc(hList, u_stmt_n);

      u_stmt->data = unary;
      u_stmt->next = u_stmt_n;
      u_stmt_n->data = NULL;
      u_stmt_n->next = NULL;
      
      ast *unary_stmt = ast_add_internal_node(NULL, u_stmt, AST_NODE_STATEMENT, st, cur_scope, (*a)->lineno);


			/* body is body of statement; args is thread ident */
			ast_list *body;
      ast_list *args;
      heap_list_malloc(hList, body);
      heap_list_malloc(hList, args);

      body->data = children->next->next->data;
      body->next = args;
      args->data = t_ident;
      args->next = (ast_list *) td;
      ast* spawn = ast_add_internal_node((char *) increment_ident, body, AST_NODE_SPAWN, st, cur_scope, (*a)->lineno);
      //spawn->data.spawn.vars.counter = increment_ident;

	    /* combine statements */
      ast_list *stmt;
      ast_list *stmt_n;
      heap_list_malloc(hList, stmt);
      heap_list_malloc(hList, stmt_n);

      stmt->data = spawn;
      stmt->next = stmt_n;
      stmt_n->data = unary_stmt;
      stmt_n->next = NULL;
      
      ast *stmt_ast = ast_add_internal_node(NULL, stmt, AST_NODE_STATEMENT, st, cur_scope, (*a)->lineno);

      ast_list *wrapper_stmt;
      ast_list *wrapper_stmt_n;
      heap_list_malloc(hList, wrapper_stmt);
      heap_list_malloc(hList, wrapper_stmt_n);

      wrapper_stmt->data = stmt_ast;
      wrapper_stmt->next = wrapper_stmt_n;
      wrapper_stmt_n->data = NULL;
      wrapper_stmt_n->next = NULL;
      
      ast *wrapper_stmt_ast = 
      	ast_add_internal_node(NULL, wrapper_stmt, AST_NODE_STATEMENT, st, cur_scope, (*a)->lineno);

      if (hold_stmt != NULL) {
      	hold_stmt->data.stmt.next = wrapper_stmt_ast;
      	spawn->data.spawn.native_spawn = 0;
      } else {
      	top_spawn_stmt = wrapper_stmt_ast;
      }

      hold_stmt = wrapper_stmt_ast;
      td = td->next;
		}

		/* add barrier node to end of statement list */
		ast *barrier_node = ast_add_internal_node(NULL, NULL, AST_NODE_BARRIER, st, cur_scope, (*a)->lineno);

		ast_list *b_stmt;
    ast_list *b_stmt_n;
    heap_list_malloc(hList, b_stmt);
    heap_list_malloc(hList, b_stmt_n);

    b_stmt->data = barrier_node;
    b_stmt->next = b_stmt_n;
    b_stmt_n->data = NULL;
    b_stmt_n->next = NULL;
    
    ast *barrier_stmt = ast_add_internal_node(NULL, b_stmt, AST_NODE_STATEMENT, st, cur_scope, (*a)->lineno);

    hold_stmt->data.stmt.next = barrier_stmt;

    /* add assignment of counter to top of statement list */
    ast *assign_index = i_ident; 
		ast *assign_init_count = ast_create_leaf(value, AST_INTLITERAL, st, cur_scope, (*a)->lineno);

		ast_list *assign_l, *assign_r;
		heap_list_malloc(hList, assign_l);
		heap_list_malloc(hList, assign_r);
		assign_l->data = assign_index;
		assign_l->next = assign_r;
		assign_r->data = assign_init_count;
		assign_r->next = NULL;
		ast *assignment = ast_add_internal_node("=", assign_l, AST_NODE_BINARY, st, cur_scope, (*a)->lineno);

		ast_list *a_stmt;
    ast_list *a_stmt_n;
    heap_list_malloc(hList, a_stmt);
    heap_list_malloc(hList, a_stmt_n);

    a_stmt->data = assignment;
    a_stmt->next = a_stmt_n;
    a_stmt_n->data = top_spawn_stmt;
    a_stmt_n->next = NULL;
    
    ast *assign_stmt = ast_add_internal_node(NULL, a_stmt, AST_NODE_STATEMENT, st, cur_scope, (*a)->lineno);

    /* put whole thing inside braces */
    (*a)->data.stmt.body = assign_stmt;
    (*a)->data.stmt.next = NULL;
    (*a)->data.stmt.braced = 1;
    (*a)->node_type = AST_NODE_STATEMENT;

    //ast_destroy_helper(children->next->data); /* hopefully temporary */

		return a;
	}

	ast **ast_create_node_native_code( ast **a, char *value){
		(*a)->data.string = value;

		return a;
	}

	ast **ast_create_node_lock(ast **a, ast_list *children){
		(*a)->data.lock.body = children->data;
		(*a)->data.lock.params = children->next;

	/* insert params into locktab */
		ast_list *tmp = children->next;
		while (tmp != NULL){
			assert(tmp->data->node_type == AST_NODE_LEAF);
			locktab_insert(lt, tmp->data->data.symtab_ptr);
			tmp = tmp->next;
		}

		return a;
	}

	ast **ast_create_node_return(ast **a, ast_list *children){
		(*a)->data.ret.value = children->data;

		return a;
	}
	ast **ast_create_node_unary(ast **a, char *value, ast_list *children) {
		strncpy( (*a)->data.unary.op, value, 3);

		(*a)->data.unary.operand = children->data;

		return a;
	}

	ast **ast_create_node_binary(ast **a, char *value, ast_list *children){
	/* copy binary operator over */
		strncpy((*a)->data.bin.op, value, 3);

		(*a)->data.bin.left = children->data;
		(*a)->data.bin.right = children->next->data;

	#ifdef AST_DEBUG
		fprintf(stderr, "internal binary node with %p %s %p operator\n", 
			(*a)->data.bin.left, value, (*a)->data.bin.right);
	#endif

		return a;
	}

	ast **ast_create_node_dec(ast **a, ast_list *children){
		(*a)->type = children->data->type;
		(*a)->data.dec.var = children->data;

		return a;
	}

	ast **ast_create_node_func_list(ast **a, ast_list *children){
		(*a)->data.func_list.cur_func = children->data;
		(*a)->data.func_list.next_func = children->next->data;

		return a;
	}

	ast **ast_create_node_func_def(ast **a, ast_list *children, char *name,
		symtab *symbol_table, scope *cur_scope){
	/* set next function, body, and arguments accoring to convention */
		(*a)->data.func_def.body = children->data;
		children = children->next;

		(*a)->data.func_def.arguments = children;
		(*a)->data.func_def.thread_generated = 0;
		(*a)->data.func_def.convert_to_ptr = 0;

	#ifdef AST_DEBUG
		fprintf(stderr, "FUNC DEF ARGUMENTS: base: %p data: %p", children, children->data);
	#endif

		symtab_entry *tmp = symtab_lookup(symbol_table, name, cur_scope);
		assert(tmp != NULL);

	/* and the type */
		(*a)->type = symtab_entry_get_type(tmp);
		(*a)->data.func_def.func_symtab = tmp;

		return a;
	}

	ast **ast_create_node_func_call(ast **a, ast_list *children, char *name,
		symtab *symbol_table, scope *cur_scope){
		(*a)->data.func_call.arguments = children;

		strncpy((*a)->data.func_call.name, name, MAX_IDENT_LENGTH);
		(*a)->data.func_call.name[MAX_IDENT_LENGTH] = '\0';
		//(*a)->data.func_call.func_symtab = symtab_lookup(symbol_table, name, cur_scope);

		//(*a)->type = symtab_entry_get_type((*a)->data.func_call.func_symtab);

	#ifdef AST_DEBUG
		fprintf(stderr, "created func call node with child %p address\n",
			(*a)->data.func_call.arguments);
	#endif

		return a;
	}


	ast **ast_create_node_stmt(ast **a, ast_list *children, void *value){
		(*a)->data.stmt.body = children->data;
		(*a)->data.stmt.next = children->next->data;

		(*a)->type = (*a)->data.stmt.body->type;

		(*a)->data.stmt.braced = (value == BRACED_STATEMENT_MARKER);

	#ifdef AST_DEBUG
		fprintf(stderr, "statement node created:: cur: %p; next: %p\n", (*a)->data.stmt.body, (*a)->data.stmt.next);
		fprintf(stderr, "AST NODE TYPE: %d\n", ast_get_node_type(*a));
	#endif

		return a;
	}

/*
 *	Add internal (non-leaf) node to ast
 *	Action will be specified by value of type argument
 *
 *	VALUE:
 *	AST_NODE_DECLARATION:		IGNORED
 *	AST_NODE_BINARY:			1-2 character binary op
 *	AST_NODE_FUNCTION_DEF:		name of function being called
 *	AST_NODE_STATEMENT:			BRACED_STATEMENT_MARKER if braced statement
 *	AST_NODE_FUNCTION_CALL:		name of function being called
 *	AST_NODE_FUNCTION_LIST:		IGNORED
 *	AST_NODE_CONDITIONAL:		IGNORED
 *  AST_NODE_WHILE:				IGNORED
 *	AST_NODE_SPAWN:				ptr to ast of counter / NULL
 *	AST_NODE_BARRIER:			IGNORED
 *	AST_NODE_UNARY:				1-2 character unary op
 *	AST_NODE_NATIVE_CODE:	CODE
 *	AST_NODE_RETURN:			IGNORED
 *	AST_NODE_LOCK:			IGNORED
 *  AST_NODE_FOR:			IGNORED
 *  AST_NODE_PFOR:			count max
 *	
 *	
 *	CHILDREN:
 *	AST_NODE_DECLARATION: 	variable being declared
 *	AST_NODE_BINARY:		left, right
 *	AST_NODE_FUNCTION_DEF:	body, arguments
 *	AST_NODE_STATEMENT		body, next
 *	AST_NODE_FUNCTION_CALL:	arguments
 *	AST_NODE_FUNCTION_LIST:	current func, next func
 *	AST_NODE_CONDITIONAL:	conditional statement, if body, else body (NULL if no else)
 *  AST_NODE_WHILE:			conditional statement, body
 *	AST_NODE_SPAWN:			body, arguments
 *	AST_NODE_BARRIER:		IGNORED
 *	AST_NODE_UNARY:			operand
 *	AST_NODE_NATIVE_CODE: IGNORED
 *	AST_NODE_RETURN:			value of return (identifier)
 *	AST_NODE_LOCK:			body, params
 *  AST_NODE_FOR:			assignment, relexpr, unary, body
 *  AST_NODE_PFOR:			thread array, index variable, body
 *	
 *	Returns NULL on error
 */
 ast *ast_add_internal_node (char *value, ast_list *children, ast_node_type type,
 	symtab *symbol_table, scope *cur_scope, int lineno){
 	assert(children != NULL || type == AST_NODE_BARRIER || type == AST_NODE_NATIVE_CODE);
 	ast *new_node;
 	malloc_checked(new_node);

	#ifdef AST_DEBUG
 	fprintf(stderr, "NODE VALUE FIELD: %s; ADDR: %p\n", value, new_node);
	#endif

	/* fill out node */
 	new_node->flag = 0;
 	new_node->type = AST_NULL;
 	new_node->node_type = type;
 	new_node->containing_scope = cur_scope;
 	new_node->lineno = lineno;
 	new_node->arraysize = -1;

 	switch(type){
 		case AST_NODE_BINARY:
	 		assert(strlen(value) < 3);
	 		ast_create_node_binary(&new_node, value, children);
	 		break;

 		case AST_NODE_DECLARATION:
	 		ast_create_node_dec(&new_node, children);
	 		break;

 		case AST_NODE_FUNCTION_DEF:
	 		ast_create_node_func_def(&new_node, children, value, symbol_table, cur_scope);
	 		break;

 		case AST_NODE_FUNCTION_CALL:
	 		ast_create_node_func_call(&new_node, children, value, symbol_table, cur_scope);
	 		break;

 		case AST_NODE_STATEMENT:
	 		ast_create_node_stmt(&new_node, children, value);
	 		break;

 		case AST_NODE_FUNCTION_LIST:
	 		ast_create_node_func_list(&new_node, children);
	 		break;

 		case AST_NODE_CONDITIONAL:
	 		ast_create_node_conditional(&new_node, children);
	 		break;

 		case AST_NODE_WHILE:
	 		ast_create_node_while(&new_node, children );
	 		break;

 		case AST_NODE_SPAWN:
	 		ast_create_node_spawn(&new_node, children, (ast*) value,
	 			symbol_table, cur_scope);
	 		break;

 		case AST_NODE_BARRIER:
	 		ast_create_node_barrier(&new_node, symbol_table);
	 		break;

 		case AST_NODE_UNARY:
	 		assert(strlen(value) < 3);
	 		ast_create_node_unary(&new_node, value, children );
	 		break;

 		case AST_NODE_NATIVE_CODE:
	 		ast_create_node_native_code(&new_node, value);
	 		break;

 		case AST_NODE_RETURN:
	 		ast_create_node_return(&new_node, children);
	 		break;

 		case AST_NODE_LOCK:
	 		ast_create_node_lock(&new_node, children);
	 		break;

 		case AST_NODE_FOR:
	 		ast_create_node_for(&new_node, children);
	 		break;

 		case AST_NODE_PFOR:
	 		ast_create_node_pfor(&new_node, children, value, symbol_table, cur_scope);
	 		break;

 		default:
 			break;
 	}

 	return new_node;
 }

 void ast_destroy_helper_ast_list(ast_list *list){
 	struct ast_list_s *old = list, *new;
 	while(old != NULL){
 		new = old->next;
 		ast_destroy_helper(old->data);
 		old = new;
 	}
 }
 void ast_destroy_helper_func_def(struct ast_s *tree){
 	ast_destroy_helper(tree->data.func_def.body);

 	ast_destroy_helper_ast_list(tree->data.func_def.arguments);
 }

 void ast_destroy_helper(struct ast_s *tree){
 	if (tree == NULL) return;

 	switch(tree->node_type){
 		case AST_NODE_LEAF:
 		if (tree->type == AST_STRINGLITERAL){
 			free(tree->data.string);
 		}
 		if (tree->type == AST_CHARLITERAL){
 			free(tree->data.character);
 		}
 		break;


 		case AST_NODE_BINARY:
	 		ast_destroy_helper(tree->data.bin.left);
	 		ast_destroy_helper(tree->data.bin.right);
	 		break;

 		case AST_NODE_DECLARATION:
	 		ast_destroy_helper(tree->data.dec.var);
	 		break;

 		case AST_NODE_FUNCTION_DEF:
	 		ast_destroy_helper_func_def(tree);
	 		break;

 		case AST_NODE_FUNCTION_CALL:
	 		ast_destroy_helper_ast_list(tree->data.func_call.arguments);
	 		break;

 		case AST_NODE_STATEMENT:
	 		ast_destroy_helper(tree->data.stmt.body);
	 		ast_destroy_helper(tree->data.stmt.next);
	 		break;

 		case AST_NODE_FUNCTION_LIST:
	 		ast_destroy_helper(tree->data.func_list.cur_func);
	 		ast_destroy_helper(tree->data.func_list.next_func);
	 		break;

 		case AST_NODE_CONDITIONAL:
	 		ast_destroy_helper(tree->data.conditional_statement.conditional_statement);
	 		ast_destroy_helper(tree->data.conditional_statement.if_statement);
	 		ast_destroy_helper(tree->data.conditional_statement.else_statement);
	 		break;

 		case AST_NODE_WHILE:
	 		ast_destroy_helper(tree->data.while_statement.conditional_statement);
	 		ast_destroy_helper(tree->data.while_statement.body);
	 		break;

 		case AST_NODE_SPAWN:
	 		if(tree->data.spawn.native_spawn == 0){
	 			/* this is magic.  please do not change.  */
	 			ast_destroy_helper(tree->data.spawn.body->data.func_def.body->data.stmt.body);
	 			free(tree->data.spawn.body->data.func_def.body);
	 			ast_destroy_helper_ast_list(tree->data.spawn.body->data.func_def.arguments);
	 			free(tree->data.spawn.body);
	 			//free(tree->data.spawn.arguments);
	 			break;
	 		}
	 		ast_destroy_helper(tree->data.spawn.arguments);
	 		ast_destroy_helper(tree->data.spawn.body);
	 		break;

 		case AST_NODE_UNARY:
	 		ast_destroy_helper(tree->data.unary.operand);
	 		break;

 		case AST_NODE_NATIVE_CODE:
	 		free(tree->data.string);
	 		break;

 		case AST_NODE_RETURN:
	 		ast_destroy_helper(tree->data.ret.value);
	 		break;

 		case AST_NODE_LOCK:
	 		ast_destroy_helper(tree->data.lock.body);
	 		ast_destroy_helper_ast_list(tree->data.lock.params);
	 		break;

 		case AST_NODE_FOR:
	 		ast_destroy_helper(tree->data.for_statement.assignment);
	 		ast_destroy_helper(tree->data.for_statement.relexpr);
	 		ast_destroy_helper(tree->data.for_statement.unary);
	 		ast_destroy_helper(tree->data.for_statement.body);
	 		break;

 		case AST_NODE_PFOR:
	 		assert(1); /* should be filtered by this point */
	 		break;

 		default:
 			assert(1);
 			break;
 	}

 	free(tree);
 }

/*
 *	Destroy Abstract Syntax Tree created with ast_create_leaf
 *	Returns negative on error
 */
 int ast_destroy(ast *tree){
 	assert(tree != NULL);
 	ast_destroy_helper(tree);

 	return 0;
 }

/*
 *	Return ast_type of node
 *	Note this type is different than ast_node_type
 *	Returns NULL on error
 */
 ast_type ast_get_type(ast *ast_to_type){
 	return ast_to_type->type;
 }

/*
 *	Return ast_type of node
 *	Note this type is different than ast_node_type
 *	Returns NULL on error
 */
 ast_node_type ast_get_node_type(ast *ast_to_type){
 	return ast_to_type->node_type;
 }

 void ast_walker_ast_list_helper(struct ast_list_s *ast_list_to_walk, void * ptr, void *ptr2,
 	void(*ast_func)(struct ast_s *, void*, void*), 
 	void(*ast_list_func)(struct ast_list_s *, void*, void*),
 	void(*leaf_func)(struct ast_s *, void*, void*)){
 	if (ast_list_to_walk == NULL || ast_list_to_walk->data == NULL) return;

 	struct ast_list_s *old = ast_list_to_walk, *new;
 	while(old != NULL){
 		new = old->next;
 		ast_walker(old->data, ptr, ptr2, ast_func, ast_list_func, leaf_func);
 		old = new;
 	}
 }

 void ast_walker(struct ast_s *ast_to_walk, void * ptr, void *ptr2,
 	void(*ast_func)(struct ast_s *, void*, void*),
 	void(*ast_list_func)(struct ast_list_s *, void*, void*),
 	void(*leaf_func)(struct ast_s *, void*, void*)
 	){
 	assert(ast_func != NULL);
 	assert(ast_list_func != NULL);
 	assert(leaf_func != NULL);

 	if (ast_to_walk == NULL) return;
 	if (ptr == NULL) return;

 	switch(ast_to_walk->node_type){
 		case AST_NODE_LEAF:
	 		leaf_func(ast_to_walk, ptr, ptr2);
	 		break;

 		case AST_NODE_BINARY:
	 		ast_func(ast_to_walk->data.bin.left, ptr, ptr2);
	 		ast_walker(ast_to_walk->data.bin.left, ptr, ptr2, ast_func, ast_list_func, leaf_func);

	 		ast_func(ast_to_walk->data.bin.right, ptr, ptr2);
	 		ast_walker(ast_to_walk->data.bin.right, ptr, ptr2, ast_func, ast_list_func, leaf_func);
	 		break;

 		case AST_NODE_DECLARATION:
	 		ast_func(ast_to_walk->data.dec.var, ptr, ptr2);
	 		ast_walker(ast_to_walk->data.dec.var, ptr, ptr2, ast_func, ast_list_func, leaf_func);
	 		break;

 		case AST_NODE_FUNCTION_DEF:
	 		ast_func(ast_to_walk->data.func_def.body, ptr, ptr2);
	 		ast_walker(ast_to_walk->data.func_def.body, ptr, ptr2, ast_func, ast_list_func, leaf_func);
	 		break;

 		case AST_NODE_FUNCTION_CALL:
	 		ast_list_func(ast_to_walk->data.func_call.arguments, ptr, ptr2);
	 		ast_walker_ast_list_helper(ast_to_walk->data.func_call.arguments, ptr, ptr2, ast_func, ast_list_func, leaf_func);
	 		break;

 		case AST_NODE_STATEMENT:
	 		ast_func(ast_to_walk->data.stmt.body, ptr, ptr2);
	 		ast_walker(ast_to_walk->data.stmt.body, ptr, ptr2, ast_func, ast_list_func, leaf_func);

	 		ast_func(ast_to_walk->data.stmt.next, ptr, ptr2);
	 		ast_walker(ast_to_walk->data.stmt.next, ptr, ptr2, ast_func, ast_list_func, leaf_func);
	 		break;

 		case AST_NODE_FUNCTION_LIST:
	 		ast_func(ast_to_walk->data.func_list.cur_func, ptr, ptr2);
	 		ast_walker(ast_to_walk->data.func_list.cur_func, ptr, ptr2, ast_func, ast_list_func, leaf_func);

	 		ast_func(ast_to_walk->data.func_list.next_func, ptr, ptr2);
	 		ast_walker(ast_to_walk->data.func_list.next_func, ptr, ptr2, ast_func, ast_list_func, leaf_func);
	 		break;

 		case AST_NODE_CONDITIONAL:
	 		ast_func(ast_to_walk->data.conditional_statement.conditional_statement, ptr, ptr2);
	 		ast_walker(ast_to_walk->data.conditional_statement.conditional_statement, ptr, ptr2, ast_func, ast_list_func, leaf_func);

	 		ast_func(ast_to_walk->data.conditional_statement.if_statement, ptr, ptr2);
	 		ast_walker(ast_to_walk->data.conditional_statement.if_statement, ptr, ptr2, ast_func, ast_list_func, leaf_func);

	 		ast_func(ast_to_walk->data.conditional_statement.else_statement, ptr, ptr2);
	 		ast_walker(ast_to_walk->data.conditional_statement.else_statement, ptr, ptr2, ast_func, ast_list_func, leaf_func);
	 		break;

 		case AST_NODE_WHILE:
	 		ast_func(ast_to_walk->data.while_statement.conditional_statement, ptr, ptr2);
	 		ast_walker(ast_to_walk->data.while_statement.conditional_statement, ptr, ptr2, ast_func, ast_list_func, leaf_func);

	 		ast_func(ast_to_walk->data.while_statement.body, ptr, ptr2);
	 		ast_walker(ast_to_walk->data.while_statement.body, ptr, ptr2, ast_func, ast_list_func, leaf_func);
	 		break;

 		case AST_NODE_SPAWN:
	 		ast_func(ast_to_walk->data.spawn.arguments, ptr, ptr2);
	 		ast_walker(ast_to_walk->data.spawn.arguments, ptr, ptr2, ast_func, ast_list_func, leaf_func);

	 		ast_func(ast_to_walk->data.spawn.body, ptr, ptr2);
	 		ast_walker(ast_to_walk->data.spawn.body, ptr, ptr2, ast_func, ast_list_func, leaf_func);
	 		break;

 		case AST_NODE_UNARY:
	 		ast_func(ast_to_walk->data.unary.operand, ptr, ptr2);
	 		ast_walker(ast_to_walk->data.unary.operand, ptr, ptr2, ast_func, ast_list_func, leaf_func);
	 		break;

	 		case AST_NODE_LOCK:
	 		ast_func(ast_to_walk->data.lock.body, ptr, ptr2);
	 		ast_walker(ast_to_walk->data.lock.body, ptr, ptr2, ast_func, ast_list_func, leaf_func);

	 		ast_list_func(ast_to_walk->data.lock.params, ptr, ptr2);
	 		ast_walker_ast_list_helper(ast_to_walk->data.lock.params, ptr, ptr2, ast_func, ast_list_func, leaf_func);
	 		break;

 		case AST_NODE_FOR:
	 		ast_func(ast_to_walk->data.for_statement.assignment, ptr, ptr2);
	 		ast_walker(ast_to_walk->data.for_statement.assignment, ptr, ptr2, ast_func, ast_list_func, leaf_func);

	 		ast_func(ast_to_walk->data.for_statement.relexpr, ptr, ptr2);
	 		ast_walker(ast_to_walk->data.for_statement.relexpr, ptr, ptr2, ast_func, ast_list_func, leaf_func);

	 		ast_func(ast_to_walk->data.for_statement.unary, ptr, ptr2);
	 		ast_walker(ast_to_walk->data.for_statement.unary, ptr, ptr2, ast_func, ast_list_func, leaf_func);
	 		
	 		ast_func(ast_to_walk->data.for_statement.body, ptr, ptr2);
	 		ast_walker(ast_to_walk->data.for_statement.body, ptr, ptr2, ast_func, ast_list_func, leaf_func);
	 		break;

 		case AST_NODE_NATIVE_CODE:
			/* no action */
 			break;

 		default:
 			assert(1);
 	}
 }

 ast *ast_insert_native_code(ast *cur, ast *new){
 	assert(cur != NULL);
 	assert(new != NULL);

 	ast *to_ret = NULL;

 	ast_list *new_body = NULL;
 	ast_list *new_next = NULL;
 	ast *new_statement;

 	switch(cur->node_type){
 		case(AST_NODE_FUNCTION_DEF):
	 		heap_list_malloc(hList, new_body);
	 		heap_list_malloc(hList, new_next);

	 		new_body->data = new;
	 		new_body->next = new_next;
	 		new_next->data = cur->data.func_def.body;
	 		new_next->next = NULL;

	 		new_statement = ast_add_internal_node(NULL, new_body, AST_NODE_STATEMENT, NULL, NULL, new->lineno);
	 		cur->data.func_def.body = new_statement;
	 		to_ret = cur;
	 		break;

 		case AST_NODE_STATEMENT:
	 		heap_list_malloc(hList, new_body);
	 		heap_list_malloc(hList, new_next);

	 		new_body->data = new;
	 		new_body->next = new_next;
	 		new_next->data = cur->data.stmt.body;
	 		new_next->next = NULL;

	 		new_statement = ast_add_internal_node(NULL, new_body, AST_NODE_STATEMENT, NULL, NULL, new->lineno);
	 		cur->data.stmt.body = new_statement;
	 		to_ret = cur;
	 		break;

 		default:
	 		assert(1);
	 		break;
 	}

 	return to_ret;
}

ast_type ast_strip_array(ast_type t){
	ast_type to_ret = AST_NULL;

	switch(t){
		case AST_INTARRAY:
			to_ret = AST_INT;
			break;

		case AST_CHARARRAY:
			to_ret = AST_CHAR;
			break;

		case AST_THREADARRAY:
			to_ret = AST_THREAD;
			break;

		default:
			to_ret = t;
			break;
	}

	return to_ret;
}
