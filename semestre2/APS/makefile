LEX = flex
YACC = bison -d
CC = gcc

LFLAGS =-ll -lm
CFLAGS =-W -Wall -ansi -pedantic -Iinclude




.PHONY: all clean 

APS : aps

all : aps

test :
	./aps test.txt

aps:  aps.tab.c lex.yy.c
	$(CC)  -o $@ $^ $(LFLAGS)

aps.tab.c: aps.y
	$(YACC) $^ 
	
lex.yy.c: aps.l
	$(LEX) $^
	
clean: 
	rm -rf aps aps.tab.c lex.yy.c aps.tab.h