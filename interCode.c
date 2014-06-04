/*************************************************************************
    > File Name: interCode.c
    > Author: lax
    > Mail: xingkungao@gmail.com
    > Created Time: Sat 31 May 2014 07:43:21 PM CST
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "interCode.h"
InterCodes* translateParamDec(syntaxTreeNode *root) {
	// ParamDec -> Specifier VarDec
	syntaxTreeNode **children = root->children;
	symbol_t *sbl = getSymbol(children[1]->idval);
	translateExp(children[1]);
	/* to be continued */
	return newUniopCode(Param, sbl->type.var->type); 
}

InterCodes* translateExtDef(syntaxTreeNode *root) {
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
		InterCodes *list = 0;
		for (i = 0; i < root->degree; i++) {
			list = codesConcat(2, list, translate(children[i]));
		}
		return list;
	}
}

InterCodes* translate(syntaxTreeNode* root) {
	if (!root) return NULL;

	int degree = root->degree;
	syntaxTreeNode** children = root->children;
	switch (root->type) {
		case ExtDecList:
			break;
		case ExtDef:
			return translateExtDef(root);
		case Specifier:
			break;
		case StructSpecifier:
			break;
		case VarDec:
			return translateVarDec(root);
		case FunDec:
			return translateFunDec(root);
		case VarList:
			break;
		case ParamDec:
			return translateParamDec(root);
			break;
		case CompSt:
			return translateCompst(root);
		case Stmt:
			return translateExp(root);
		case StmtList:
			break;
		case DefList:
			break;
		case Def:
			break;
		case DecList:
			break;
		case Dec:
			return translateDec(root);
		case Exp:
			checkExp(root, chil,  degree, space+2);
			break;
		case Args:
			checkArgs(root, chil,  degree, space+2);
			break;
		default: 
			int i;
			InterCodes *codes = 0;
			for (i = 0; i < degree; i++) 
				codes = codesConcat(2, list, translate(children[i]));
			return codes;
	}
}

InterCodes* translateCond(syntaxTreeNode* root, Operand* ltrue, Operand* lflase) {
}
		
int getSize (vtype_t type) {
	if (!type) return 0;
	if (type->kind == BASIC) return 4;
	if (type->kind == ARRAY) return type->u.array.size * getSize(type->u.array.elem);
	if (type->kind == STRUCTURE) {
		int cnt = 0;
		fieldList *fields = type->u.structure;
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
	printf("enter Exp\n" );
#endif
	syntaxTreeNode** children = root->children;
	int degree = root->degree;
	switch(root->prodnum) {
		case 1:  // Exp -> Exp ASSIGNOP Exp
			{
				Operand *t1 = (children[0]->prodnum == 16)? newVar(-1) : newAddr(-1);
				Operand *t2 = newTemp();
				InterCodes* c1 = translateExp(children[0], t1);
				InterCodes* c2 = translateExp(children[2], t2);
				return codeConcat(4, c1, c2, newAssignCode(t1, t2), newAssignCode(place, t1));
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
				 InterCdoes* c2 = codesConcat(2, newUniopCode(Label, l1), newAssignCode(place, newConst(1)));
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
					 c3 = newBinopCode(Add, place, t1, t2);
				 else if (root->prodnum == 6)
					 c3 = newBinopCode(Sub, place, t1, t2);
				 else if (root->prodnum == 7)
					 c3 = newBinopCode(Mul, place, t1, t2);
				 else if (root->prodnum == 8)
					 c3 = newBinopCode(Div, place, t1, t2);
				 return  codesConcat(3, c1, c2, c3);
			}

		case 9:  // Exp -> LP Exp RP
			return translateExp(children[1], place);
		case 10: // Exp -> MINUS Exp 
			{
				Operand* t1 = newTemp();
				InterCodes *c1 = translateExp(children[1], t1);
				InterCodes *c2 = newBinopCode(Sub, place, newConst(0), t1);
				return codesConcat(2, c1, c2);
			}

		case 12:  // Exp -> ID LP Arg RP
		case 13:  // Exp -> ID LP RP
			{
				symbol_t sbl = getSymbol(children[0]->idval);
				printf("get one func from symbol table\n");
				assert(sbl);
				if (degree == 3) {
					if (!strcmp(children[0]->idval, "read")) 
						return newUniopCode(Read, place);
					return newCallCode(place, children[0]->idval);
				}
				else {
					OperandList *argList = 0;
					InterCodes *c1 = translateArgs(children[2], argList);
					if (!strcmp(chldren[0]->idval, "write")) 
						return codesConcat(2, c1, newUnipCode(Write, place)); 
					InterCodes* c2 = 0;
					while (argList) {
						 c2 = codesConcat(2, c2, newUniopCode(Arg, argList->op));
						 argList = argList->next;
					}
					return codesConcat(3, c1, c2, newUniopCode(Call, place, children[0]->idval));
				}
			}
		case 14:  // Exp -> Exp LB Exp RB
			{
				Operand *t1 = newVar(-1);
				Operand *t2 = newTemp(), *t3 = newTemp(), *t4 = newTemp();
				InterCodes *c1 = translateExp(children[0], t1);
				InterCodes *c2 = translateExp(children[2], t2);
				InterCodes *c3 = newBinopCode(Mul, t3, t2, newConst(getSize(root->attr.synType)));
				InterCodes *c4 = newBinopCode(Add, t4, t3, t1);

				if (place->u.var_no == -1) {
					place->u.var_no = t4->u.var_no;
					return codesConcat(4, c1, c2, c3, c4);
				}
				return codesConcat(5, c1, c2, c3, c4, codesConcat(newAssignCode(place, newAddr(t4->u.var_no))));
			}

		case 15:  // Exp -> Exp DOT ID 
			{
				Operand *v1 = newVar(-1), *t2 = newTemp();
				InterCodes *c1 = translateExp(children[0], t1);
				int idx = 0;
				FieldList_t *fields = children[0]->attr.syntype->u.structure;
				while (fields) {
					if (!strcmp(field->name, children[2]->idval)) 
						break;
					idx += getSize(fileds->type);
					fields = fields->next;
				}
				InterCodes *c2 = newBinopCode(Add, t2, t1, newConst(idx));
				if (place->u.var_no < 0) {
					place->u.var_no = t2->u.var_no;
					return codesConcat(2, c1, c2);
				}
				return codesConcat(3, c1, c2, newAssignCode(place, newAddr(t2->u.var_no)));

			}
			break;
		case 16:  // Exp -> ID
			{
				symbol_t *sbl = getSymbol(children[0]->idval);
				InterCodes *c1;
				if (place->u.var_no < 0) {
					if (root->attr->synType->kind == BASIC)
						place->kind = Variable;
					else
						place->kind = Dereference;
					/* to be continued */
					place->u.var_no = sbl->fa;
				}
				else {
					if (root->attr->synType->kind == BASIC)
						c1 = newAssignCode(place, sbl);
					else
						;
				}
				return c1;
			}
		case 17:  // Exp -> INT
			{
				return newAssignCode(place, newConst(children[0]->ival));
			}
		case 18:  // Exp -> FLOAT
			{
			}
			break;
		default:;
	}
}


Operands* translateCompst(syntaxTreeNode* root){
	int degree = root->degree;
	syntaxTreeNode** children = root->children;
	switch(root->prodnum) {
		case 1: // Compst -> LC DefList Stmtlist RC
			{
//				enterScope();
				InterCodes *c1 = translate(children[1]);
				InterCodes *c2 = translate(children[2]);
//				leaveScope();
				return codesConcat(2, c1, c2);
			}
	}
}

InterCodes* translateCond(syntaxTreeNode *root, Operand *ltrue, Operand *lfalse) {
	int degree = root->degree;
	syntaxTreeNode** children = root->children;
	switch(root->prodnum) {
		case 2:  // Exp -> Exp AND Exp
			{
				Operand* l1 = newLabel();
				InterCodes* c1 = translateCond(children[0], l1, lfalse);
				InterCodes* c2 = translateCond(children[0], ltrue, lfalse);
				return codesConcat(3, c1, newUniopCode(Label, l1), c2);
			}

		case 3:  // Exp -> Exp OR Exp
			{
				Operand* l1 = newLabel();
				InterCodes* c1 = translateCond(children[0], ltrue, l1);
				InterCodes* c2 = translateCond(children[0], ltrue, lfalse);
				return codesConcat(3, c1, newUniopCode(Label, l1), c2);
			}
		case 4:  // Exp -> Exp RELOP Exp
			{
				Operand* t1 = newTemp(), *t2 = newTemp();
				InterCodes* c1 = translateExp(children[0], t1);
				InterCodes* c2 = translateExp(children[2], t2);
				CodeKind kind;
				if (!strcmp(children[1]->idval, "=="))
					kind = Eq;
				else if (!strcmp(children[1]->idval, "!="))
					kind = Ne;
				else if (!strcmp(children[1]->idval, "<"))
					kind = Lt;
				else if (!strcmp(children[1]->idval, ">"))
					kind = Gt;
				else if (!strcmp(children[1]->idval, "<="))
					kind = Le;
				else if (!strcmp(children[1]->idval, ">="))
					kind = Ge;
				return codesConcat(4, c1, c2, newBinopCode(kind, ltrue, t1, t2), newUniopCode(Goto, lfalse));
			}
		case 11: // Exp -> NOT Exp 
			{
				return translateCond(children[0], lfalse, ltrue);
			}
		default:
			{
				Operand *t1 = newTemp();
				InterCodes *c1 = translateExp(children[0]);
				return codesConcat(3, c1, newBinopCode(Ne, ltrue, t1, newConst(0)), newUniopCode(Goto, lfalse));
			}
	}
}


InterCodes* translateFunDec(syntaxTreeNode* root){
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
	int degree = root->degree;
	syntaxTreeNode** children = root->children;
	switch(root->prodnum) {
		case 1: // Dec -> VarDec
			return translateVarDec(children[0]);
		case 2: // Dec -> VarDec ASSIGNOP Exp
			{
				/* to be continued */
				InterCodes *c1 = translateVarDec(children[0]);
			}
	}
}


InterCodes* translateStmt(syntaxTreeNode* root){
#ifdef DEBUG
	printf("enter check_stmt\n");
#endif
	int degree = root->degree;
	syntaxTreeNode** children = root->children;
	switch(root->prodnum) {
		case 1:  // Stmt -> Exp SEMI
			return translateExp(children[0], NULL);
		case 2:  // Stmp -> CompSt 
			enterScope();
			return translateCompst(children[0]);
			leaveScope();
		case 3: // Stmp -> RETURN Exp SEMI
			{
				Operand *t1 = newTemp();
				InterCodes* c1 = translateExp(children[1], t1);
				InterCodes* c2 = newUniopCode(Return, t1);
				return codesConcat(2, c1, c2);
			}
		case 4: // Stmp -> IF LP Exp RP Stmt
			{
				Operand *l1 = newLabel(), *t2 = newLabel();
				InterCodes *c1 = translateCond(children[2], l1, l2);
				InterCodes *c2 = translateStmt(children[4]);
				return codesConcat(4, c1, newUniopCode(Label, l1), c2, newUniopCode(Label, l2));
			}
		case 5: // Stmp -> IF LP Exp RP Stmp ELSE Stmt
			{
				Operand *l1 = newLabel(), *t2 = newLabel(), *t3 = newLabel();
				InterCodes *c1 = translateCond(children[2], l1, l2);
				InterCodes *c2 = translateStmt(children[4]);
				InterCodes *c3 = translateStmt(children[6]);
				return codesConcat(7, c1, newUniopCode(Label, l1), c2, newUniopCode(Goto,l3), newUniopCode(Label, l2), c3, newUniopCode(Label, l3));
			}
		case 6: // WHILE LP Exp RP Stmt
			{
				Operand *l1 = newLabel(), *t2 = newLabel(), *t3 = newLabel();
				InterCodes *c1 = translateCond(children[2], l1, l2);
				InterCodes *c2 = translateStmt(children[4]);
				return codesConcat(6, newUniopCode(Label, l1), c1, newUniopCode(Label, l2), c2, newUniopCode(Goto,l1), newUniopCode(Label, l3));
			}
	}
}

InterCodes* translateArgs(syntaxTreeNode* root,  OperandList** list) {
	/* we use double pointer here because we need to reverse the order of arguments, i.e. we need to change parameter 'list' itself */
	int degree = root->degree;
	syntaxTreeNode** children = root->children;
	Operand *t1 = newTemp();
	InterCodes* c1 = translateExp(children[0], t1);
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
	int degree = root->degree;
	syntaxTreeNode** children = root->children;
	switch (root->prodnum) {
		case 1: // VarDec -> ID
			if (!root->attr.inStruct) {
				symbol_t *sbl = getSymbol(children[0]->idval);
				if (sbl->kind == var && sbl->type.var->type->kind == ARRAY) {
					return newDecCode(newTemp(), getSize(sbl->type.var->type));
				}
				return NULL;
			}
			return NULL;

		case 2: // VarDec -> VarDec LB INT RB
			{
				return translateVarDec(children[0]);
			}
	}
}

