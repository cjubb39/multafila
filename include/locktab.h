
/*
 *	Header for module for creating a table holding information necessary to
 * 	generate lock statement for multafila, a language focusing on making
 *	multithreading easy and intuitive.
 *	
 *	Chae Jubb
 *	
 */

#ifndef LOCKTAB_H
#define LOCKTAB_H

#include <stdint.h>
#include "global_config.h"

#define LTI_VALID						0
#define LTI_ALREADYEXISTS		1

struct locktab_data {
	struct locktab_data *next;
	void *ptr;
};

typedef struct locktab_s {
	struct locktab_data *head;
	struct locktab_data *tail;
} locktab;

/*
 *	Create new lock table
 *	Returns the created lock table; NULL on error
 */
locktab *locktab_init();

/*
 *	Insert lock data into lock table
 *	Returns LTI_VALID on insertion; LTI_ALREADYEXISTS on already exists
 */
int locktab_insert(locktab *lock_table, void *data);

/*
 *	Destroy lock table
 *	Returns negative on error
 */
int locktab_destroy(locktab *lock_table);

#endif
