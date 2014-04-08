/*************************************************************************
    > File Name: syntaxTreeNode.h
    > Author: lax
    > Mail: xingkungao@gmail.com
    > Created Time: Mon 07 Apr 2014 02:46:34 PM CST
 ************************************************************************/
#ifndef _SYNTAXTREE_H_
#define _SYNTAXTREE_H_

typedef enum {CINT, CFLOAT, CID, CTYPE, COTHERLEXEME, CGRAMMAR}type_t;

struct syntaxTreeNode {
	int lineno;
	type_t type;
	union {
		char terminal[16];
		char nonterminal[16];
	};
	union {
		int ival;
		float fval;
		char idval[32];
		char otherval[32];
	};
	int degree;
	struct syntaxTreeNode **children;
};
typedef struct syntaxTreeNode syntaxTreeNode;

#endif

