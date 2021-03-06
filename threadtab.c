
/*
 *	Module for creating a table holding information necessary to generate
 * 	threading constructs for multafila, a language focusing on making 
 *	multithreading easy and intuitive.
 *	
 *	Chae Jubb
 *	
 */

#include <string.h>
#include <assert.h>

#include "include/global_config.h"
#include "include/error_handling.h"
#include "include/mem_manage.h"
#include "include/threadtab.h"
#include "include/threadtab_structs.h"
#include "include/ast.h"

/*
 *	Create thread_data object to be used in other functions
 *	Returns negative on error
 */
struct thread_data *create_thread_data(char *name, uint64_t length){
	struct thread_data *newTD;
	malloc_checked(newTD);
	memset(newTD, 0, sizeof(*newTD));

	strncpy(newTD->name, name, MAX_IDENT_LENGTH);
	newTD->length = length;

	return newTD;
}

/*
 *	Create new thread table
 *	Returns the created thread table; NULL on error
 */
threadtab *threadtab_init(){
	/* create new tb and set to zero */
	struct threadtab_s *newTB;
	malloc_checked(newTB);
	memset(newTB, 0, sizeof(*newTB));

	return newTB;
}

int threadtab_insert_helper(threadtab *thread_table, struct thread_data *data, int count){
	if (count == 0) {
		free(data);
		return 0;
	}

	data->offset = thread_table->length;
	++thread_table->length;

	if (thread_table->head == NULL){
		thread_table->head = (thread_table->tail = data);
	} else {
		thread_table->tail->next = data;
		thread_table->tail = data;
	}
	data->next = NULL;

	/* copy thread_data */
	struct thread_data *new_data;
	malloc_checked(new_data);
	memcpy(new_data, data, sizeof (*new_data));
	threadtab_insert_helper(thread_table, new_data, count - 1);

	return 0;
}

/*
 *	Insert thread data into thread table
 *	Returns negative on error
 */
int threadtab_insert(threadtab *thread_table, struct thread_data *data){
	assert(thread_table != NULL);
	assert(data != NULL);

	threadtab_insert_helper(thread_table, data, data->length);

	return 0;
}

/*
 *	Lookup entry in thread table by name
 *	Returns pointer to struct thread_data; NULL if not found
 */
struct thread_data *threadtab_lookup(threadtab *thread_table, char *name){
	struct thread_data *cur = thread_table->head;
	while(cur != NULL && strcmp(cur->name, name) != 0){
		cur = cur->next;
	}

	return cur;
}

/*
 *	Add function associated with thread
 *	Returns pointer to modified struct thread_data
 */
struct thread_data *threadtab_add_assoc_func(struct thread_data *td, ast *a){
	td->assoc_ast = a;
	return td;
}

/*
 *	Destroy thread table
 *	Returns negative on error
 */
int threadtab_destroy(threadtab *thread_table){
	assert(thread_table != NULL);

	struct thread_data *old = thread_table->head, *new;
	while(old != NULL){
		new = old->next;
		free(old);
		old = new;
	}

	free(thread_table);

	return 0;
}
