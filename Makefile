CC 		= gcc
CFLAGS 	= -g -lfl -ly
FLEX	= flex
BISON 	= bison
BFLAGS  = -d -v
ACFILES = $(shell find src/ -name "*.c")
CFILES = $(filter-out src/lex.yy.c, $(ACFILES))
AHFILES = $(shell find src/ -name "*.h")
HFILES  = $(filter-out src/ex1.tab.h, $(AHFILES))

compiler: $(CFILES) $(HFILES) src/ex1.tab.c
	$(CC) $(CFILES) $(HFILES) $(CFLAGS) -o compiler 

src/ex1.tab.c: src/ex1.y src/lex.yy.c src/syntax.h  
	$(BISON) $(BFLAGS) src/ex1.y 
	@mv ex1.tab.c ex1.tab.h ex1.output src/

src/lex.yy.c: src/ex1.l 
	$(FLEX) src/ex1.l
	@mv lex.yy.c src/

.PHONY: clean
clean:
	rm -rf compiler src/lex.yy.c src/ex1.tab.h src/ex1.tab.c src/ex1.output
