CC  = gcc

CFLAGS	= -g3 -O0 -std=gnu89 -Wall $(INCLUDES)
LDFLAGS	= -lm

.PHONY: default
default: parser

SRC	= ast.c							\
			error_handling.c	\
			gen_code.c				\
			locktab.c					\
			mem_manage.c			\
			sem_check.c 			\
			symtab.c					\
			threadtab.c				\
			lex.yy.c					\
			y.tab.c					\

HEADERS = $(wildcard include/*.h)

# header dependency
parser: $(SRC) $(HEADERS)
	$(CC) $(CFLAGS) $(SRC) $(LDFLAGS) -o parser

y.tab.c: parser.y
	yacc --defines=include/y.tab.h $<

lex.yy.c: lexer.l
	lex $<

.PHONY: test
test:
	./testall.sh

.PHONY: clean
clean:
	rm -f *.o *~ a.out core parser lex.yy.c y.tab.c include/y.tab.h

.PHONY: all
all: clean default
