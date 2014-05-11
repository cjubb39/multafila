
/*
 *	Header for module for generating target code (C code) for multafila, a
 *	language focusing on making multithreading easy and intuitive.  Assumes
 *	only an implementation of POSIX Threads.
 *	
 *	Chae Jubb
 *	
 */

#ifndef GEN_CODE_H
#define GEN_CODE_H
void gen_code(ast *, symtab*, threadtab*, locktab*);

#endif
