#ifndef AST_STRUCTS_H
#define AST_STRUCTS_H

#include "global_config.h"
#include "symtab.h"
#include "threadtab.h"

struct ast_function_list_node{
	struct ast_s *cur_func;
	struct ast_s *next_func;
};

struct ast_function_def_node {
	symtab_entry *func_symtab;
	struct ast_s *body;
	struct ast_list_s *arguments;
	void *assoc_spawn_info;

	unsigned thread_generated	:1;
	unsigned convert_to_ptr		:1;
	unsigned									:6;
};

struct ast_function_call_node {
	symtab_entry *func_symtab;
	struct ast_list_s *arguments;
};

struct ast_declaration_node {
	struct ast_s *var;
};

struct ast_binary_node {
	char op[3];
	struct ast_s *left;
	struct ast_s *right;
};

struct ast_statement_node {
	struct ast_s *body;
	struct ast_s *next;
	unsigned int braced;
};

struct ast_conditional_node {
	struct ast_s *conditional_statement;
	struct ast_s *if_statement;
	struct ast_s *else_statement;
};

struct ast_while_node {
	struct ast_s *conditional_statement;
	struct ast_s *body;
};

struct ast_for_node {
	struct ast_s *assignment;
	struct ast_s *relexpr;
	struct ast_s *unary;
	struct ast_s *body;	 
};

struct ast_spawn_node {
	struct ast_s *arguments;
	struct ast_s *body;
	struct thread_data *thread;
	struct ast_spawn_vars{
		struct ast_list_s *old_vars;
		struct ast_list_s *new_vars;
		struct ast_s *counter;
	} vars;
	unsigned int native_spawn	:1;
	unsigned int							:7;
};

struct ast_barrier_node {
	threadtab *thread_table;
};

struct ast_unary_math {
	char op[3];
	struct ast_s *operand;
};

struct ast_return_node {
	struct ast_s *value;
};

struct ast_lock_node {
	struct ast_s* body;
	struct ast_list_s *params;
};

#endif
