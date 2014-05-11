
/*
 *	Header for module for easy memory management for multafila, a language
 *	focusing on  making multithreading easy and intuitive, whose compilation
 *	process involves many heap allocations.
 *	
 *	Chae Jubb
 *	
 */

#ifndef MEM_MANAGE_H
#define MEM_MANAGE_H

#include <string.h>

typedef struct heap_list_node_s {
	struct heap_list_node_s *next;
	void *data;
} heap_list;

typedef struct heap_list_head_s {
	struct heap_list_node_s *head;
	struct heap_list_node_s *tail;
} heap_list_head;

heap_list_head* heap_list_init(heap_list_head*);
heap_list_head* heap_list_add(heap_list_head*, void*);
heap_list_head* heap_list_purge(heap_list_head*);

/*
 *	Check malloc return value
 *	Exits if malloc returns NULL, otherwise passes malloc return value
 */
#define malloc_checked(name) ({ \
 	name = (typeof(name)) malloc(sizeof(*name)); \
 	if (name == NULL) die("Malloc Error. Exiting."); \
 	name;})

#define heap_list_malloc(hl_head, data) ({\
	malloc_checked(data); \
	heap_list_add(hl_head, data); \
	data;})

/*
 *	Check malloc return value of a string/int allocation
 */
char *malloc_checked_string(size_t size);
int *malloc_checked_int(size_t size);
#endif