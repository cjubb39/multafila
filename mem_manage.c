#include <stdlib.h>
#include "include/mem_manage.h"
#include "include/error_handling.h"

/*
 *	Check malloc return value of a string allocation
 */
char *malloc_checked_string(size_t size){
	char *tmp = malloc(size * sizeof(char));
	if (tmp == NULL)
		die("Malloc Error. Exiting.");

	return tmp;
}

heap_list_head* heap_list_init(heap_list_head *head){
	head->head = NULL;
	head->tail = NULL;

	return head;
}

heap_list_head* heap_list_add(heap_list_head *head, void *data){
	struct heap_list_node_s *tmp;
	malloc_checked(tmp);
	tmp->next = NULL;
	tmp->data = data;

	if (head->head == NULL){
		head->head = tmp;
	} else {
		head->tail->next = tmp;
	}

	head->tail = tmp;

	return head;
}

heap_list_head* heap_list_purge(heap_list_head *head){
	struct heap_list_node_s *cur = head->head, *next;

	while(cur != NULL){
		next = cur;
		free(cur->data);
		free(cur);
		cur = next;
	}

	head->head = NULL;
	head->tail = NULL;

	return head;
}
