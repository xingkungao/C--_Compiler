/*************************************************************************
    > File Name: interCode.c
    > Author: lax
    > Mail: xingkungao@gmail.com
    > Created Time: Sat 31 May 2014 07:43:21 PM CST
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "symbol.h"
#include "syntax.h"
#include "interCode.h"
#define DEBUG

int tempCnt = 0;
int labelCnt = 0;

InterCodes* translateParamDec(syntaxTreeNode *root); 
InterCodes* translateExtDef(syntaxTreeNode *root); 
InterCodes* translate(syntaxTreeNode* root); 
InterCodes* translateExp(syntaxTreeNode* root, Operand* place);
InterCodes* translateCompst(syntaxTreeNode* root);
InterCodes* translateCond(syntaxTreeNode *root, Operand *ltrue, Operand *lfalse);
InterCodes* translateFunDec(syntaxTreeNode* root);
InterCodes* translateDec(syntaxTreeNode* root);
InterCodes* translateStmt(syntaxTreeNode* root);
InterCodes* translateArgs(syntaxTreeNode* root,  OperandList** list);
InterCodes* translateVarDec(syntaxTreeNode* root);



InterCodes* translateParamDec(syntaxTreeNode *root) {
#ifdef DEBUG
	printf("enter translateParamDec\n");
#endif

	// ParamDec -> Specifier VarDec
	syntaxTreeNode **children = root->children;
	printf("name is %s\n", children[1]->idval);
	translateVarDec(children[1]);
	symbol_t *sbl = getSymbol(children[1]->attr.name);
	assert(sbl);
	sbl->type.var->isParam = 1;
	/* to be continued */ ///
	return newUniopCode(Param, sbl->type.var->op); 
}

InterCodes* translateExtDef(syntaxTreeNode *root) {
#ifdef DEBUG
	printf("enter translateExtDef\n");
#endif
	syntaxTreeNode **children = root->children;
	if (root->prodnum == 3)  // ExtDef -> Specifier FunDec CompSt 
	{
		InterCodes* c1 = translate(children[0]);
		enterScope();
		InterCodes* c2 = translate(children[1]);
		InterCodes* c3 = translate(children[2]);
		leaveScope();
		return codesConcat(3, c1, c2, c3);
	}
	else {
		int i;
		InterCodes *list = NULL;
		for (i = 0; i < root->degree; i++) {
			list = codesConcat(2, list, translate(children[i]));
		}
		return list;
	}
}

InterCodes* translate(syntaxTreeNode* root) {
#ifdef DEBUG
	printf("enter translate\n");
#endif
	if (!root) {
		printf("empty root\n");
		return NULL;
	}

	int degree = root->degree;
	syntaxTreeNode** children = root->children;
	switch (root->type) {
		case ExtDef: //
			return translateExtDef(root);
		case VarDec: //
			return translateVarDec(root);
		case FunDec:
			return translateFunDec(root);
		case ParamDec: //
			return translateParamDec(root);
		case CompSt:
			return translateCompst(root);
		case Stmt: //
			assert(root);
			return translateStmt(root);
		case Dec:
			return translateDec(root);
			/*
		case Exp:
			return translateDec(root);
		case Args:
			return translateDec(root);
			*/
		default: 
			{
			int i;
			InterCodes *codes = NULL;
			for (i = 0; i < degree; i++) {
			//	printf("name in other is %s\n", children[i]->name);
				codes = codesConcat(2, codes, translate(children[i]));
			}
			return codes;
			}
	}
}
	
int getSize (vtype_t* type) {
	if (!type) return 0;
	printf("               in getsize\n");
	if (type->kind == BASIC) return 4;
	if (type->kind == ARRAY) return type->u.array.size * getSize(type->u.array.elem);
	if (type->kind == STRUCTURE) {
		int cnt = 0;
		fieldList_t *fields = type->u.structure;
		while (fields) {
			cnt += getSize(fields->type);
			fields = fields->tail;
		}
		return cnt;
	}
	assert(0);
}

InterCodes* translateExp(syntaxTreeNode* root, Operand* place) {
#ifdef DEBUG
	printf("enter translateExp\n");
#endif
	syntaxTreeNode** children = root->children;
	int degree = root->degree;
	switch(root->prodnum) {
		case 1:  // Exp -> Exp ASSIGNOP Exp
			{
				// if Exp1 is not ID, then it must be some kind of dereference
				Operand *t1 = (children[0]->prodnum == 16)? newVar(-1) : newDeref(-1);
				Operand *t2 = newTemp();
				InterCodes* c1 = translateExp(children[0], t1);
				InterCodes* c2 = translateExp(children[2], t2);
				return codesConcat(4, c1, c2, newAssignCode(t1, t2), place?newAssignCode(place, t1):NULL);
				if (place)
					return codesConcat(3, c1, c2, newAssignCode(place, t2));
				else 
					return codesConcat(3, c1, c2, newAssignCode(t1, t2));
			}
		case 2:  // Exp -> Exp AND Exp
		case 3:  // Exp -> Exp OR Exp
		case 4:  // Exp -> Exp RELOP Exp
		case 11: // Exp -> NOT Exp 
			{
				 Operand *l1 = newLabel(), *l2 = newLabel();
				 Operand *const1 = newConst(0);
				 InterCodes* c0 = newAssignCode(place, newConst(0)); 
				 InterCodes* c1 = translateCond(root, l1, l2);
				 InterCodes* c2 = codesConcat(2, newUniopCode(Label, l1), place ? newAssignCode(place, newConst(1)) : NULL);
				 return codesConcat(4, c0, c1, c2, newUniopCode(Label, l2));
			}

		case 5:  // Exp -> Exp PLUS Exp
		case 6:  // Exp -> Exp MINUS Exp
		case 7:  // Exp -> Exp STAR Exp
		case 8:  // Exp -> Exp DIV Exp
			{
				 Operand *t1 = newTemp(), *t2 = newTemp();
				 InterCodes* c1 = translateExp(children[0], t1);
				 InterCodes* c2 = translateExp(children[2], t2);
				 InterCodes* c3;
				 if (root->prodnum == 5) 
					 c3 = place ? newBinopCode(Add, place, t1, t2) : NULL;
				 else if (root->prodnum == 6)
					 c3 = place ? newBinopCode(Sub, place, t1, t2) : NULL;
				 else if (root->prodnum == 7)
					 c3 = place ? newBinopCode(Mul, place, t1, t2) : NULL;
				 else if (root->prodnum == 8)
					 c3 = place ? newBinopCode(Div, place, t1, t2) : NULL;
				 return  codesConcat(3, c1, c2, c3);
			}

		case 9:  // Exp -> LP Exp RP
			return translateExp(children[1], place);
		case 10: // Exp -> MINUS Exp 
			{
				Operand* t1 = newTemp();
				InterCodes *c1 = translateExp(children[1], t1);
				InterCodes *c2 = place ? newBinopCode(Sub, place, newConst(0), t1) : NULL;
				return codesConcat(2, c1, c2);
			}

		case 12:  // Exp -> ID LP Arg RP
		case 13:  // Exp -> ID LP RP
			{
				Operand *v = place ? place : newTemp();
				symbol_t* sbl = getSymbol(children[0]->idval);
				//assert(sbl);
				printf("get one func from symbol table: %s\n", children[0]->idval);
				if (root->degree == 3) {
					printf("here\n");
					printf("%s\n", children[0]->idval);
					if (!strcmp(children[0]->idval, "read")) 
						return newUniopCode(Read, v);
					return newCallCode(v, children[0]->idval);
				}
				else {
					OperandList *argList = 0;
					printf("there\n");
					InterCodes *c1 = translateArgs(children[2], &argList);
					InterCodes* c2 = 0;
					if (!strcmp(children[0]->idval, "write")) 
						return codesConcat(2, c1, newUniopCode(Write, argList->op)); 
					while (argList) {
						 c2 = codesConcat(2, c2, newUniopCode(Arg, argList->op));
						 argList = argList->next;
					}
					return codesConcat(3, c1, c2, newCallCode(v, children[0]->idval));
				}
			}
		case 14:  // Exp -> Exp LB Exp RB
			{
				Operand *t1 = newVar(-1);
				Operand *t2 = newTemp(), *t3 = newTemp(), *t4 = newTemp();
				InterCodes *c1 = translateExp(children[0], t1);
				InterCodes *c2 = translateExp(children[2], t2);
//				assert(root->attr.synType);
				root->attr.synType = children[0]->attr.synType->u.array.elem;
				InterCodes *c3 = newBinopCode(Mul, t3, t2, newConst(getSize(root->attr.synType)));
				InterCodes *c4 = newBinopCode(Add, t4, t3, t1);

				if (!place) 
					return codesConcat(4, c1, c2, c3, c4);
				if (place->u.var_no == -1) {
					place->u.var_no = t4->u.var_no;
					return codesConcat(4, c1, c2, c3, c4);
				}
				return codesConcat(5, c1, c2, c3, c4, newAssignCode(place, newDeref(t4->u.var_no)));
			}

		case 15:  // Exp -> Exp DOT ID 
			{
				Operand *v1 = newVar(-1), *t2 = newTemp();
				InterCodes *c1 = translateExp(children[0], v1);
				int idx = 0;
				fieldList_t *fields = children[0]->attr.synType->u.structure;
				while (fields) {
					if (!strcmp(fields->name, children[2]->idval)) 
						break;
					idx += getSize(fields->type);
					fields = fields->tail;
				}
				InterCodes *c2 = newBinopCode(Add, t2, v1, newConst(idx));
				if (!place) 
					return codesConcat(2, c1, c2);
				if (place->u.var_no < 0) {
					place->u.var_no = t2->u.var_no;
					return codesConcat(2, c1, c2);
				}
				return codesConcat(3, c1, c2, newAssignCode(place, newDeref(t2->u.var_no)));

			}
			break;
		case 16:  // Exp -> ID
			{
				symbol_t *sbl = getSymbol(children[0]->idval);
				printf("exp->id, id name is %s\n", children[0]->idval);
				assert(sbl);
				root->attr.synType = sbl->type.var->type;
				InterCodes *c1;
				if (!place) {
					printf("to here\n");
					return NULL;
				}
				root->attr.synType = sbl->type.var->type;
				if (place->u.var_no < 0) {
					printf("to here\n");
					if (sbl->type.var->type->kind == BASIC || sbl->type.var->isParam)
						place->kind = Variable;
					else
						place->kind = Address;
					/* to be continued */ ///
					printf("to here\n");
					place->u.var_no = sbl->type.var->op->u.var_no;
					printf("to here\n");
					return NULL;
				}
				else {
					printf("to here here\n");
					//assert(root->attr.synType);
					//if (root->attr.synType->kind == BASIC)
					assert(sbl->type.var->type);
					if (sbl->type.var->type->kind == BASIC)
						c1 = newAssignCode(place, sbl->type.var->op);
					else {
						c1 = newAssignCode(place, newAddr(sbl->type.var->op->u.var_no));
					}
					printf("to here\n");
					return c1;
				}
			}
		case 17:  // Exp -> INT
			{
				if (!place)
					return NULL;
				//if (place->kind == Variable || place->kind == Address) {
				/*
				if (place->kind == Variable) {
					place->kind = Constant;
					place->u.val = children[0]->ival;
				//	place->u.var_no;
					return;
				}
				*/
				return newAssignCode(place, newConst(children[0]->ival));
			}
		case 18:  // Exp -> FLOAT
			{
			}
			break;
		default:;
	}
}


InterCodes* translateCompst(syntaxTreeNode* root){
#ifdef DEBUG
	printf("enter translateCompst\n");
#endif
	int degree = root->degree;
	syntaxTreeNode** children = root->children;
	switch(root->prodnum) {
		case 1: // Compst -> LC DefList Stmtlist RC
			{
				enterScope();
				InterCodes *c1 = translate(children[1]);
				InterCodes *c2 = translate(children[2]);
				leaveScope();
				return codesConcat(2, c1, c2);
			}
	}
}

InterCodes* translateCond(syntaxTreeNode *root, Operand *ltrue, Operand *lfalse) {
#ifdef DEBUG
	printf("enter translateCond\n");
#endif
	int degree = root->degree;
	syntaxTreeNode** children = root->children;
	switch(root->prodnum) {
		case 2:  // Exp -> Exp AND Exp
			{
				Operand* l1 = newLabel();
				InterCodes* c1 = translateCond(children[0], l1, lfalse);
				InterCodes* c2 = translateCond(children[2], ltrue, lfalse);
				return codesConcat(3, c1, newUniopCode(Label, l1), c2);
			}

		case 3:  // Exp -> Exp OR Exp
			{
				Operand* l1 = newLabel();
				InterCodes* c1 = translateCond(children[0], ltrue, l1);
				InterCodes* c2 = translateCond(children[2], ltrue, lfalse);
				return codesConcat(3, c1, newUniopCode(Label, l1), c2);
			}
		case 4:  // Exp -> Exp RELOP Exp
			{
				Operand* t1 = newTemp(), *t2 = newTemp();
				InterCodes* c1 = translateExp(children[0], t1);
				InterCodes* c2 = translateExp(children[2], t2);
				CodeKind type;
				if (!strcmp(children[1]->idval, "=="))
					type = Eq;
				else if (!strcmp(children[1]->idval, "!="))
					type = Ne;
				else if (!strcmp(children[1]->idval, "<"))
					type = Lt;
				else if (!strcmp(children[1]->idval, ">"))
					type = Gt;
				else if (!strcmp(children[1]->idval, "<="))
					type = Le;
				else if (!strcmp(children[1]->idval, ">="))
					type = Ge;
				return codesConcat(4, c1, c2, newBinopCode(type, ltrue, t1, t2), newUniopCode(Goto, lfalse));
			}
		case 11: // Exp -> NOT Exp 
			{
				return translateCond(children[1], lfalse, ltrue);
			}
		default:
			{
				Operand *t1 = newTemp();
				InterCodes *c1 = translateExp(root, t1);
				return codesConcat(3, c1, newBinopCode(Ne, ltrue, t1, newConst(0)), newUniopCode(Goto, lfalse));
			}
	}
}


InterCodes* translateFunDec(syntaxTreeNode* root){
#ifdef DEBUG
	printf("enter translateFunDec\n");
#endif
	int degree = root->degree;
	syntaxTreeNode** children = root->children;
	switch(root->prodnum) {
		case 1: // FunDec -> ID LP VarList RP
			return codesConcat(2, newFuncCode(children[0]->idval), translate(children[2]));
		case 2: // FunDec -> ID LP RP
			return newFuncCode(children[0]->idval);
	}
}

InterCodes* translateDec(syntaxTreeNode* root) {
#ifdef DEBUG
	printf("enter translateDec\n");
#endif
	int degree = root->degree;
	syntaxTreeNode** children = root->children;
	switch(root->prodnum) {
		case 1: // Dec -> VarDec
			return translateVarDec(children[0]);
		case 2: // Dec -> VarDec ASSIGNOP Exp
			{
				/* to be continued */ ///
				InterCodes *c1 = translateVarDec(children[0]);
				printf("flkasjdflaksjflaksfjalskjfklasdfjalksd\n");
				symbol_t* sbl =  getSymbol(children[0]->attr.name);
				assert(sbl);
				Operand *op = sbl->type.var->op;
				assert(op);
				return codesConcat(2, c1, translateExp(children[2], op));
			}
	}
}


InterCodes* translateStmt(syntaxTreeNode* root){
#ifdef DEBUG
	printf("enter translateStmt\n");
#endif
	assert(root);
	int degree = root->degree;
	syntaxTreeNode** children = root->children;
	switch(root->prodnum) {
		case 1:  // Stmt -> Exp SEMI
			printf("exp semi\n");
			return translateExp(children[0], NULL);
		case 2:  // Stmp -> CompSt 
//			enterScope();
			printf("exp compst\n");
			return translateCompst(children[0]);
//			leaveScope();
		case 3: // Stmp -> RETURN Exp SEMI
			{
				printf("return exp semi\n");
				Operand *t1 = newTemp();
				InterCodes* c1 = translateExp(children[1], t1);
				InterCodes* c2 = newUniopCode(Return, t1);
				return codesConcat(2, c1, c2);
			}
		case 4: // Stmp -> IF LP Exp RP Stmt
			{
				printf("if lp exp rp stmt \n");
				Operand *l1 = newLabel(), *l2 = newLabel();
				InterCodes *c1 = translateCond(children[2], l1, l2);
assert(children[4]);
				InterCodes *c2 = translateStmt(children[4]);
				return codesConcat(4, c1, newUniopCode(Label, l1), c2, newUniopCode(Label, l2));
			}
		case 5: // Stmp -> IF LP Exp RP Stmp ELSE Stmt
			{
				printf("if lp exp rp stmt else stmt \n");
				Operand *l1 = newLabel(), *l2 = newLabel(), *l3 = newLabel();
				InterCodes *c1 = translateCond(children[2], l1, l2);
assert(children[4]);
				InterCodes *c2 = translateStmt(children[4]);
assert(children[6]);
				InterCodes *c3 = translateStmt(children[6]);
				return codesConcat(7, c1, newUniopCode(Label, l1), c2, newUniopCode(Goto,l3), newUniopCode(Label, l2), c3, newUniopCode(Label, l3));
			}
		case 6: // WHILE LP Exp RP Stmt
			{
				printf("while lp exp rp stmt\n");
				Operand *l1 = newLabel(), *l2 = newLabel(), *l3 = newLabel();
				InterCodes *c1 = translateCond(children[2], l2, l3);
assert(children[4]);
				InterCodes *c2 = translateStmt(children[4]);
				return codesConcat(6, newUniopCode(Label, l1), c1, newUniopCode(Label, l2), c2, newUniopCode(Goto,l1), newUniopCode(Label, l3));
			}
	}
}

InterCodes* translateArgs(syntaxTreeNode* root,  OperandList** list) {
#ifdef DEBUG
	printf("enter translateArgs\n");
#endif
	/* we use double pointer here because we need to reverse the order of arguments, i.e. we need to change parameter 'list' itself */
	int degree = root->degree;
	syntaxTreeNode** children = root->children;
	Operand *t1 = newTemp();
	if (children[0]->prodnum == 13)
		printf("no argu\n");
	else if (children[0]->prodnum == 14)
		printf("have argu\n");
	InterCodes* c1 = translateExp(children[0], t1);
	printf("hhhhhhhhhhhhhhhhhhh\n");
	OperandList* node = (OperandList*)malloc(sizeof(OperandList));
	node->op = t1;
	node->next = *list; /* reverse the order of arguments */
	*list = node;
	switch (root->prodnum) {
		case 1: // Args -> Exp COMMA Args
				return codesConcat(2, c1, translateArgs(children[2], list)); 
		case 2: // Args -> Exp 
				return c1;
	}
}
InterCodes* translateVarDec(syntaxTreeNode* root) {
#ifdef DEBUG
	printf("enter translateVarDec\n");
#endif
	int degree = root->degree;
	syntaxTreeNode** children = root->children;
	switch (root->prodnum) {
		case 1: // VarDec -> ID
			{
				if (root->attr.inStruct) {
					printf("in struct now\n");
					return NULL;
				}
				symbol_t *sbl = (symbol_t*)malloc(sizeof(symbol_t));
				strcpy(sbl->name, children[0]->idval);
				sbl->kind = var;
				sbl->type.var = (varType_t*)malloc(sizeof(varType_t));
				sbl->type.var->type = root->attr.synType;
				sbl->type.var->isParam = 0;
				assert(sbl->type.var->type);
				sbl->type.var->op = newTemp();
				insertSymbol(sbl);
				if (root->attr.synType->kind == ARRAY) 
					return newDecCode(sbl->type.var->op, getSize(root->attr.synType));
				if (root->attr.synType->kind == STRUCTURE) 
					return newDecCode(sbl->type.var->op, getSize(root->attr.synType));
				return NULL;

			}
		case 2: // VarDec -> VarDec LB INT RB
			{
				return translateVarDec(children[0]);
			}
	}
}

void generateIR(const char* f, InterCodes* codes) {
	if (!codes) {
		printf("empty codes\n");
		return;
	}
	printf("in generateIR\n");
	FILE *file = freopen(f, "w", stdout);
	InterCodes *ptr = codes;
	InterCode *code;
	do {
		code = ptr->code;
		switch(code->kind) {
			case Func:
				printf("FUNCTION %s :\n", code->u.funcdec.name);
				break;
			case Param:
				printf("PARAM t%d\n", code->u.uniop.op->u.var_no);
				break;
			case Return:
				printf("RETURN ");
				if (code->u.uniop.op->kind == Variable)
					printf("t%d\n", code->u.uniop.op->u.var_no);
				else if (code->u.uniop.op->kind == Constant)
					printf("#%d\n", code->u.uniop.op->u.val);
				break;
			case Arg:
				printf("ARG ");
				if (code->u.uniop.op->kind == Address)
					printf("&t%d\n", code->u.uniop.op->u.var_no);
				else if (code->u.uniop.op->kind == Variable)
					printf("t%d\n", code->u.uniop.op->u.var_no);
				else if (code->u.uniop.op->kind == Constant)
					printf("#%d\n", code->u.uniop.op->u.val);
				break;
			case Call:
				printf("t%d := CALL %s\n", code->u.call.result->u.var_no, code->u.call.name);
				break;
			case Read:
				printf("READ t%d\n", code->u.uniop.op->u.var_no);
				break;
			case Write:
				printf("WRITE t%d\n", code->u.uniop.op->u.var_no);
				break;
			case Label:
				printf("LABEL label%d :\n", code->u.uniop.op->u.var_no);
				break;
				
			case Goto:
				printf("GOTO label%d\n", code->u.uniop.op->u.var_no);
				break;
			case Assign: 
				{
					if (code->u.assign.left->kind == Dereference)
						printf("*");
					printf("t%d := ", code->u.assign.left->u.var_no);
					if (code->u.assign.right->kind == Dereference)
						printf("*t%d", code->u.assign.right->u.var_no);
					else if (code->u.assign.right->kind == Variable)
						printf("t%d", code->u.assign.right->u.var_no);
					else if (code->u.assign.right->kind == Constant)
						printf("#%d", code->u.assign.right->u.val);
					else if (code->u.assign.right->kind == Address)
						printf("&t%d", code->u.assign.right->u.var_no);
					printf("\n");
				}
				break;
			case Add:
			case Sub:
			case Mul:
			case Div:
				printf("t%d := ", code->u.binop.result->u.var_no);

				if (code->u.binop.op1->kind == Address)
					printf("&t%d", code->u.binop.op1->u.var_no);
				else if (code->u.binop.op1->kind == Variable)
					printf("t%d", code->u.binop.op1->u.var_no);
				else if (code->u.binop.op1->kind == Constant)
					printf("#%d", code->u.binop.op1->u.val);
				printf(" ");
				if (code->kind == Add) 
					printf("+ ");
				else if (code->kind == Sub) 
					printf("- ");
				else if (code->kind == Mul) 
					printf("* ");
				else if (code->kind == Div) 
					printf("/ ");

				if (code->u.binop.op2->kind == Address)
					printf("&t%d\n", code->u.binop.op2->u.var_no);
				else if (code->u.binop.op2->kind == Variable)
					printf("t%d\n", code->u.binop.op2->u.var_no);
				else if (code->u.binop.op2->kind == Constant)
					printf("#%d\n", code->u.binop.op2->u.val);
				break;
		case Eq:
		case Ne:
		case Gt:
		case Ge:
		case Lt:
		case Le:
				printf("IF ");

				if (code->u.binop.op1->kind == Variable)
					printf("t%d ", code->u.binop.op1->u.var_no);
				else if (code->u.binop.op1->kind == Constant)
					printf("#%d ", code->u.binop.op1->u.val);

				if (code->kind == Eq) 
					printf("== ");
				else if (code->kind == Ne)
					printf("!= ");
				else if (code->kind == Lt)
					printf("< ");
				else if (code->kind == Le)
					printf("<= ");
				else if (code->kind == Gt)
					printf("> ");
				else if (code->kind == Ge)
					printf(">= ");

				if (code->u.binop.op2->kind == Variable)
					printf("t%d ", code->u.binop.op2->u.var_no);
				else if (code->u.binop.op2->kind == Constant)
					printf("#%d ", code->u.binop.op2->u.val);

				printf(" GOTO label%d\n", code->u.binop.result->u.var_no);
				break;

		case Decl:
				printf("DEC t%d %d\n", code->u.dec.addr->u.var_no, code->u.dec.size);
				break;

				

		default:
				assert(0);
		}
		ptr = ptr->next;
	} while (ptr != codes);
	close(stdout);
}

