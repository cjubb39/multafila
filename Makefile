CC  = gcc
CXX = g++


CFLAGS   = -g -Wall $(INCLUDES)
CXXFLAGS = -g -Wall $(INCLUDES)


.PHONY: default
default: small-parser

# header dependency
small-parser: y.tab.c lex.yy.c
	$(CC) $(CFLAGS) y.tab.c lex.yy.c -ll
y.tab.c: practice.y
	yacc -d practice.y
lex.yy.c: practice.l
	lex practice.l

.PHONY: clean
clean:
	rm -f *.o *~ a.out core small-parser lex.yy.c y.tab.c y.tab.h

.PHONY: all
all: clean default


symtab_test: symtab_test.c symtab.c
