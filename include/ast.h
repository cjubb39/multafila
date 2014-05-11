#ifndef AST_H
#define AST_H

#include <string.h>
#include <stdint.h>

#include "global_config.h"
#include "y.tab.h"
#include "ast_structs.h"
#include "symtab.h"

#define BRACED_STATEMENT_MARKER ((void*)0x06081993)

typedef enum {
	AST_NODE_FUNCTION_LIST,
	AST_NODE_FUNCTION_DEF,
	AST_NODE_FUNCTION_CALL,
	AST_NODE_DECLARATION,
	AST_NODE_BINARY,
	AST_NODE_STATEMENT,
	AST_NODE_LEAF,
	AST_NODE_CONDITIONAL,
	AST_NODE_WHILE,
	AST_NODE_SPAWN,
	AST_NODE_BARRIER,
	AST_NODE_UNARY,
	AST_NODE_NATIVE_CODE,
	AST_NODE_RETURN,
	AST_NODE_LOCK
} ast_node_type;

typedef struct ast_s {
	ast_type type; /* type of the value of the node */
	ast_node_type node_type; /* kind of node */
	scope *containing_scope;
	unsigned int flag;

	union {
		/* for literal values */
		int integer;
		char *string;
		char *character;
		double doub;
		int *intarray;
		char *chararray;
		char **stringarray;
		/* for variable names */
		struct{
			symtab_entry *symtab_ptr;
			unsigned int convert_to_ptr;
		};


		/* internal nodes */
		struct ast_function_list_node func_list;
		struct ast_function_def_node func_def;
		struct ast_function_call_node func_call;
		struct ast_declaration_node dec;
		struct ast_binary_node bin;
		struct ast_statement_node stmt;
		struct ast_conditional_node conditional_statement;
		struct ast_while_node while_statement;
		struct ast_spawn_node spawn;
		struct ast_barrier_node barrier;
		struct ast_unary_math unary;
		struct ast_return_node ret;
		struct ast_lock_node lock;

	} data;
} ast;

struct ast_list_s {
	struct ast_s *data;
	struct ast_list_s *next;
};

typedef struct ast_list_s ast_list;


/*
 *	Create leaft note with value and type given in the given scope
 *	Returns NULL on error
 */
ast *ast_create_leaf (char *value, ast_type type, symtab*, scope* cur_scope);

ast *ast_create_array_leaf (char *value, int size, ast_type type, symtab*, scope* cur_scope);

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
 *	AST_NODE_SPAWN:				ptr to struct thread_Data created by create_thread_data
 *	AST_NODE_BARRIER:			IGNORED
 *	AST_NODE_UNARY:				1-2 character unary op
 *	AST_NODE_NATIVE_CODE:	CODE
 *	AST_NODE_RETURN:			IGNORED
 *	AST_NODE_LOCK:			IGNORED
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
 *	
 *	Returns NULL on error
 */
ast *ast_add_internal_node (char *value, ast_list *children, ast_node_type type, symtab *, scope* cur_scope);

/*
 *	Return ast_type of node
 *	Note this type is different than ast_node_type
 *	Returns NULL on error
 */
ast_type ast_get_type(ast *ast_to_type);

/*
 *	Return ast_type of node
 *	Note this type is different than ast_node_type
 *	Returns NULL on error
 */
ast_node_type ast_get_node_type(ast *ast_to_type);

/*
 *  Returns value of node
 *	Return casted to uint64_t to allow for 64 bit data to be passed
 */
uint64_t ast_getValue(ast *ast_to_value);

/*
 *	Destroy Abstract Syntax Tree created with ast_create_leaf
 *	Returns negative on error
 */
int ast_destroy(ast*);

void ast_walker(struct ast_s *, void*, void(*)(struct ast_s *, void*),
	void(*)(struct ast_list_s *, void*), void(*)(struct ast_s *, void*));

void blank_func(void *, void *);

ast *ast_insert_native_code(ast*, ast*);

#endif
