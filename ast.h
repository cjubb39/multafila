#include "global_config.h"
#include "y.tab.h"
#include <stdint.h>

typedef struct ast_s ast;

struct ast_list {
	ast_s *data;
	ast_list *next;
}

typedef enum {

} ast_node_type;

typedef enum {

} ast_type;

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
ast *ast_add_internal_node (ast *root, struct ast_list *children, ast_node_type type);

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
