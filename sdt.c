/*************************************************************************
    > File Name: sdt.c
    > Author: lax
    > Mail: xingkungao@gmail.com
    > Created Time: Sun 04 May 2014 01:04:34 PM CST
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sdt.h"
void traverse(syntaxTreeNode* root, int space);
//#define DEBUG

vtype_t* VTYPEINT;
vtype_t* VTYPEFLOAT;
void init() {
	VTYPEINT = (vtype_t*)malloc(sizeof(vtype_t));
	VTYPEINT->kind = BASIC;
	VTYPEINT->u.basic = 0;
	VTYPEFLOAT = (vtype_t*)malloc(sizeof(vtype_t));
	VTYPEFLOAT->kind = BASIC;
	VTYPEFLOAT->u.basic = 1;
}

void addReadWrite() {
	printf("enter addreadwrite\n");
	symbol_t* read = (symbol_t*)malloc(sizeof(symbol_t));
	strcpy(read->name, "read");
	read->kind = function;
	read->type.func = (funcType_t*)malloc(sizeof(funcType_t));
	read->type.func->returnType = VTYPEINT;   
	read->type.func->paramList = NULL;
	insertSymbol(read);

	symbol_t* write = (symbol_t*)malloc(sizeof(symbol_t));
	strcpy(write->name, "write");
	write->kind = function;
	write->type.func = (funcType_t*)malloc(sizeof(funcType_t));
	write->type.func->returnType = VTYPEINT;   
	typeList_t* wlist = (typeList_t*)malloc(sizeof(typeList_t));
	wlist->type = VTYPEINT;
	wlist->next = NULL;
	write->type.func->paramList = wlist;
	insertSymbol(write);
	printf("leave addreadwrite\n");
}


void checkExtDef(syntaxTreeNode* root, syntaxTreeNode** children, int degree, int space){
#ifdef DEBUG
	printf("enter ExtDef\n");
#endif
	switch(root->prodnum) {
		case 1: // ExtDef -> Specifier ExtDecList SEMI
			traverse(children[0], space);
			children[1]->attr.inhType = children[0]->attr.synType;
			traverse(children[1], space);
			break;
		case 2: // ExtDef -> Specifier SEMI
			traverse(children[0], space);
			break;
		case 3: // ExtDef -> Specifier FunDec CompSt 
			traverse(children[0],space);
			children[1]->attr.inhType = children[0]->attr.synType;
			enterScope();
			traverse(children[1], space);
			children[2]->attr.returnType = children[0]->attr.synType;
			traverse(children[2], space);
			leaveScope();
			break;
		case 4: // ExtDef -> Specifier FunDec SEMI
			{
				traverse(children[0], space);
//			enterScope();
				children[1]->attr.inhType = children[0]->attr.synType;
//				children[1]->attr.defined = 0;
				traverse(children[1], space);
//			leaveScope();
			break;
			}
	}
}


void checkExtDecList(syntaxTreeNode* root, syntaxTreeNode** children, int degree, int space){
#ifdef DEBUG
	printf("enter ExtDecList\n");
#endif
	switch(root->prodnum) {
		case 1: // ExtDecList -> VarDec
			children[0]->attr.inhType = root->attr.inhType;
			traverse(children[0], space);
			break;
		case 2: // ExtDecList -> VarDec COMMA ExtDecList
			children[0]->attr.inhType = root->attr.inhType;
			traverse(children[0], space);
			children[2]->attr.inhType = root->attr.inhType;
			traverse(children[2], space);
			break;
		default:;
	}
}



void checkSpecifier(syntaxTreeNode* root, syntaxTreeNode** children, int degree, int space){
#ifdef DEBUG
	printf("enter Specifier\n");
#endif
	switch(root->prodnum) {
		case 1:  // Specifier -> Type
			root->attr.inhType = root->attr.synType = (strcmp(children[0]->otherval, "float") ? VTYPEINT : VTYPEFLOAT);
assert(root->attr.synType != NULL);
			root->attr.legal = 1;
			break;
		case 2:  // Specifier -> StructSpecifier 
			traverse(children[0], space);
			root->attr.synType = children[0]->attr.synType;
//assert(children[0]->attr.synType);
			root->attr.legal = children[0]->attr.legal;
			break;
		default:;
	}
#ifdef DEBUG
	printf("leave Specifier\n");
#endif
}

void checkStructSpecifier(syntaxTreeNode* root, syntaxTreeNode** children, int degree, int space){
#ifdef DEBUG
	printf("enter StructSpecifier\n");
#endif
	switch(root->prodnum) {
		symbol_t* sbl;
		case 1:  // StructSpecifier -> STRUCT OptTag LC DefList RC
		{
			if (children[1]->children[0]) { 
				sbl = getSymbol((*(children[1]->children))->idval);
				if (!sbl || sbl->depth < scopeDepth) {
					children[3]->attr.inStruct = 1;
					enterScope();
					traverse(children[3], space);
					leaveScope();
					root->attr.synType = (vtype_t*)malloc(sizeof(type_t));
					root->attr.synType->kind = STRUCTURE;
					root->attr.synType->u.structure = children[3]->attr.structure;
					symbol_t* newSymbol = (symbol_t*)malloc(sizeof(symbol_t));
					strcpy(newSymbol->name, children[1]->children[0]->idval);
					newSymbol->kind = var;
					newSymbol->type.var = (varType_t*)malloc(sizeof(varType_t));
					newSymbol->type.var->type = root->attr.synType;
					insertSymbol(newSymbol);
				}
			}
			break;
		}
		case 2:  // StructSpecifier -> STRUCT Tag 
				sbl = getSymbol((*(children[1]->children))->idval);
				if (sbl) {
					root->attr.synType = sbl->type.var->type;
assert(sbl->type.var->type);
				}
				break;
	}
}



void checkVarDec(syntaxTreeNode* root, syntaxTreeNode** children, int degree, int space){
#ifdef DEBUG
	printf("enter VarDec\n");
#endif
	switch(root->prodnum) {
		vtype_t* type;
		case 1:  // VarDec -> ID
			root->attr.synType = root->attr.inhType;
		//	strcpy(root->idval, children[0]->idval); //??
			root->attr.name = children[0]->idval;
			if (root->attr.inStruct) {
				root->attr.structure = (fieldList_t*)malloc(sizeof(fieldList_t));
				memset(root->attr.structure, 0, sizeof(fieldList_t));
				root->attr.structure->type = root->attr.synType;
				root->attr.structure->lineno = children[0]->lineno;
				root->attr.structure->name = (char*)malloc(32);
				strcpy(root->attr.structure->name, children[0]->idval);
				return;
			}
			symbol_t* newsbl = (symbol_t*)malloc(sizeof(symbol_t));
			strcpy(newsbl->name, children[0]->idval);
			newsbl->kind = var;
			newsbl->type.var = (varType_t*)malloc(sizeof(varType_t));
			newsbl->type.var->type = root->attr.inhType;
			insertSymbol(newsbl);
			break;
		case 2:  // VarDec -> VarDec LB INT RB 
			type = (vtype_t*)malloc(sizeof(vtype_t));
			type->kind = ARRAY;
			type->u.array.elem = root->attr.inhType;
			type->u.array.size = children[2]->ival;
			children[0]->attr.inhType = type;
			children[0]->attr.inStruct = root->attr.inStruct;
			traverse(children[0], space);
			root->attr.synType = children[0]->attr.synType;
			root->attr.structure = children[0]->attr.structure;
			root->attr.name = children[0]->attr.name;
			printf("vardec type is %d\n", root->attr.synType->kind);
			break;
	}
}


void checkFunDec(syntaxTreeNode* root, syntaxTreeNode** children, int degree, int space){
#ifdef DEBUG
	printf("enter FunDec\n");
#endif
	symbol_t* sbl = getSymbol(children[0]->idval);
	printf("name is %s\n", sbl->name);
	assert(!sbl);
	if (!sbl) {
		sbl = (symbol_t*)malloc(sizeof(symbol_t));
		strcpy(sbl->name, children[0]->idval);
		sbl->kind = function;
		sbl->type.func = (funcType_t*)malloc(sizeof(funcType_t));
		sbl->type.func->returnType = root->attr.inhType;
assert(sbl->type.func->returnType->kind == BASIC);
		insertSymbol(sbl);
	}

	switch(root->prodnum) {
		case 1:  // FunDec -> ID LP VarList RP
//			enterScope();
			traverse(children[2], space);
//			leaveScope();
			sbl->type.func->paramList = children[2]->attr.arguments;
			break;
		case 2:  // FunDec -> ID LP RP
			sbl->type.func->paramList = 0;
			root->attr.legal = 1;
			break;
	}
}



void checkVarList(syntaxTreeNode* root, syntaxTreeNode** children, int degree, int space){
#ifdef DEBUG
	printf("enter VarList\n");
#endif
	switch(root->prodnum) {
		case 1:  // VarList -> ParamDec COMMA VarList 
//			children[0]->attr.synType = root->attr.arguments->type;
			traverse(children[0], space);
//			type = (vtype_t*)malloc(sizeof(vtype_t));
//			root->attr.arguments->next = children[0]->attr.synType; 
			traverse(children[2], space);
//			root->attr.arguments->next = children[2]->attr.arguments;
			root->attr.arguments = (typeList_t*)malloc(sizeof(typeList_t));
			root->attr.arguments->type = children[0]->attr.synType;
			root->attr.arguments->next = children[2]->attr.arguments;
assert(root->attr.arguments->next);
			break;
		case 2: // VarList -> ParamDec 
			traverse(children[0], space);
			root->attr.arguments = (typeList_t*)malloc(sizeof(typeList_t));
			root->attr.arguments->type = children[0]->attr.synType;
			root->attr.arguments->next = 0;
			break;
		default:;
	}
}



void checkParamDec(syntaxTreeNode* root, syntaxTreeNode** children, int degree, int space){
#ifdef DEBUG
	printf("enter ParamDec\n");
#endif
	switch(root->prodnum) {
		case 1:  // ParamDec -> Specifier VarDec 
			traverse(children[0], space);
			children[1]->attr.inhType = children[0]->attr.synType;
			printf("in paramdec, type is %d\n", children[0]->attr.synType->u.basic);
			printf("in paramdec, type is %d\n", children[0]->attr.synType->kind);
			traverse(children[1], space);
			root->attr.synType = children[1]->attr.synType;
	}
}


void checkCompSt(syntaxTreeNode* root, syntaxTreeNode** children, int degree, int space){
#ifdef DEBUG
	printf("enter Compst\n");
#endif
	switch(root->prodnum) {
		case 1:  // CompSt -> LC DefList StmtList RC 
			traverse(children[1], space);
			children[2]->attr.returnType = root->attr.returnType;
//assert(root->attr.returnType);
			traverse(children[2], space);
	}
}



void checkStmtList(syntaxTreeNode* root, syntaxTreeNode** children, int degree, int space){
#ifdef DEBUG
	printf("enter StmtList\n");
#endif
	switch(root->prodnum) {
		case 1:  // StmtList -> Stmt StmtList
			printf("here here\n");
			children[0]->attr.returnType = root->attr.returnType; 

			printf("here here\n");
assert(children[0]->attr.returnType);
			traverse(children[0], space);
			children[1]->attr.returnType = root->attr.returnType;
			traverse(children[1], space);
			break;
		default:;
	}
}



void checkStmt(syntaxTreeNode* root, syntaxTreeNode** children, int degree, int space){
#ifdef DEBUG
	printf("enter Stmt\n");
#endif
	int i;
assert(root->attr.returnType);
	switch(root->prodnum) {
		case 1:  // Stmt -> Exp SEMI
			children[0]->attr.returnType = root->attr.returnType;
			traverse(children[0], space);
			break;
		case 2:  // Stmp -> CompSt 
			enterScope();
			children[0]->attr.returnType = root->attr.returnType;
			traverse(children[0], space);
			leaveScope();
			break;
		case 3: // Stmp -> RETURN Exp SEMI
			traverse(children[1], space);
			break;
		case 4: // Stmp -> IF LP Exp RP Stmt
		case 5: // Stmp -> IF LP Exp RP Stmp ELSE Stmt
		case 6: // WHILE LP Exp RP Stmt
			for (i = 0; i < degree; i++) {
				children[i]->attr.returnType = root->attr.returnType;
				traverse(children[i], space);
			}
	}
}


void checkDefList(syntaxTreeNode* root, syntaxTreeNode** children, int degree, int space){
#ifdef DEBUG
	printf("enter DefList\n");
#endif
	switch(root->prodnum) {
		case 1:  // DefList -> Def DefList
			children[1]->attr.inStruct = children[0]->attr.inStruct = root->attr.inStruct;
			traverse(children[0], space);
			traverse(children[1], space);
			fieldList_t* list;
			if (root->attr.inStruct) {
				list = root->attr.structure = children[0]->attr.structure;
assert(list);
				while (list->tail) 
					list = list->tail;
				list->tail = children[1]->attr.structure;
			}
			break;
		case 2:  // DefList -> e
			if (root->attr.inStruct) {
				if (root->attr.inStruct)
					root->attr.structure = 0;
			}
			break;
	}
}

void checkDef(syntaxTreeNode* root, syntaxTreeNode** children, int degree, int space){
#ifdef DEBUG
	printf("enter Def\n");
#endif
	switch(root->prodnum) {
		case 1:  // Def -> Specifier DecList SEMI
			traverse(children[0], space);
			children[1]->attr.inStruct = root->attr.inStruct;
			children[1]->attr.inhType = children[0]->attr.synType;
			traverse(children[1], space);
			if (root->attr.inStruct) {
				root->attr.structure = children[1]->attr.structure;
assert(root->attr.structure);
			}
	}
#ifdef DEBUG
	printf("leave Def\n");
#endif
}


void checkDecList(syntaxTreeNode* root, syntaxTreeNode** children, int degree, int space){
#ifdef DEBUG
	printf("enter DecList\n");
#endif
	switch(root->prodnum) {
		case 1:  // DecList -> Dec  
			children[0]->attr.inhType = root->attr.inhType;
			children[0]->attr.inStruct = root->attr.inStruct;
			traverse(children[0], space);
			if (root->attr.inStruct) {
				root->attr.structure = children[0]->attr.structure; 
			}
			break;
		case 2:  // DecList -> Dec COMMA DecList
			children[2]->attr.inhType = children[0]->attr.inhType = root->attr.inhType;
			children[2]->attr.inStruct = children[0]->attr.inStruct = root->attr.inStruct;
			traverse(children[0], space);
			traverse(children[2], space);
			if (root->attr.inStruct) {
assert(children[0]->attr.structure);
				children[0]->attr.structure->tail = children[2]->attr.structure; 
				root->attr.structure = children[0]->attr.structure;
			}
	}
#ifdef DEBUG
	printf("leave DecList\n");
#endif
}


void checkDec(syntaxTreeNode* root, syntaxTreeNode** children, int degree, int space){
#ifdef DEBUG
	printf("enter Dec\n");
#endif
	switch(root->prodnum) {
		case 1:  // Dec -> VarDec 
			children[0]->attr.inStruct = root->attr.inStruct;
			children[0]->attr.inhType = root->attr.inhType;
			traverse(children[0], space);
			if (children[0]->attr.inStruct) 
				root->attr.structure = children[0]->attr.structure;
			break;
		case 2:  // Dec -> VarDec ASSIGNOP Exp
			children[0]->attr.inhType = root->attr.inhType;
			children[0]->attr.inStruct = root->attr.inStruct;
			assert(!(root->attr.inStruct));
			traverse(children[0], space);
			traverse(children[2], space);

			/* to be continued */
			root->attr.synType = children[0]->attr.synType;
			break;
	}
#ifdef DEBUG
	printf("leave Dec\n");
#endif
}



void checkExp(syntaxTreeNode* root, syntaxTreeNode** children, int degree, int space){
#ifdef DEBUG
	printf("enter Exp\n" );
#endif
	symbol_t* sbl;
	fieldList_t* flist;
	int flag;
	switch(root->prodnum) {
		case 1:  // Exp -> Exp ASSIGNOP Exp
			traverse(children[0], space);
			traverse(children[2], space);
			root->attr.synType = children[0]->attr.synType;
/*
			else if (children[0]->attr.legal && children[2]->attr.legal) {
				if (children[0]->attr.synType->kind == STRUCTURE && children[2]->attr.synType->kind == STRUCTURE) {
					if (equalType(children[0]->attr.structure, children[2]->attr.synType)) {
						root->attr.synType = children[0]->attr.synType;
assert(root->attr.synType);
						root->attr.legal = 1;
					}
					if (!strcmp(children[0]->attr.synType->u.structure->name, children[2]->attr.synType->u.structure->name)) {
						root->attr.synType = children[0]->attr.synType;
assert(root->attr.synType);
						root->attr.legal = 1;
					}
					else { 
						reportError5(children[1]->lineno);
						root->attr.legal = 0;
					}
				}
				else if (equalType(children[0]->attr.synType, children[2]->attr.synType)) {
					root->attr.synType = children[0]->attr.synType;
assert(root->attr.synType);
					root->attr.legal = 1;
				}
				else {
					reportError7(children[1]->lineno);
					root->attr.legal = 0;
				}
			}
			else {
				root->attr.legal = 0;
			}
			break;
	*/
			break;
		case 2:  // Exp -> Exp AND Exp
		case 3:  // Exp -> Exp OR Exp
		case 4:  // Exp -> Exp RELOP Exp
		case 5:  // Exp -> Exp PLUS Exp
		case 6:  // Exp -> Exp MINUS Exp
		case 7:  // Exp -> Exp STAR Exp
		case 8:  // Exp -> Exp DIV Exp
			traverse(children[0], space);
			traverse(children[2], space);
			/*
			if (children[0]->attr.legal && children[2]->attr.legal) {
printf("in Exp -> Exp op Exp\n");
printf("syntype of first child is %d\n", children[0]->attr.synType->kind);
printf("syntype of second child is %d\n", children[2]->attr.synType->kind);
				if (equalType(children[0]->attr.synType, children[2]->attr.synType)) {
					root->attr.synType = children[0]->attr.synType;
assert(root->attr.synType);
					root->attr.legal = 1;
				}
				else {
					reportError7(children[1]->lineno);
					root->attr.legal = 0;
				}
			}
			else {
				root->attr.legal = 0;
			}
			*/

			root->attr.synType = children[0]->attr.synType;
			break;
		case 9:  // Exp -> LP Exp RP
		case 10: // Exp -> MINUS Exp
		case 11: // Exp -> NOT Exp
			traverse(children[1], space);
//			root->attr.legal = children[1]->attr.legal;
			root->attr.synType = children[1]->attr.synType;
assert(root->attr.synType);
			break;
		case 12:  // Exp -> ID LP Arg RP
			sbl = getSymbol(children[0]->idval);
			root->attr.synType = sbl->type.func->returnType;
		case 13:  // Exp -> ID LP RP
			sbl = getSymbol(children[0]->idval);
			root->attr.synType = sbl->type.func->returnType;
			/*
			if (sbl) {
				if (sbl->kind != function) {
					reportError11(children[0]->lineno, children[0]->idval);
					root->attr.legal = 0;
				}
				else {
					root->attr.synType = sbl->type.func->returnType;
assert(root->attr.synType);
					if (degree == 4) {
						children[2]->attr.arguments = sbl->type.func->paramList;
assert(children[2]->attr.arguments);
						traverse(children[2], space);
						root->attr.legal = children[2]->attr.legal;
					}
					else if (degree == 3) {
						if (sbl->type.func->paramList) {
							reportError9(root->lineno);
							root->attr.legal = 0;
						}
						else  {
							root->attr.legal = 1;
						}
						
					}
				}
			}
			else {
				reportError2(root->lineno, children[0]->idval);
				root->attr.legal = 0;
			}
			*/
			break;
		case 14:  // Exp -> Exp LB Exp RB
			traverse(children[0], space);
			root->attr.synType = children[0]->attr.synType->u.array.elem;

			/*
			if (children[0]->attr.legal) {
				if (children[0]->attr.synType->kind == ARRAY) {
					traverse(children[2], space);
					if (children[2]->attr.legal) {
						if(children[2]->attr.synType->kind == BASIC && children[2]->attr.synType->u.basic == 0) {
							root->attr.synType = children[0]->attr.synType->u.array.elem;
assert(root->attr.synType);
							root->attr.legal = 1;
						}
						else {
							reportError12(children[2]->lineno);
							root->attr.legal = 0;
						}
					}
					else {
						root->attr.legal = 0;
					}
				}
				else {
					reportError10(children[0]->lineno, children[0]->idval);
					root->attr.legal = 0;
				}
			}
			else {
				root->attr.legal = 0;
			}
			*/
			break;
		case 15:  // Exp -> Exp DOT ID 
			{
				printf("here \n");
			traverse(children[0], space);
				printf("here \n");
			fieldList_t* flist = children[0]->attr.synType->u.structure;
			while (flist) {
				printf("here \n");
				printf("literal %s\n", flist->name);
				if (!strcmp(flist->name, children[2]->idval)) {
					break;
				}
				flist = flist->tail;
			}
			if (flist) 
				root->attr.synType = flist->type; 
			/*
			if (children[0]->attr.legal) {
				if (children[0]->attr.synType->kind != STRUCTURE) {
					reportError13(children[0]->lineno, children[2]->idval);
					root->attr.legal = 0;
					return;
				}
				else {
					flag = 0;
					flist = children[0]->attr.synType->u.structure;
					while (flist) {
assert(flist->name);
						if (!strcmp(flist->name, children[2]->idval)) {
				//			root->attr.synType = children[0]->attr.synType;
//assert(root->attr.synType);
				//			root->attr.legal = children[0]->attr.legal;
							flag = 1;
							break;
						}
						flist = flist->tail;
					}
					if (!flag) {
						reportError14(children[2]->lineno, children[2]->idval);
						root->attr.legal = 0;
						return;
					}
				}


				root->attr.synType = children[0]->attr.synType;
assert(root->attr.synType);
				root->attr.legal = children[0]->attr.legal;
			}
			else {
				root->attr.legal = 0;
			}
			*/
			break;
			}
		case 16:  // Exp -> ID
			sbl = getSymbol(children[0]->idval);
			assert(sbl);
			if (sbl) {
				printf("get id from table: %s\n", sbl->name);
				root->attr.synType = sbl->type.var->type;
				assert(root->attr.synType);
				/*
			fieldList_t* flist = root->attr.synType->u.structure;
			while (flist) {
				printf("here \n");
				printf("literal %s\n", flist->name);
				flist = flist->tail;
			}
			*/
			}
			else {
				reportError1(children[0]->lineno, children[0]->idval);
				root->attr.legal = 0;
			}
			break;
		case 17:  // Exp -> INT
		case 18:  // Exp -> FLOAT

			root->attr.synType = root->prodnum == 17 ? VTYPEINT : VTYPEFLOAT;
assert(root->attr.synType);
			root->attr.legal = 1;

			break;
		default:;
	}
}


			
void checkArgs(syntaxTreeNode* root, syntaxTreeNode** children, int degree, int space){
#ifdef DEBUG
	printf("enter Args\n");
#endif
	traverse(children[0], space);
	if (!root->attr.arguments) {
		reportError9(children[0]->lineno);
		root->attr.legal = 0;
		return;
	}
	else if (!equalType(root->attr.arguments->type, children[0]->attr.synType)){
		reportError9(children[0]->lineno);
		root->attr.legal = 0;
		return;
	}
	switch(root->prodnum) {
		case 1:  // Args -> Exp COMMA Args 
			children[2]->attr.arguments = root->attr.arguments->next;
			traverse(children[2], space);
			root->attr.legal = children[2]->attr.legal;
			break;
		case 2:  // Args -> Exp 
			if (root->attr.arguments->next) { 
				reportError9(children[0]->lineno);
				root->attr.legal = 0;
			}
			else  {
				root->attr.legal = 0;
			}
	}
}


void traverse(syntaxTreeNode* root, int space) {
	if (!root)
			return;

	int i;
#ifdef DEBUG
	for (i = 0; i < space; i++)
		printf(" ");
#endif
	int degree = root->degree;
	syntaxTreeNode** chil = root->children;
	switch(root->type) {
		case ExtDecList:
			checkExtDecList(root, chil,  degree, space+2);
			break;
		case ExtDef:
			checkExtDef(root, chil,  degree, space+2);
			break;
		case Specifier:
			checkSpecifier(root, chil,  degree, space+2);
			break;
		case StructSpecifier:
			checkStructSpecifier(root, chil,  degree, space+2);
			break;
		case VarDec:
			checkVarDec(root, chil,  degree, space+2);
			break;
		case FunDec:
			checkFunDec(root, chil,  degree, space+2); 
			break;
		case VarList:
			checkVarList(root, chil,  degree, space+2);
			break;
		case ParamDec:
			checkParamDec(root, chil,  degree, space+2);
			break;
		case CompSt:
			checkCompSt(root, chil,  degree, space+2);
			break;
		case Stmt:
			checkStmt(root, chil,  degree, space+2);
			break;
		case StmtList:
			checkStmtList(root, chil,  degree, space+2);
			break;
		case DefList:
			checkDefList(root, chil,  degree, space+2);
			break;
		case Def:
			checkDef(root, chil,  degree, space+2);
			break;
		case DecList:
			checkDecList(root, chil,  degree, space+2);
			break;
		case Dec:
			checkDec(root, chil,  degree, space+2);
			break;
		case Exp:
			checkExp(root, chil,  degree, space+2);
			break;
		case Args:
			checkArgs(root, chil,  degree, space+2);
			break;
		default: 
			for (i = 0; i < degree; i++) {
				traverse(chil[i], space +2);
			}
	}
}

