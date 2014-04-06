#include <stdlib.h>
#include <string.h>

#include <assert.h>

#include "global_config.h"
#include "errorHandling.h"
#include "symtab.h"
#include "symtab_structs.h"

/* FUNCTIONS FOR BUILDING SYMBOL TABLE */

void symtab_create_section(struct symtab_scope **newScope, enum symtab_scope_type_e type,
		struct symtab_scope *parent){
	assert(newScope != NULL);

	malloc_checked(*newScope);
	/* set struct to NULL so we can rely on NULL pointer in future */
	memset(*newScope, 0, sizeof(**newScope));
	
	/* initialize actual data of section */
	(*newScope)->type = type;
	(*newScope)->parent = parent;
}

/*
 *	Initialize symbol table
 *	Returns pointer to initialized symtab
 */
symtab *symtab_init(){
	symtab *newST;
	malloc_checked(newST);
	

	/* initialize global section */
	symtab_create_section(&newST->head, GLOBAL, NULL);

	/* set current scope to global */
	newST->cur_scope = newST->head;

	return newST;
}

void symtab_destroy_helper(struct symtab_scope *scope){
	/* base case */
	if (scope == NULL){
		return;
	}

	/* destroy children and then siblings */
	symtab_destroy_helper(scope->child);
	symtab_destroy_helper(scope->sibling);

	/* after all children and siblings are destroyed, we can destroy scope itself */
	int i;
	for (i = 0; i < SYMTAB_SCOPE_ARRAY_SIZE; ++i){

		/* free linked list at each hash bucket */
		struct symtab_entry *old = scope->data[i], *new;
		while (old != NULL){
			new = old->next;
			free(old);
			old = new;
		}
	}

	free(scope);
}

/*
 *	Destroy symbol table
 *	Returns negative on error
 */
int symtab_destroy(symtab *symbol_table){
	assert(symbol_table != NULL);
	
	/* need to clear my head */
	symtab_destroy_helper(symbol_table->head);

	free(symbol_table);

	return 0;
}

/*
 *	Create a new scope and moves into it.
 *	Returns new current scope.  NULL on error.
 */
scope *symtab_open_scope(symtab *symbol_table, symtab_scope_type type){
	assert(symbol_table != NULL);

	struct symtab_scope *cur_scope = symbol_table->cur_scope;

	/* create new scope */
	struct symtab_scope *new_scope;
	symtab_create_section(&new_scope, type, cur_scope);

	/* insert into structure */
	if (cur_scope->child == NULL){
		cur_scope->child = new_scope;
	
	/* add to end of this list if cur_scope already has children */
	} else {
		struct symtab_scope *tmp = cur_scope->child;
		while (tmp->sibling != NULL){
			tmp = tmp->sibling;
		}
		tmp->sibling = new_scope;
	}

	/* set new current scope */
	symbol_table->cur_scope = new_scope;

	return symbol_table->cur_scope;
}

/*
 *	Closes current scope
 *  Returns new current scope.  NULL on error.
 */
scope *symtab_close_scope(symtab *symbol_table){
	assert(symbol_table != NULL);
	assert(symbol_table->cur_scope != symbol_table->head);

	symbol_table->cur_scope = symbol_table->cur_scope->parent;

	return symbol_table->cur_scope;
}
