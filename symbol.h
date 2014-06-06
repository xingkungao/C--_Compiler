/*************************************************************************
  > File Name: syntaxTreeNode.h > Author: lax > Mail: xingkungao@gmail.com
  > Created Time: Mon 07 Apr 2014 02:46:34 PM CST
 ************************************************************************/
#ifndef _SYNTAXTREE_H_
#define _SYNTAXTREE_H_
#include <assert.h>
#include "semantic.h"
#include "interCode.h" 
#define SPECIFIER_LENGTH 16
#define TOKEN_LENGTH 32
#define NR_SYMBOL_TABLE_ENTRY 0x4000
#define MAX_SCOPE 32
/* symbol for varialbe */ 
typedef struct { 
	vtype_t* type;
	Operand *op;
	int isParam;
}varType_t;


/* symbol for function */
typedef struct {
	int paramNum;
//	int defined;
	typeList_t* paramList;
	vtype_t* returnType;
}funcType_t;


/* symbol table entry */
struct symbol {
	char name[32];
	enum {var, function} kind;
	union {
		varType_t* var;
		funcType_t* func;
	}type;
	int depth;
};
typedef struct symbol symbol_t;

/* entry in the hash table */
struct hashEntry {
	symbol_t *data;
	struct hashEntry *next, *prev, *scope;
};
typedef struct hashEntry hashEntry_t;


unsigned int hashPjw(char* name); 
int equalType(vtype_t* a, vtype_t* b);

void initSymbolTable();
symbol_t* getSymbol(char *name);
void insertStybol(symbol_t* sbl);
void leaveScope();
void enterScope();
void init();

#endif

