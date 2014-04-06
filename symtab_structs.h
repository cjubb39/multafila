#ifndef SYMTAB_STRUCTS_H
#define SYMTAB_STRUCTS_H value

/*
 *	NOTE changing this value may also require changing 
 *	return width in symtab#symtab_entry_hash
 */
#define SYMTAB_SCOPE_ARRAY_SIZE 1009

#include "symtab.h"

/*declarations for internal structs, enums, etc */
struct symtab_entry {

	struct symtab_entry *next;
};

struct symtab_scope {
	enum symtab_scope_type_e type;
	struct symtab_scope *parent;
	struct symtab_scope *sibling;
	struct symtab_scope *child;
	struct symtab_entry *data[SYMTAB_SCOPE_ARRAY_SIZE];
};

struct symtab_s {
	struct symtab_scope *head;
	struct symtab_scope *cur_scope;
};

#endif
