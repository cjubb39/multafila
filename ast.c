#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "include/global_config.h"
#include "include/error_handling.h"
#include "include/mem_manage.h"
#include "include/ast.h"
#include "include/symtab.h"

//#define AST_DEBUG

extern heap_list_head *hList;

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
	(*a)->data.integer = malloc_checked_int(strlen(value) + 1);
	(*a)->data.integer = value;
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

ast **ast_create_leaf_thread ( ast **a, char *value, symtab *symbol_table, scope *cur_scope ){
	(*a)->data.symtab_ptr = symtab_lookup( symbol_table, value, cur_scope );
	return a;
}

ast *ast_create_leaf (char *value, ast_type type, symtab *symbol_table, scope *cur_scope) {
	assert(value != NULL);
	assert(type != AST_NULL);
	assert(symbol_table != NULL);
	assert(cur_scope != NULL);

	ast *new_leaf;
	malloc_checked(new_leaf);

	/* fill out leaf */
	new_leaf->node_type = AST_NODE_LEAF;
	new_leaf->type = type;
	new_leaf->containing_scope = cur_scope;

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

ast **ast_create_node_spawn( ast **a, ast_list *children,
	struct thread_data *thread, symtab *st, scope *cur_scope ) {
	(*a)->data.spawn.body = NULL;//children->data;
	(*a)->data.spawn.arguments = NULL;//children->next->data;

	/* get pointer to thread being used */
	struct thread_data * td = threadtab_lookup(symtab_get_threadtab(st),
			symtab_entry_get_name(children->next->data->data.symtab_ptr));
	
	assert (td != NULL);

	(*a)->data.spawn.thread = td;

	/* prepare function call */
	char func_name[MAX_IDENT_LENGTH + 1];
	snprintf(func_name, sizeof func_name, SPAWN_FUNC_FORMAT, td->offset);
	symtab_insert(st, func_name, AST_VOID_STAR, ST_STATIC_DEC);

	ast_list *body;
	ast_list *arguments;
	heap_list_malloc(hList, body);
	heap_list_malloc(hList, arguments);

	body->data = children->data;
	body->next = arguments;
	arguments->data = NULL;
	arguments->next = NULL;

	ast *new_func = ast_add_internal_node(func_name, body, AST_NODE_FUNCTION_DEF,
		st, cur_scope);

	heap_list_add(hList, (void *)new_func);

	new_func->data.func_def.thread_generated = 1;
	new_func->data.func_def.convert_to_ptr = 1;

	threadtab_add_assoc_func(td, new_func);

	return a;
}

ast **ast_create_node_barrier(ast **a, symtab *st){
	(*a)->data.barrier.thread_table = symtab_get_threadtab(st);
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
	(*a)->data.func_call.func_symtab = symtab_lookup(symbol_table, name, cur_scope);

	(*a)->type = symtab_entry_get_type((*a)->data.func_call.func_symtab);
	
	#ifdef AST_DEBUG
	fprintf(stderr, "created func call node with child %p address\n",
		(*a)->data.func_call.arguments);
	#endif

	return a;
}


ast **ast_create_node_stmt(ast **a, ast_list *children){
	(*a)->data.stmt.body = children->data;
	(*a)->data.stmt.next = children->next->data;

	(*a)->type = (*a)->data.stmt.body->type;

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
 *	AST_NODE_STATEMENT:			IGNORED
 *	AST_NODE_FUNCTION_CALL:		name of function being called
 *	AST_NODE_FUNCTION_LIST:		IGNORED
 *	AST_NODE_CONDITIONAL:		IGNORED
 *  AST_NODE_WHILE:				IGNORED
 *	AST_NODE_SPAWN:				ptr to struct thread_Data created by create_thread_data
 *	AST_NODE_BARRIER:			IGNORED
 *	AST_NODE_UNARY:				1-2 character unary op
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
 *	
 *	Returns NULL on error
 */
ast *ast_add_internal_node (char *value, ast_list *children, ast_node_type type,
		symtab *symbol_table, scope *cur_scope){
	assert(children != NULL || type == AST_NODE_BARRIER);
	ast *new_node;
	malloc_checked(new_node);

	#ifdef AST_DEBUG
	fprintf(stderr, "NODE VALUE FIELD: %s; ADDR: %p\n", value, new_node);
	#endif

	/* fill out node */
	new_node->type = AST_NULL;
	new_node->node_type = type;
	new_node->containing_scope = cur_scope;

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
			ast_create_node_stmt(&new_node, children);
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
			ast_create_node_spawn(&new_node, children, (struct thread_data*) value,
				symbol_table, cur_scope);
			break;

		case AST_NODE_BARRIER:
			ast_create_node_barrier(&new_node, symbol_table);
			break;

		case AST_NODE_UNARY:
			assert(strlen(value) < 3);
			ast_create_node_unary(&new_node, value, children );
			break;

		default:
			break;
	}

	return new_node;
}


void ast_destroy_helper(struct ast_s *tree);

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
			if (tree->type == AST_INTLITERAL){
				free(tree->data.integer);
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
			ast_destroy_helper(tree->data.spawn.arguments);
			ast_destroy_helper(tree->data.spawn.body);
			break;

		case AST_NODE_UNARY:
			ast_destroy_helper(tree->data.unary.operand);
			break;

		default:
			assert(1);
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
