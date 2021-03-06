
/*
 *	Header for module for creating a symbol table for multafila, a language
 *	focusing on making multithreading easy and intuitive.
 *	
 *	Chae Jubb
 *	
 */

#ifndef SYMTAB_H
#define SYMTAB_H value

#include "global_config.h"
#include "threadtab.h"

/* declarations for external structs, enums, etc */
#define STI_VALID					0
#define STI_ALREADYEXISTS	1

#define ST_STATIC_DEC			2
#define ST_NONSTATIC_DEC	3

typedef struct symtab_s symtab;
typedef struct symtab_scope scope;
typedef struct symtab_entry_s symtab_entry;

typedef enum symtab_scope_type_e {
	SCOPE_NORMAL,
	SCOPE_PFOR,
	SCOPE_SPAWN,
	SCOPE_WHILE,
	SCOPE_FOR,
	SCOPE_IF,
	SCOPE_FUNCTION,
	SCOPE_GLOBAL
} symtab_scope_type;

/*
 *	Initialize symbol table
 *	Returns pointer to initialized symtab
 */
symtab *symtab_init();

/*
 *	Destroy symbol table
 *	Returns negative on error
 */
int symtab_destroy(symtab *symbol_table);

/*
 *	Create a new scope
 *	Returns newly opened scope. NULL on error.
 */
scope *symtab_open_scope(symtab *symbol_table, symtab_scope_type);

/*
 *	Closes current scope
 *  Returns new current scope.  NULL on error.
 */
scope *symtab_close_scope(symtab *symbol_table);

/* 
 *	Takes symbol name and types, as integer defined in yacc
 *	Returns STI_VALID on success, non-STI_VALID otherwise.
 */
int symtab_insert(symtab *symbol_table, char *symbol_name, ast_type type, unsigned int, void *ptr);

/*
 *	Takes symbol name and returns a type, as integer defined in yacc
 *	Returns pointer to symtab entry; NULL on error.
 */
symtab_entry *symtab_lookup(symtab *symbol_table, char *symbol_name, scope *current_scope);

/*
 *	Checks if symbol was declared inside the current thread block (pfor or spawn)
 *	Returns 1 on true; 0 on false
 */
int symtab_declared_curr_thread_block(symtab *symbol_table, char *symbol_name);

ast_type symtab_entry_get_type(symtab_entry *entry);
char *symtab_entry_get_name(symtab_entry *entry);
void* symtab_entry_get_ptr(symtab_entry *entry);

void symtab_set_threadtab(symtab *, threadtab *);
threadtab* symtab_get_threadtab(symtab*);

#endif
