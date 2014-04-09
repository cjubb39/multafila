#include <stdio.h>
#include <stdlib.h>

#include "error_handling.h"

/*
 *	Exit with error message
 */
inline void die(const char *message){
	perror(message);
	exit(1);
}

/*
 *	Check malloc return value of a string allocation
 */
char *malloc_checked_string(size_t size){
	char *tmp = malloc(size * sizeof(char));
	if (tmp == NULL)
		die("Malloc Error. Exiting.");

	return tmp;
}
