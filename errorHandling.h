#include <stdio.h>
#include <stdlib.h>

/*
 *	Exit with error message
 */
inline void die(const char *message){
	perror(message);
	exit(1);
}

/*
 *	Check malloc return value
 *	Exits if malloc returns NULL, otherwise passes malloc return value
 */
#define malloc_checked(name) ({ \
 	name = (typeof(name)) malloc(sizeof(*name)); \
 	if (name == NULL) die("Malloc Error. Exiting."); \
 	name;})



/*inline void *malloc_checked(size_t size){
	void *tmp = malloc(size);
	if (tmp == NULL){
		die("Malloc Error. Exiting.");
	}

	return tmp;
}*/