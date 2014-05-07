#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "include/global_config.h"
#include "include/error_handling.h"
#include "include/mem_manage.h"
#include "include/ast.h"
#include "include/symtab.h"

//#define AST_DEBUG

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
	(*a)->data.integer = malloc_checked_string(strlen(value) + 1);
	strcpy((*a)->data.integer, value);
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

ast **ast_create_leaf_barrier ( ast **a, char *value ) {
	/* not yet implemented */
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

		case AST_BARRIER:
			ast_create_leaf_barrier(&new_leaf, value );
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

ast **ast_create_node_spawn( ast **a, ast_list *children ) {
	(*a)->data.spawn.body = children->data;
	(*a)->data.spawn.arguments = children->next->data;

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

	#ifdef AST_DEBUG
	fprintf(stderr, "FUNC DEF ARGUMENTS: base: %p data: %p", children, children->data);
	#endif

	symtab_entry *tmp = symtab_lookup(symbol_table, name, cur_scope);

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
 *	AST_NODE_SPAWN:				IGNORED
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
 *	AST_NODE_UNARY:			operand
 *	
 *	Returns NULL on error
 */
ast *ast_add_internal_node (char *value, ast_list *children, ast_node_type type,
		symtab *symbol_table, scope *cur_scope){
	assert(children != NULL);
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
			ast_create_node_spawn(&new_node, children );
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
