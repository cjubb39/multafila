#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#include "global_config.h"
#include "errorHandling.h"
#include "symtab.h"
#include "symtab_structs.h"
#include "y.tab.h"

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
	symtab_create_section(&newST->head, SCOPE_GLOBAL, NULL);

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


/* FUNCTIONS FOR ACCESSING SYMBOL TABLE */

/*
 *	Function that determines which bucket the symtab_entry will be placed in.
 *	Uses algorithm djb2 and mods result by size of hash table
 */
uint16_t symtab_entry_hash(char *symbol_name){
	assert(strlen(symbol_name) <= MAX_IDENT_LENGTH);

	unsigned long hash = 5381;
	int c;

	while ((c = *symbol_name++))
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

	return (hash % SYMTAB_SCOPE_ARRAY_SIZE);
}

struct symtab_entry *symtab_create_entry(char *symbol_name, int type){
	struct symtab_entry *new_entry;
	malloc_checked(new_entry);

	/* set up defaults */
	memset(new_entry, 0, sizeof(*new_entry));
	strncpy(new_entry->name, symbol_name, MAX_IDENT_LENGTH);
	new_entry->type = type;

	return new_entry;
}

/* 
 *	Takes symbol name and types, as integer defined in yacc
 *	Returns negative on error
 */
int symtab_insert(symtab *symbol_table, char *symbol_name_in, int type){
	assert(symbol_name_in != NULL);
	assert(*symbol_name_in != '\0');
	assert(symbol_table != NULL);
	assert(symbol_table->cur_scope != NULL);

	/* set symbol name to proper length */
	char symbol_name[MAX_IDENT_LENGTH + 1];
	strncpy(symbol_name, symbol_name_in, MAX_IDENT_LENGTH);
	symbol_name[MAX_IDENT_LENGTH] = '\0';

	struct symtab_scope *cur_scope = symbol_table->cur_scope;
	struct symtab_entry **add_point = &cur_scope->data[symtab_entry_hash(symbol_name)];
	struct symtab_entry *new_entry = symtab_create_entry(symbol_name, type);

	if (*add_point == NULL){
		*add_point = new_entry;
	
	/* collision scenario */
	} else {
		struct symtab_entry *tmp = *add_point;
		while(1){

			/* make sure clash isn't with symbol of same name */
			if (strncmp(tmp->name, symbol_name, MAX_IDENT_LENGTH) == 0){
				free(new_entry);
				return STI_ALREADYEXISTS;
			}

			if (tmp->next == NULL)
				break;

			tmp = tmp->next;
		}

		assert(tmp->next == NULL);
		tmp->next = new_entry;
	}

	return ST_VALID;
}

struct symtab_entry *symtab_search_scope(struct symtab_scope *scope, char *symbol_name){
	struct symtab_entry *cur = scope->data[symtab_entry_hash(symbol_name)];

	while(cur != NULL && strncmp(cur->name, symbol_name, MAX_IDENT_LENGTH) != 0){
		cur = cur->next;
	}

	if (cur == NULL){
		return NULL;
	}

	return cur;
}

/*
 *	Takes symbol name and returns a type, as integer defined in yacc
 *	Returns 0 on error (including not found)
 */
int symtab_lookup(symtab *symbol_table, char *symbol_name_in, scope *search_scope){
	assert(symbol_name_in != NULL);
	assert(symbol_table != NULL);
	assert(symbol_table != NULL);

	/* set symbol name to proper length */
	char symbol_name[MAX_IDENT_LENGTH + 1];
	strncpy(symbol_name, symbol_name_in, MAX_IDENT_LENGTH);
	symbol_name[MAX_IDENT_LENGTH] = '\0';

	if(search_scope == NULL)
		search_scope = symbol_table->cur_scope;
	
	struct symtab_entry *result;

	while(search_scope != NULL){
		result = symtab_search_scope(search_scope, symbol_name);
		if (result != NULL)
			break;

		search_scope = search_scope->parent;
	}

	if (search_scope == NULL){
		return 0;
	}

	return result->type;
}

/*
 *	Checks if symbol was declared inside the current thread block (pfor or spawn)
 *	Returns 1 on true; 0 on false
 */
int symtab_declared_curr_thread_block(symtab *symbol_table, char *symbol_name){
	struct symtab_scope *cur = symbol_table->cur_scope;
	int found = 0;

	while(cur != NULL){
		if (symtab_search_scope(cur, symbol_name) != NULL){
			found = 1;
			break;
		}

		if (cur->type == SCOPE_PFOR || cur->type == SCOPE_SPAWN){
			break;
		}

		cur = cur->parent;
	}

	return found;
}
