#ifndef GLOBAL_CONFIG_H
#define GLOBAL_CONFIG_H

//#define GLOBAL_DEBUG

#ifdef GLOBAL_DEBUG
#define GEN_TEST_DEBUG
#define AST_DEBUG
#define SYMTAB_DEBUG
#define THREADTAB_DEBUG
#define PARSER_DEBUG
#endif


#define MAX_IDENT_LENGTH 32

typedef enum {
	AST_STRINGLITERAL,
	AST_STRING,
	AST_INT,
	AST_NULL
} ast_type;

#endif
