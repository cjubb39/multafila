
/*
 *	Module for general error handling functions for multafila, a language
 *	focusing on making multithreading easy and intuitive.
 *	
 *	Chae Jubb
 *	
 */

#include <stdio.h>
#include <stdlib.h>

#include "include/error_handling.h"

/*
 *	Exit with error message
 */
inline void die(const char *message){
	perror(message);
	exit(1);
}
