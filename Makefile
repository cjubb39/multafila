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
	rm -f *.o *~ a.out core small-parser

.PHONY: all
all: clean default

