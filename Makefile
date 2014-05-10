CC  = gcc

CFLAGS	= -g3 -O0 -std=gnu89 -Wall $(INCLUDES)
LDFLAGS	= -lm

.PHONY: default
default: small-parser

SRC	= ast.c							\
			error_handling.c	\
			gen_code.c				\
			mem_manage.c			\
			symtab.c					\
			threadtab.c				\
			lex.yy.c					\
			y.tab.c						

HEADERS = $(wildcard include/*.h)

# header dependency
small-parser: $(SRC) $(HEADERS)
	$(CC) $(CFLAGS) $(SRC) $(LDFLAGS) -o small-parser

y.tab.c: parser.y
	yacc -d $<

lex.yy.c: lexer.l
	lex $<

.PHONY: clean
clean:
	rm -f *.o *~ a.out core small-parser lex.yy.c y.tab.c y.tab.h

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
