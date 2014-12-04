#include <stdio.h>
#include <stdlib.h>
#include "symbol.h"
#include "syntax.h"
#include "sdt.h"
#include "semantic.h"
extern syntaxTreeNode* root;
extern int errorTag;

int main(int argc, char** argv) {
	FILE* f;
	if (argc > 3) {
		printf("Usage: ./parser testFile outputFile\n");
		return 1;
	}
	if (!(f = fopen(argv[1], "r"))) {
		perror(argv[1]);
		return 1;
	}
	yyrestart(f);
	yyparse();

	/* semantic analysis */
	if (!errorTag) {
		init();
		initSymbolTable();
		addReadWrite();
		traverse(root, 0);
	}
	assert(root);
	printf("\n\n\n\n\n");
	
	if (!errorTag) {
		initSymbolTable();
		addReadWrite();
		//generateIR(argv[2], translate(root));
		gen_code(fopen(argv[2], "w+"), translate(root));
	}

//	freeMem(root);
	close(f);
	return 0;
}
