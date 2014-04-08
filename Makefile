CC = gcc
CFLAGS 	= -lfl -ly
FLEX	= flex
BISON 	= bison
BFLAGS = -d -v

parser: main.c ex1.tab.c 
	$(CC) main.c ex1.tab.c $(CFLAGS) -o parser 

ex1.tab.c: ex1.y lex.yy.c syntaxTreeNode.h
	$(BISON) $(BFLAGS) ex1.y 

lex.yy.c: ex1.l 
	$(FLEX) ex1.l

.PHONY: play clean
play: parser test1.c
	./parser test1.c 
clean:
	rm -rf parser lex.yy.c ex1.tab.h ex1.tab.c
