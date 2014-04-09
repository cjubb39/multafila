#include <stdio.h>
#include <stdlib.h>

/*
 *	Exit with error message
 */
inline void die(const char *message);

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
