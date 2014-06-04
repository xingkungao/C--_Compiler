#include <stdio.h>
#include <stdlib.h>
#include "symbol.h"

int main(int argc, char** argv) {
	FILE* f;
	if (argc <= 1 || argc >= 3) {
		printf("Usage: ./parser oneTestFile\n");
		return 1;
	}
	if (!(f = fopen(argv[1], "r"))) {
		perror(argv[1]);
		return 1;
	}
	yyrestart(f);
	yyparse();

	init();
	initSymbolTable();
//	enterScope();
	addReadWrite();

	traverse(root, 0);

//	leaveScope();
	return 0;
}
