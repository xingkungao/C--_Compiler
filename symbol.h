/*************************************************************************
  > File Name: syntaxTreeNode.h > Author: lax > Mail: xingkungao@gmail.com
  > Created Time: Mon 07 Apr 2014 02:46:34 PM CST
 ************************************************************************/
#ifndef _SYNTAXTREE_H_
#define _SYNTAXTREE_H_
#include <assert.h>
#define false 0
#define true 1

typedef enum {TINT, TFLOAT, TID, TSEMI, TCOMMA, TASSIGNOP, TRELOP, TPLUS, TMINUS, TSTAR,
	TDIV, TAND, TOR, TDOT, TNOT, TTYPE, TLP, TRP, TLB, TRB, TLC, TRC, TSTRUCT, TRETURN, 
	TIF, TELSE, TWHILE, Program, ExtDefList, ExtDef, ExtDecList, Specifier, StructSpecifier, 
	OptTag, Tag, VarDec, FunDec, VarList, ParamDec, CompSt, StmtList, Stmt, DefList, Def, 
	DecList, Dec, Exp, Args
} type_t;

#define SPECIFIER_LENGTH 16
#define TOKEN_LENGTH 32
#define NR_SYMBOL_TABLE_ENTRY 0x4000
#define MAX_SCOPE 32

typedef struct type vtype_t;
struct nodeAttr { /* for variable */
	vtype_t* inhType; //
	vtype_t* synType; //
	int inStruct;
	struct fieldList* structure;  
	int legal; //
	/* for function */
	vtype_t* returnType; //
	struct typeList* arguments; //
};
typedef struct nodeAttr nodeAttr_t;
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

typedef struct fieldList fieldList_t;
typedef struct typeList typeList_t;

/* type of variable */
struct type {
	enum {BASIC, ARRAY, STRUCTURE} kind;
	union {
		// int or float
		int basic;
		// array 
		struct {
			struct type* elem;
			int size;
		}array;
		// structure
		struct fieldList* structure;
	}u;
};

/* symbol for varialbe */ 
typedef struct { 
	struct type* type;
}varType_t;


/* symbol for function */
typedef struct {
	int paramNum;
//	int defined;
	struct typeList* paramList;
	struct type* returnType;
}funcType_t;


struct typeList {
	struct type* type; // type 
	struct typeList* next; // next type 
};

struct fieldList {
	char *name; // name of the field
	int lineno;
	struct type* type;
	struct fieldList* tail;
};


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
	struct hashEntry *next, *scope;
};
typedef struct hashEntry hashEntry_t;

extern hashEntry_t symbolTable[NR_SYMBOL_TABLE_ENTRY];
extern hashEntry_t scopeStack[MAX_SCOPE]; 
extern int scopeDepth;
extern vtype_t* VTYPEINT;
extern vtype_t* VTYPEFLOAT;
extern syntaxTreeNode* root;



#define reportError1(i,s) printf("Error type 1 at line %d: Undefined variable \"%s\"\n", i, s); 
#define reportError2(i,s) printf("Error type 2 at line %d: Undefined function\"%s\"\n", i, s); 
#define reportError3(i,s) printf("Error type 3 at line %d: Redefined variable \"%s\"\n", i, s); 
#define reportError4(i,s) printf("Error type 4 at line %d: Redefined function\"%s\"\n", i, s); 
#define reportError5(i) printf("Error type 5 at line %d: Type mismatched\n", i); 
#define reportError6(i) printf("Error type 6 at line %d: The left-hand side of an assignment must be a variable\n", i); 
#define reportError7(i) printf("Error type 7 at line %d: Operands type mismatched\n", i); 
#define reportError8(i) printf("Error type 8 at line %d: The return type mismatched\n", i); 
#define reportError9(i) printf("Error type 9 at line %d: The method is not applicable for the arguments\n", i); 
#define reportError10(i,s) printf("Error type 10 at line %d: \"%s\" must be an array\n", i, s); 
#define reportError11(i,s) printf("Error type 11 at line %d: \"%s\" must be an function\n", i, s); 
#define reportError12(i) printf("Error type 12 at line %d: Operands type mistaken \n", i); 
//optional
#define reportError13(i,s) printf("Error type 13 at line %d: Illegal use of \".\" Request for \"%s\" in something not a structure\n", i, s); 
#define reportError14(i,s) printf("Error type 14 at line %d: Un-existed field \"%s\"\n", i, s); 
#define reportError15(i,s) printf("Error type 15 at line %d: Redefined field or initialize variable in struct \"%s\"\n", i, s); 
#define reportError16(i,s) printf("Error type 16 at line %d: Duplicated structure name \"%s\"\n", i, s); 
#define reportError17(i,s) printf("Error type 17 at line %d: Undefined struct \"%s\"\n", i, s); 


unsigned int hashPjw(char* name); 

void checkExtDecList(syntaxTreeNode* root, syntaxTreeNode** children, int degree, int space);
void checkSpecifier(syntaxTreeNode* root, syntaxTreeNode** children, int degree, int space);
void checkStructSpecifier(syntaxTreeNode* root, syntaxTreeNode** children, int degree, int space);
void checkVarDec(syntaxTreeNode* root, syntaxTreeNode** children, int degree, int space);
void checkFunDec(syntaxTreeNode* root, syntaxTreeNode** children, int degree, int space);
void checkVarList(syntaxTreeNode* root, syntaxTreeNode** children, int degree, int space);
void checkParamDec(syntaxTreeNode* root, syntaxTreeNode** children, int degree, int space);
void checkCompSt(syntaxTreeNode* root, syntaxTreeNode** children, int degree, int space);
void checkStmtList(syntaxTreeNode* root, syntaxTreeNode** children, int degree, int space);
void checkStmt(syntaxTreeNode* root, syntaxTreeNode** children, int degree, int space);
void checkDefList(syntaxTreeNode* root, syntaxTreeNode** children, int degree, int space);
void checkDef(syntaxTreeNode* root, syntaxTreeNode** children, int degree, int space);
void checkDecList(syntaxTreeNode* root, syntaxTreeNode** children, int degree, int space);
void checkDec(syntaxTreeNode* root, syntaxTreeNode** children, int degree, int space);
void checkExp(syntaxTreeNode* root, syntaxTreeNode** children, int degree, int space);
void checkArgs(syntaxTreeNode* root, syntaxTreeNode** children, int degree, int space);
void traverse(syntaxTreeNode* root, int space);

int equalType(vtype_t* a, vtype_t* b);

void initSymbolTable();
symbol_t* getSymbol(char *name);
void insertStybol(symbol_t* sbl);
void leaveScope();
void enterScope();
void init();

#endif
