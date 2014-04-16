#ifndef AST_STRUCTS_H
#define AST_STRUCTS_H

#include "global_config.h"
#include "symtab.h"

struct ast_function_list_node{
	struct ast_s *cur_func;
	struct ast_s *next_func;
};

struct ast_function_def_node {
	symtab_entry *func_symtab;
	struct ast_s *body;
	struct ast_list_s *arguments;
};

struct ast_function_call_node {
	symtab_entry *func_symtab;
	struct ast_list_s *arguments;
};

struct ast_declaration_node {
	struct ast_s *var;
};

struct ast_binary_node {
	char op[2];
	struct ast_s *left;
	struct ast_s *right;
};

struct ast_statement_node {
	struct ast_s *body;
	struct ast_s *next;
};

#endif
