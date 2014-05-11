
/*
 *	Global configuration header file for multafila, a language focusing on
 *	making multithreading easy and intuitive.
 *	
 *	Chae Jubb
 *	
 */

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
	AST_INTLITERAL,
	AST_INTARRAY,
	AST_CHAR,
	AST_CHARLITERAL,
	AST_CHARARRAY,
	AST_BOOLEAN,
	AST_BOOLEANLITERAL,
	AST_NULL,
	AST_THREAD,
	AST_THREADARRAY,
	AST_VOID_STAR,
	AST_VOID
} ast_type;

#define YYSTYPE char *

#endif
