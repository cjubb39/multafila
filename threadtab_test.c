#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "global_config.h"
#include "threadtab.h"

/* for testing */
#include "threadtab_structs.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

int main(void){
	threadtab *tb = threadtab_init();

	threadtab_insert(tb, create_thread_data("helloWorld", 7));
	threadtab_insert(tb, create_thread_data("bob", 4));
	threadtab_insert(tb, create_thread_data("chae", 2));
	threadtab_insert(tb, create_thread_data("jubb", 5));
	threadtab_insert(tb, create_thread_data("alpastor", 17));

	/* traverse tb and print out */
	printf("Total Size of Array: %" PRIu64 "\n", tb->length);
	
	struct thread_data *cur = tb->head;
	int i = -1;
	while(cur != NULL){
		printf("%d::%" PRIu64 ":\t", ++i, cur->length);
		printf("%" PRIu64 "\t%s\n", cur->offset, cur->name);

		cur = cur->next;
	}

	threadtab_destroy(tb);

	return 0;
}
