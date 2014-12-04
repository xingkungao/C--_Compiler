/*************************************************************************
    > File Name: sdt.h
    > Author: lax
    > Mail: xingkungao@gmail.com
    > Created Time: Thu 05 Jun 2014 09:57:53 AM CST
 ************************************************************************/

#ifndef _SDT_H_
#define _SDT_H_
#include "semantic.h"
#include "syntax.h"
#include "symbol.h"

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
#endif

