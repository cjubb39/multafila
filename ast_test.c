#include "include/global_config.h"
#include "include/error_handling.h"
#include "include/ast.h"
#include "include/symtab.h"

#include "include/gen_test.h"

int main(void){
	symtab *st = symtab_init();

	/* pre-install printOut */
	symtab_insert(st, "printOut", AST_STRING);

	scope *cur_scope = symtab_open_scope(st, SCOPE_FUNCTION);


	/* create variable named var of type string */
	symtab_insert(st, "var", AST_STRING);

	ast *var = ast_create_leaf("var", AST_STRING, st, cur_scope);

	ast_list dec;
	dec.data = var;
	dec.next = NULL;

	ast *var1 = ast_add_internal_node(NULL, &dec, AST_NODE_DECLARATION, st, cur_scope);

	/* create string literal with value "Hello, World!" */
	ast *hw = ast_create_leaf("Hello, World!", AST_STRINGLITERAL, st, cur_scope);

	/* set var = "Hello, World!" */
	ast_list lh, rh;
	lh.data = var1;
	lh.next = &rh;
	rh.data = hw;
	rh.next = NULL;
	char op[2] = {'=', '\0'};
	ast *var_set = ast_add_internal_node(op, &lh, AST_NODE_BINARY, st, cur_scope);

	printf("EXPECTED: %p %s %p\n", var1, op, hw);


	/* now we do printOut(var); */
	ast *var1_2 = ast_create_leaf("var", AST_STRING, st, cur_scope);
	ast_list arg_c;
	arg_c.data  = var1_2;
	arg_c.next  = NULL;

	ast *po = ast_add_internal_node("printOut", &arg_c, AST_NODE_FUNCTION_CALL, st, cur_scope);


	ast_list stmtcur2, snext2;
	stmtcur2.data = po;
	stmtcur2.next = &snext2;
	snext2.data = NULL;
	snext2.next = NULL;

	ast *stmt2 = ast_add_internal_node(NULL, &stmtcur2, AST_NODE_STATEMENT, st, cur_scope);
	printf("EXPECTED: %p %p\n", po, NULL);



	
	ast_list stmtcur, snext;
	stmtcur.data = var_set;
	stmtcur.next = &snext;
	snext.data = stmt2;
	snext.next = NULL;

	ast *stmt = ast_add_internal_node(NULL, &stmtcur, AST_NODE_STATEMENT, st, cur_scope);
	printf("EXPECTED: %p %p\n", var_set, stmt2);

	/* here we have
              /------------------\
	string var = "Hello, World!\n"  \
	                             printOut(var);
	*/


	ast_list body, arg;
	body.data = stmt;
	body.next = &arg;
	arg.data  = NULL;
	arg.next  = NULL;

	symtab_insert(st, "main", AST_INT);
	ast *func_def_out = ast_add_internal_node("main", &body, AST_NODE_FUNCTION_DEF, st, cur_scope);


	ast_list flist_cur, flist_next;
	flist_cur.data = func_def_out;
	flist_cur.next = &flist_next;
	flist_next.data = NULL;
	flist_next.next = NULL;

	ast *program = ast_add_internal_node(NULL, &flist_cur, AST_NODE_FUNCTION_LIST, st, cur_scope);


	printf("\n\n==========" \
		"GENERATING PROGRAM OUTPUT==========\n\n");

	gen_test(program);

	ast_destroy(program);

	symtab_close_scope(st);
	symtab_destroy(st);

	return 0;
}
