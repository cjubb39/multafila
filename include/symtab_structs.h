#ifndef SYMTAB_STRUCTS_H
#define SYMTAB_STRUCTS_H

/*
 *	NOTE changing this value may also require changing 
 *	return width in symtab#symtab_entry_hash
 */
#define SYMTAB_SCOPE_ARRAY_SIZE 1009

#include "global_config.h"
#include "symtab.h"
#include "ast.h"
#include "threadtab.h"

/*declarations for internal structs, enums, etc */
struct symtab_entry_s {
	char name[MAX_IDENT_LENGTH + 1];
	ast_type type;
	void *ptr;
	/* gives us bit flags */
	unsigned initialized	:1;
	unsigned thread				:1;
	unsigned							:6;

	struct symtab_entry_s *next;
};

struct symtab_scope {
	enum symtab_scope_type_e type;
	struct symtab_scope *parent;
	struct symtab_scope *sibling;
	struct symtab_scope *child;
	struct symtab_entry_s *data[SYMTAB_SCOPE_ARRAY_SIZE];
};

struct symtab_s {
	struct symtab_scope *head;
	struct symtab_scope *cur_scope;
	threadtab *thread_table;
};

#endif
