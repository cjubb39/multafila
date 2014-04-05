#include "global_config.h"

/*
 *	Store symbol name, data type, whether initialized, scope
 *
 */

typedef struct symtab_s symtab;

/*
 *	Initialize symbol table
 *	Returns pointer to initialized symtab
 */
symtab symtab_init(void);

/*
 *	Destroy symbol table
 *	Returns negative on error
 */
int symtab_destroy(symtab symbol_table);

/*
 *	Create a new scope
 *	Returns negative on error
 */
int symtab_open_scope(symtab symbol_table);

/*
 * Closes current scope
 *  Returns negative on error
 */
int symtab_close_scope(symtab symbol_table);

/* 
 *	Takes symbol name and types, as integer defined in yacc
 *	Returns negative on error
 */
int symtab_insert(symtab symbol_table, char *symbol_name, int type);

/*
 *	Takes symbol name and returns a type, as integer defined in yacc
 *	Returns NULL on error (including not found)
 */
int symtab_lookup(symtab symbol_table, char *symbol_name);

/*
 *	Checks if symbol was declared inside the current thread block (pfor or spawn)
 *	Returns 1 on true; 0 on false
 */
int symtab_declared_curr_thread_block(symtab symbol_table, char *symbol_name);
