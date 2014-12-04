/*************************************************************************
  > File Name: interCode.h
  > Author: lax
  > Mail: xingkungao@gmail.com
  > Created Time: Sat 31 May 2014 06:54:42 PM CST
 ************************************************************************/

#ifndef _INTERCODE_H_
#define _INTERCODE_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
//#include "symbol.h"
extern int labelCnt;
extern int tempCnt; 
typedef enum {Variable, Constant, Address, Dereference, Lbl} OperandKind;
typedef enum {Add, Sub, Mul, Div,\
	Eq, Ne, Gt, Lt, Ge, Le,\
		Func, Param, Arg, Return, Call, Read, Write,\
	Addr, Store, Assign,\
		Goto, Label, Decl
} CodeKind;

typedef struct Operand Operand;
struct Operand {
	OperandKind kind; 
	union {
		int var_no;
		int val;
	}u;
};

typedef struct OperandList OperandList;
struct OperandList {
	Operand *op;
	OperandList *next;
};

typedef struct InterCode InterCode; 
struct InterCode {
	CodeKind kind;
	union {
		struct {char *name;} funcdec;
		struct {Operand *left, *right;} assign;
		struct {Operand *result, *op1, *op2;} binop;
		struct {Operand *op;} uniop;
		struct {Operand *addr; int size;} dec;
		struct {Operand *result; char* name;} call;
	}u;
};

struct InterCodes {
	InterCode *code;
	struct InterCodes *prev, *next;
};
typedef struct InterCodes InterCodes;


Operand* newOperand(OperandKind kind, int value);
Operand* newTemp(); 
Operand* newLabel();
Operand* newVar(int val);
Operand* newConst(int val); 
Operand* newAddr(int val); 
Operand* newDeref(int val);
InterCodes* newCallCode(Operand* op, char* name); 
InterCodes* newAssignCode(Operand* left, Operand* right);
InterCodes* newBinopCode(CodeKind type, Operand* result, Operand* op1, Operand* op2);
InterCodes* newUniopCode(CodeKind type, Operand* op);
InterCodes* newDecCode(Operand* op, int size);
InterCodes* newFuncCode(char *name);
InterCodes* codesConcat(int n, ...);



#endif
