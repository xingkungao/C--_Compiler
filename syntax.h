/*************************************************************************
    > File Name: syntax.h
    > Author: lax
    > Mail: xingkungao@gmail.com
    > Created Time: Thu 05 Jun 2014 09:39:04 AM CST
 ************************************************************************/

#ifndef _SYNTAX_H_
#define _SYNTAX_H_
#include "semantic.h"

#define false 0
#define true 1

typedef enum {TINT, TFLOAT, TID, TSEMI, TCOMMA, TASSIGNOP, TRELOP, TPLUS, TMINUS, TSTAR,
	TDIV, TAND, TOR, TDOT, TNOT, TTYPE, TLP, TRP, TLB, TRB, TLC, TRC, TSTRUCT, TRETURN, 
	TIF, TELSE, TWHILE, Program, ExtDefList, ExtDef, ExtDecList, Specifier, StructSpecifier, 
	OptTag, Tag, VarDec, FunDec, VarList, ParamDec, CompSt, StmtList, Stmt, DefList, Def, 
	DecList, Dec, Exp, Args
} type_t;



struct syntaxTreeNode {
	int lineno;
	type_t type;
	char name[16];
	union {
		int ival;
		float fval;
		char idval[32];
		char otherval[32];
	};
	nodeAttr_t attr; 
	int prodnum;
	int degree;
	struct syntaxTreeNode **children;
};
typedef struct syntaxTreeNode syntaxTreeNode;



#endif
