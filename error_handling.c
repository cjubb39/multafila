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
