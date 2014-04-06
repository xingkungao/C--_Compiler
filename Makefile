CC = gcc
CFLAGS 	= -lfl -ly
FLEX	= flex
BISON 	= bison
BFLAGS = -d -v

parser: main.c parse.tab.c 
	$(CC) main.c parse.tab.c $(CFLAGS) -o parser 

parse.tab.c: parse.y lex.yy.c
	$(BISON) $(BFLAGS) parse.y 

lex.yy.c: scan.l 
	$(FLEX) scan.l

.PHONY: play clean
play: parser test.c
	./parser test.c 
clean:
	rm -rf parser lex.yy.c parse.tab.h parse.tab.c
