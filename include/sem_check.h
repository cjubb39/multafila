#ifndef SEM_CHECK_H
#define SEM_CHECK_H
void sem_check(ast *a);

extern symtab *st;
extern int errorcount;
extern ast* fuction_def_node;
extern ast* function_list;

#endif