CC = gcc
CFLAGS 	= -g -lfl -ly
FLEX	= flex
BISON 	= bison
BFLAGS = -d -v

parser: main.c ex1.tab.c semantic.c sdt.c symbol.c interCode.c IR.c syntax.h symbol.h semantic.h interCode.h  sdt.h
	$(CC) main.c ex1.tab.c semantic.c sdt.c symbol.c interCode.c IR.c syntax.h symbol.h semantic.h interCode.h  sdt.h $(CFLAGS) -o parser  

ex1.tab.c: ex1.y lex.yy.c syntax.h  
	$(BISON) $(BFLAGS) ex1.y 

lex.yy.c: ex1.l 
	$(FLEX) ex1.l

.PHONY: clean
clean:
	rm -rf parser lex.yy.c ex1.tab.h ex1.tab.c
