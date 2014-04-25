#ifndef MEM_MANAGE_H
#define MEM_MANAGE_H

typedef struct heap_list_node_s {
	struct heap_list_node_s *next;
	void *data;
} heap_list;

typdef struct heap_list_head_s {
	struct heap_list_node_s *head;
	struct heap_list_node_s *tail;
} heap_list_head;

heap_list* heap_list_create(heap_list_head*);

/*
 *	Check malloc return value
 *	Exits if malloc returns NULL, otherwise passes malloc return value
 */
#define malloc_checked(name) ({ \
 	name = (typeof(name)) malloc(sizeof(*name)); \
 	if (name == NULL) die("Malloc Error. Exiting."); \
 	name;})

/*
 *	Check malloc return value of a string allocation
 */
char *malloc_checked_string(size_t size);
#endif