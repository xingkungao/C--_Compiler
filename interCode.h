/*************************************************************************
    > File Name: interCode.h
    > Author: lax
    > Mail: xingkungao@gmail.com
    > Created Time: Sat 31 May 2014 06:54:42 PM CST
 ************************************************************************/

#ifdef _INTERCODE_H_
#define INTERCODE_H_
#include "symbol.h"
enum {Variable, Constant, Address, Dereference, Label} OperandKind; 
enum {Add, Sub, Mul, Div, \
	Eq, Ne, Gt, Lt, Ge, Le, \
		 Func, Param, Arg, Return, Call, Read, Write,\
		 Addr, Store,\
		 Assign,\
		 Goto, Label,\
		 Dec 
} Codekind;
typedef struct Operand_* Operand;
struct Operand {
	OprandKind kind; 
	union {
		int var_no;
		int value;
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

inline Operand* newOperand(OperandKind kind, int value) {
	Operand op = (Operand*)malloc(sizeof(Operand));
	op->kind = kind;
	switch (kind) {
		case Variable:
		case Dereference:
			op->u.var_no = value;
		case Constant:
		case Label:
		case Address:
			op->u.val = value;
	}
	return op;
}

inline Operand* newTemp() {
	return newOperand(Variable, tempCnt++);
}

inline Operand* newLabel() {
	return newOperand(Label, labelCnt++);
}

inline Operand* newVar(int val) {
	return newOperand(Variable, val);
}

inline Operand* newConst(int val) {
	return newOperand(Constant, val);
}

inline Operand* newAddr(int val) {
	return newOperand(Address, val);
}

inline Operand* newDeref(int val) {
	return newOperand(Dereference, val);
}

inline InterNodes* newCallCode(Operand* op, char* name) {
	InterCode *code = (InterCode*)malloc(sizeof(InterCode));
	code->kind = Call;
	code->u.call.name = (char*)malloc(32 * sizeof(char));
	strcpy(code->u.call.name, name);
	code->u.call.result = op;
	InterCodes *codes = (InterCodes*)malloc(sizeof(InterCodes));
	codes->code = code;
	codes->prev = codes->next = codes;
	return codes;
}

inline InterNodes* newAssignCode(Operand* left, Operand* right) {
	InterCode *code = (InterCode*)malloc(sizeof(InterCode));
	code->kind = Assign;
	code->u.assign.left = left;
	code->u.assign.right = right;
	InterCodes *codes = (InterCodes*)malloc(sizeof(InterCodes));
	codes->code = code;
	codes->prev = codes->next = codes;
	return codes;
}

inline InterNodes* newBinopCode(CodeKind type, Operand* result, Operand* op1, Operand* op2) {
	InterCode *code = (InterCode*)malloc(sizeof(InterCode));
	code->kind = type;
	code->u.binop.result = result;
	code->u.binop.op1 = op1;
	code->u.binop.op2 = op2;
	InterCodes *codes = (InterCodes*)malloc(sizeof(InterCodes));
	codes->code = code;
	codes->prev = codes->next = codes;
	return codes;
}

inline InterNodes* newUniopCode(CodeKind type, Operand* op) {
	InterCode *code = (InterCode*)malloc(sizeof(InterCode));
	code->kind = type;
	code->u.uniop.op = op;
	InterCodes *codes = (InterCodes*)malloc(sizeof(InterCodes));
	codes->code = code;
	codes->prev = codes->next = codes;
	return codes;
}

inline InterNodes* newDecCode(Operand* op, int size) {
	InterCode *code = (InterCode*)malloc(sizeof(InterCode));
	code->kind = Dec;
	code->u.dec.op = op;
	code->u.dec.size = size;
	InterCodes *codes = (InterCodes*)malloc(sizeof(InterCodes));
	codes->code = code;
	codes->prev = codes->next = codes;
	return codes;
}

inline InterNodes* newFuncCode(char *name) {
	InterCode *code = (InterCode*)malloc(sizeof(InterCode));
	code->kind = Func;
	code->u.funcdec.name = (char*)malloc(32 * sizeof(char));
	strcpy(code->u.funcdec.name, name);
	InterCodes *codes = (InterCodes*)malloc(sizeof(InterCodes));
	codes->code = code;
	codes->prev = codes->next = codes;
	return codes;
}

inline InterCodes* codesConcat(int n, ...) {
	if (!n) return NULL;
	va_list args;
	va_start(args, n);
	InterCodes *codes = va_arg(args, InterCodes*);
	InterCodes *ptr;
	int i;
	for (i = 0; i < n; i++) {
		ptr = va_arg(args, InterCodes*);
		if (!codes) codes = ptr;
		else if (ptr){
			ptr->next = codes;
			ptr->prev = codes->prev;
			ptr->prev->next = ptr;
			ptr->next->prev = ptr;
		}
	}
	va_end(args);
	return codes;
}


	
extern int labelCnt;
extern int tempCnt;


#endif
