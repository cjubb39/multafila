#include "ast.h"
#include "y.tab.h"

int type_enum2yacc(ast_type type){
	int ret;
	switch(type){
		case AST_STRINGLITERAL:
			ret = STRINGLITERAL;
			break;

		case AST_STRING:
			ret = STRING;
			break;

		default:
			break;
	}

	return ret;
}

ast_type type_yacc2enum(int type){
	ast_type ret;

	switch(type){
		case STRINGLITERAL:
			ret = AST_STRINGLITERAL;
			break;

		case STRING:
			ret = AST_STRING;
			break;

		default:
			break;
	}

	return ret;
}
