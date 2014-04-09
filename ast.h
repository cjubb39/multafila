#include "global_config.h"
#include "y.tab.h"
#include <string.h>
#include <stdint.h>

typedef enum {
	assign,
} ast_node_type;

typedef enum {
	string, stringliteral,
} ast_type;

typedef struct ast_s {
	ast_type leaf_type;
	ast_node_type node_type;
	union {
		int integer;
		char *string;
		double doub;
		struct { char *oper;
				 struct ast *left;
				 struct ast *right; } binary;
	} x;
} ast;

typedef struct ast_list ast_list;

struct ast_list {
	ast *data;
	ast_list *next;
};


/*
 *	Create leaft note with value and type given
 *	Returns NULL on error
 */
ast *ast_create_leaf (char *value, ast_type type);

/*
 *	Add internal (non-leaf) node to ast
 *	Action will be specified by value of type argument
 *	Returns NULL on error
 */
ast *ast_add_internal_node (struct ast_list *children, ast_node_type type);

/*
 *	Return ast_type of node
 *	Note this type is different than ast_node_type
 *	Returns NULL on error
 */
ast_type ast_getType(ast ast_to_type);

/*
 *  Returns value of node
 *	Return casted to uint64_t to allow for 64 bit data to be passed
 */
uint64_t ast_getValue(ast ast_to_value);

/*
 *	Destroy Abstract Syntax Tree created with ast_create_leaf
 *	Returns negative on error
 */
int ast_destroy(ast);
