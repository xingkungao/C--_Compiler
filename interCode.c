/*************************************************************************
    > File Name: util.c
    > Author: lax
    > Mail: xingkungao@gmail.com
    > Created Time: Thu 05 Jun 2014 09:16:43 AM CST
 ************************************************************************/

#include  "interCode.h"


Operand* newOperand(OperandKind kind, int value) {
	Operand* op = (Operand*)malloc(sizeof(Operand));
	op->kind = kind;
	switch (kind) {
		case Variable:
		case Address:
			op->u.var_no = value;
			break;
		case Constant:
		case Lbl:
		case Dereference:
			op->u.val = value;
	}
	return op;
}

Operand* newTemp() {
	return newOperand(Variable, tempCnt++);
}

Operand* newLabel() {
	return newOperand(Lbl, labelCnt++);
}

Operand* newVar(int val) {
	return newOperand(Variable, val);
}

Operand* newConst(int val) {
	return newOperand(Constant, val);
}

Operand* newAddr(int val) {
	return newOperand(Address, val);
}

Operand* newDeref(int val) {
	return newOperand(Dereference, val);
}

InterCodes* newCallCode(Operand* op, char* name) {
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

InterCodes* newAssignCode(Operand* left, Operand* right) {
	InterCode *code = (InterCode*)malloc(sizeof(InterCode));
	code->kind = Assign;
	code->u.assign.left = left;
	code->u.assign.right = right;
	InterCodes *codes = (InterCodes*)malloc(sizeof(InterCodes));
	codes->code = code;
	codes->prev = codes->next = codes;
	return codes;
}

InterCodes* newBinopCode(CodeKind type, Operand* result, Operand* op1, Operand* op2) {
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

InterCodes* newUniopCode(CodeKind type, Operand* op) {
	InterCode *code = (InterCode*)malloc(sizeof(InterCode));
	code->kind = type;
	code->u.uniop.op = op;
	InterCodes *codes = (InterCodes*)malloc(sizeof(InterCodes));
	codes->code = code;
	codes->prev = codes->next = codes;
	return codes;
}

InterCodes* newDecCode(Operand* op, int size) {
	InterCode *code = (InterCode*)malloc(sizeof(InterCode));
	code->kind = Decl;
	code->u.dec.addr = op;
	code->u.dec.size = size;
	InterCodes *codes = (InterCodes*)malloc(sizeof(InterCodes));
	codes->code = code;
	codes->prev = codes->next = codes;
	return codes;
}

InterCodes* newFuncCode(char *name) {
	InterCode *code = (InterCode*)malloc(sizeof(InterCode));
	code->kind = Func;
	code->u.funcdec.name = (char*)malloc(32 * sizeof(char));
	strcpy(code->u.funcdec.name, name);
	InterCodes *codes = (InterCodes*)malloc(sizeof(InterCodes));
	codes->code = code;
	codes->prev = codes->next = codes;
	return codes;
}

InterCodes* codesConcat(int n, ...) {
	if (!n) 
		return NULL;

	int i;
	va_list codeList;
	va_start(codeList, n);

	InterCodes *codes = va_arg(codeList, InterCodes*);
	InterCodes *ptr, *last, *p;
	/** pay attention to null pointers in args*/
	for (i = 1; i < n; i++) {
		ptr = va_arg(codeList,InterCodes*);
		if (!codes) codes = ptr;
		else if (ptr){
			/*
			ptr->next = codes;
			ptr->prev = codes->prev;
			ptr->prev->next = ptr;
			ptr->next->prev = ptr;
			*/
			p = ptr->prev;
			codes->prev->next = ptr;
			last = codes->prev;
			ptr->prev->next = codes;
			ptr->prev = last;
			codes->prev = p;

		}
	}
	va_end(codeList);
	return codes;
}

