#include <stdio.h>
#include <stdlib.h>

#include "symtab.h"
//#include "symtab_structs.h"

#include "y.tab.h"

int main(void){
	symtab *blah = symtab_init();
	printf("created\n");

	scope *s1 = symtab_open_scope(blah, SCOPE_FUNCTION);
	symtab_insert(blah, "jubb", INTEGER);
	
	symtab_open_scope(blah, SCOPE_PFOR);

	symtab_insert(blah, "chae", INTEGER);

	printf("chae curTB?: %d\n", symtab_declared_curr_thread_block(blah, "chae"));
	printf("jubb curTB?: %d\n", symtab_declared_curr_thread_block(blah, "jubb"));

	symtab_open_scope(blah, SCOPE_SPAWN);
	symtab_open_scope(blah, SCOPE_WHILE);

	printf("chae search result 1: %d\n", symtab_lookup(blah, "chae", NULL));

	symtab_close_scope(blah);

	scope *s3 = symtab_open_scope(blah, SCOPE_FOR);

	int i1 = symtab_insert(blah, "chaejubb", INTEGER);
	int i2 = symtab_insert(blah, "chaej", INTEGER);
	int i3 = symtab_insert(blah, "chaejohn", INTEGER);
	int i4 = symtab_insert(blah, "chae", INTEGER);
	printf("%d %d %d %d\n", i1, i2, i3, i4);

	symtab_open_scope(blah, SCOPE_IF);

	symtab_close_scope(blah);
	printf("chae search result 2: %d\n", symtab_lookup(blah, "chae", s1));

	scope *s2 = symtab_open_scope(blah, SCOPE_NORMAL);

	symtab_insert(blah, "alpastor", INTEGER);
	symtab_close_scope(blah);

	printf("alpastor search result 1: %d\n", symtab_lookup(blah, "alpastor", NULL));
	printf("alpastor search result 2: %d\n", symtab_lookup(blah, "alpastor", s2));
	
	symtab_open_scope(blah, SCOPE_NORMAL);
	printf("Five scopes opened\n");

	symtab_destroy(blah);
	printf("destroyed\n");

	return 0;
}