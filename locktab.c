
/*
 *	Module for creating a table holding information necessary to generate
 * 	lock statement for multafila, a language focusing on making multithreading
 *	easy and intuitive.
 *	
 *	Chae Jubb
 *	
 */

#include <string.h>
#include <assert.h>

#include "include/global_config.h"
#include "include/error_handling.h"
#include "include/mem_manage.h"
#include "include/locktab.h"
#include "include/ast.h"

/*
 *	Create new lock table
 *	Returns the created lock table; NULL on error
 */
locktab *locktab_init(){
	/* create new tb and set to zero */
	struct locktab_s *newLT;
	malloc_checked(newLT);
	memset(newLT, 0, sizeof(*newLT));

	return newLT;
}

/*
 *	Insert lock data into lock table
 *	Returns LTI_VALID on insertion; LTI_ALREADYEXISTS on already exists
 */
int locktab_insert(locktab *lock_table, void *data){
	assert(lock_table != NULL);
	assert(data != NULL);

	/* check if already in table */
	struct locktab_data *tmp = lock_table->head;
	while(tmp != NULL){
		if(tmp->ptr == data){
			return LTI_ALREADYEXISTS;
		}
		tmp = tmp->next;
	}

	/* adding to table now */
	struct locktab_data *newLT;
	malloc_checked(newLT);
	memset(newLT, 0, sizeof(*newLT));
	newLT->ptr = data;

	if (lock_table->head == NULL){
		lock_table->head = (lock_table->tail = newLT);
	} else {
		lock_table->tail->next = newLT;
		lock_table->tail = newLT;
	}

	return 0;
}

/*
 *	Destroy lock table
 *	Returns negative on error
 */
int locktab_destroy(locktab *lock_table){
	assert(lock_table != NULL);

	struct locktab_data *old = lock_table->head, *new;
	while(old != NULL){
		new = old->next;
		free(old);
		old = new;
	}

	free(lock_table);

	return 0;
}
