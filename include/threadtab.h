/*
 *	Holds information on threads
 *
 */

#ifndef THREADTAB_H
#define THREADTAB_H

#include <stdint.h>
#include "global_config.h"
#include "threadtab_structs.h"

typedef struct threadtab_s threadtab;

/*
 *	Create thread_data object to be used in other functions
 *	Returns negative on error
 */
struct thread_data *create_thread_data(char *name, uint64_t length);

/*
 *	Create new thread table
 *	Returns the created thread table; NULL on error
 */
threadtab *threadtab_init();

/*
 *	Insert thread data into thread table
 *	Returns negative on error
 */
int threadtab_insert(threadtab *thread_table, struct thread_data *data);

/*
 *	Destroy thread table
 *	Returns negative on error
 */
int threadtab_destroy(threadtab *thread_table);

#endif
