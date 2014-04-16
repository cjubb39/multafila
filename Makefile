CC  = gcc
CXX = g++


CFLAGS   = -g -Wall $(INCLUDES)
CXXFLAGS = -g -Wall $(INCLUDES)


.PHONY: default
default: small-parser

# header dependency
small-parser: y.tab.c lex.yy.c
	$(CC) $(CFLAGS) y.tab.c lex.yy.c -ll -o small-parser
y.tab.c: practice2.y
	yacc -d practice2.y
lex.yy.c: practice2.l
	lex practice2.l

.PHONY: clean
clean:
	rm -f *.o *~ a.out core small-parser

.PHONY: all
all: clean default




symtab_test: symtab_test.c symtab.c

threadtab_test: threadtab_test.c threadtab.c
