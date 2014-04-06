#include <string.h>
#include <assert.h>

#include "global_config.h"
#include "error_handling.h"
#include "threadtab.h"
#include "threadtab_structs.h"

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

/*
 *	Insert thread data into thread table
 *	Returns negative on error
 */
int threadtab_insert(threadtab *thread_table, struct thread_data *data){
	assert(thread_table != NULL);
	assert(data != NULL);

	data->offset = thread_table->length;
	thread_table->length += data->length;

	if (thread_table->head == NULL){
		thread_table->head = (thread_table->tail = data);
	} else {
		thread_table->tail->next = data;
		thread_table->tail = data;
	}

	return 0;
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
