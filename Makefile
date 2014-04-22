CC  = gcc
CXX = g++


CFLAGS   = -g -Wall $(INCLUDES)
CXXFLAGS = -g -Wall $(INCLUDES)


.PHONY: default
default: small-parser

# header dependency
small-parser: y.tab.c lex.yy.c
	$(CC) $(CFLAGS) ast.c symtab.c error_handling.c y.tab.c lex.yy.c -ll -o small-parser
y.tab.c: practice2.y
	yacc -d practice2.y
lex.yy.c: practice2.l
	lex practice2.l

.PHONY: clean
clean:
	rm -f *.o *~ a.out core small-parser

.PHONY: all
all: clean default




symtab_test: symtab_test.c symtab.c error_handling.c

threadtab_test: threadtab_test.c threadtab.c error_handling.c

ast_test: ast_test.c ast.c symtab.c error_handling.c gen_test.c ast.h ast_structs.h

a_test: ast_test a_val

a_val:
	valgrind --leak-check=full ./ast_test

sttt_test:
	gcc -g -Wall symtab_test.c symtab.c error_handling.c -o symtab_test
	gcc -g -Wall threadtab_test.c threadtab.c error_handling.c -o threadtab_test
	valgrind --leak-check=full ./symtab_test
	valgrind --leak-check=full ./threadtab_test
