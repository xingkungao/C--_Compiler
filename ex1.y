%{
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "symbol.h"
#include "lex.yy.c"
syntaxTreeNode* createNode(type_t type, int prodnum, char *name, int degree, ...); void preOrderTraversal(syntaxTreeNode *root, int level);
void freeMem(syntaxTreeNode* root);
int errorTag = 0;
syntaxTreeNode* root;
%}

/* declared types */
%union {
	syntaxTreeNode* node;
}


/* declared tokens */
%token <node> INT FLOAT ID SEMI COMMA ASSIGNOP RELOP PLUS MINUS STAR DIV AND OR DOT NOT TYPE LP RP LB RB LC RC STRUCT RETURN IF ELSE WHILE
%type <node> ExtDefList ExtDef ExtDecList Specifier StructSpecifier OptTag Tag VarDec FunDec VarList ParamDec CompSt StmtList Program Stmt DefList Def DecList Dec Exp Args error

/* declared operators */
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE %right ASSIGNOP %left AND OR %left RELOP %left PLUS MINUS 
%left STAR DIV

%right NOT
%left LP RP
%left LB RB
%left DOT
%locations
%%

/* high level definition */
Program : ExtDefList {
						$$ = createNode(Program, 1, "Program", 1, $1);
						root = $$;
//						if (!errorTag) 
//							preOrderTraversal($$, 1);
			//			freeMem($$);
					}
		;
ExtDefList : ExtDef ExtDefList  {$$ = createNode(ExtDefList, 1, "ExtDefList", 2, $1, $2);}
		   |                    {$$ = createNode(ExtDefList, 2, "ExtDefList", 0);}
		   ;

ExtDef : Specifier ExtDecList SEMI {$$ = createNode(ExtDef, 1, "ExtDef", 3, $1, $2, $3);}
	   | Specifier SEMI {$$ = createNode(ExtDef, 2, "ExtDef", 2, $1, $2);}
	   | Specifier FunDec CompSt {$$ = createNode(ExtDef, 3, "ExtDef", 3, $1, $2, $3); }
	   | error SEMI
	   ;

ExtDecList: VarDec {$$ = createNode(ExtDecList, 1, "ExtDecList", 1, $1);}
		  | VarDec COMMA ExtDecList {$$ = createNode(ExtDecList, 2, "ExtDecList", 3, $1, $2, $3); }
		  ;

/* specifiers */
Specifier : TYPE {$$ = createNode(Specifier, 1, "Specifier", 1, $1);}
		  | StructSpecifier {$$ = createNode(Specifier, 2, "Specifier", 1, $1);} ;

StructSpecifier : STRUCT OptTag LC DefList RC {$$ = createNode(StructSpecifier, 1, "StructSpecifier", 5, $1, $2, $3, $4, $5);}
				| STRUCT Tag {$$ = createNode(StructSpecifier, 2, "StructSpecifier", 2, $1, $2);}
				;

OptTag : ID {$$ = createNode(OptTag, 1, "OptTag", 1, $1);} 
	   | {$$ = createNode(OptTag, 2, "OptTag", 0);}
	   ;

Tag : ID {$$ = createNode(Tag, 1, "Tag", 1, $1);}
	;

/* declarators */
VarDec : ID {$$ = createNode(VarDec, 1, "VarDec", 1, $1);}
	   | VarDec LB INT RB {$$ = createNode(VarDec, 2, "VarDec", 4, $1, $2, $3, $4);}
	   ;

FunDec : ID LP VarList RP {$$ = createNode(FunDec, 1, "FunDec", 4, $1, $2, $3, $4);}
	   | ID LP RP {$$ = createNode(FunDec, 2, "FunDec", 3, $1, $2, $3);}
	   | error RP
	   ;

VarList : ParamDec COMMA VarList {$$ = createNode(VarList, 1, "VarList", 3, $1, $2, $2);}
		| ParamDec {$$ = createNode(VarList, 2, "VarList", 1, $1);}
		;

ParamDec : Specifier VarDec {$$ = createNode(ParamDec, 1, "ParamDec", 2, $1, $2);}
		 ;

/* statement */
CompSt : LC DefList StmtList RC {$$ = createNode(CompSt, 1, "CompSt", 4, $1, $2, $3, $4);}
	   | error RC

	   ;

StmtList : Stmt StmtList {$$ = createNode(StmtList, 1, "StmtList", 2, $1, $2);}
		 | {$$ = createNode(StmtList, 2, "StmtList", 0);}
		 ;

Stmt : Exp SEMI {$$ = createNode(Stmt, 1, "Stmt", 2, $1, $2);}
	 | CompSt {$$ = createNode(Stmt, 2, "Stmt", 1, $1);}
	 | RETURN Exp SEMI {$$ = createNode(Stmt, 3, "Stmt", 3, $1, $2, $3);}
	 | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE {$$ = createNode(Stmt, 4, "Stmt", 4, $1, $2, $3, $4);}
	 | IF LP Exp RP Stmt ELSE Stmt {$$ = createNode(Stmt, 5, "Stmt", 7, $1, $2, $3, $4, $5, $6, $7);}
	 | WHILE LP Exp RP Stmt {$$ = createNode(Stmt, 6, "Stmt", 5, $1, $2, $3, $4, $5);}
	 | error SEMI 
	 ;

/* local definition */
DefList : Def DefList {$$ = createNode(DefList, 1, "DefList", 2, $1, $2);}
		| {$$ = createNode(DefList, 2, "DefList", 0);}
		;

Def : Specifier DecList SEMI {$$ = createNode(Def, 1, "Def", 3, $1, $2, $3);}
	| error SEMI
	;

DecList : Dec {$$ = createNode(DecList, 1, "DecList", 1, $1);}
		| Dec COMMA DecList {$$ = createNode(DecList, 2, "DecList", 3, $1, $2, $3);}
		;

Dec : VarDec {$$ = createNode(Dec, 1, "Dec", 1, $1);}
	| VarDec ASSIGNOP Exp {$$ = createNode(Dec, 2, "Dec", 3, $1, $2, $3);}
	;

/* expressions */
Exp : Exp ASSIGNOP Exp {$$ = createNode(Exp, 1, "Exp", 3, $1, $2, $3);}
	| Exp AND Exp {$$ = createNode(Exp, 2, "Exp", 3, $1, $2, $3);}
	| Exp OR Exp {$$ = createNode(Exp, 3, "Exp", 3, $1, $2, $3);}
	| Exp RELOP Exp {$$ = createNode(Exp, 4, "Exp", 3, $1, $2, $3);}
	| Exp PLUS Exp {$$ = createNode(Exp, 5, "Exp", 3, $1, $2, $3);}
	| Exp MINUS Exp {$$ = createNode(Exp, 6, "Exp", 3, $1, $2, $3);}
	| Exp STAR Exp {$$ = createNode(Exp, 7, "Exp", 3, $1, $2, $3);}
	| Exp DIV Exp {$$ = createNode(Exp, 8, "Exp", 3, $1, $2, $3);}
	| LP Exp RP {$$ = createNode(Exp, 9, "Exp", 3, $1, $2, $3);}
	| MINUS Exp {$$ = createNode(Exp, 10, "Exp", 2, $1, $2);}
	| NOT Exp {$$ = createNode(Exp, 11, "Exp", 2, $1, $2);}
	| ID LP Args RP {$$ = createNode(Exp, 12, "Exp", 4, $1, $2, $3, $4);}
	| ID LP RP {$$ = createNode(Exp, 13, "Exp", 3, $1, $2, $3);}
	| Exp LB Exp RB {$$ = createNode(Exp, 14, "Exp", 4, $1, $2, $3, $4);}
	| Exp DOT ID {$$ = createNode(Exp, 15, "Exp", 3, $1, $2, $3);}
	| ID {$$ = createNode(Exp, 16, "Exp", 1, $1);}
	| INT {$$ = createNode(Exp, 17, "Exp", 1, $1);}
	| FLOAT {$$ = createNode(Exp, 18, "Exp", 1, $1);}
	| error RP
	;
Args : Exp COMMA Args {$$ = createNode(Args, 1, "Args", 3, $1, $2, $3);}
	 | Exp {$$ = createNode(Args, 2, "Args", 1, $1);}
	 ;
%%

int yyerror(char *s) {
	errorTag = 1;
	printf("Error type 2 at line %d: %s\n", yylloc.first_line, s);
}

int yywrap() {
	return 1;
}

syntaxTreeNode* createNode(type_t type, int prodnum, char *name, int degree, ...) {
	syntaxTreeNode *newNode;
	int i;
	va_list ap;
	if (!(newNode = (syntaxTreeNode*)malloc(sizeof(syntaxTreeNode)))) {
		printf("error allocating memory");
		exit(1);
	}
	memset(newNode, 0, sizeof(*newNode));
	strncpy(newNode->name, name, 16);
	newNode->degree = degree;
	newNode->type = type;
	newNode->prodnum = prodnum;
	if (!degree) {
		newNode->lineno = 0; 
		return newNode;
	}
	va_start(ap, degree);
	if (!(newNode->children = (syntaxTreeNode**)malloc(degree * sizeof(syntaxTreeNode*)))) {
		printf("error allocating memory");
		exit(1);
	}
	memset(newNode->children, 0, degree * sizeof(newNode));
	for (i = 0; i < degree; i++) {
		newNode->children[i] = va_arg(ap,syntaxTreeNode*);
	}
	va_end(ap);
	newNode->lineno = newNode->children[0]->lineno;
	return newNode;
}

void preOrderTraversal(syntaxTreeNode* root, int level) {
	int i;
	if (root->type > TWHILE && !root->degree)
		return;
	for (i = 0; i < level; i++) 
		printf("  ");
	if (root->type > TWHILE)  {
		printf("%s (%d)\n", root->name, root->lineno);
		for (i = 0; i < root->degree; i++) 
			preOrderTraversal(root->children[i], level + 1);
	}
	else if (root->type == TID) {
		printf("%s: %s \n", root->name, root->idval);
	}
	else if (root->type == TTYPE) {
		printf("%s: %s\n",  root->name, root->otherval);
	}
	else if (root->type == TINT) {
		printf("%s: %d\n", root->name, root->ival);
	}
	else if (root->type == TFLOAT) {
		printf("%s: %f\n", root->name, root->fval);
	}
	else {
		printf("%s \n", root->name);
	}
		
}

void freeMem(syntaxTreeNode* root) {
	int i;
	if (!root) return;
	if (root->type >= TWHILE) {
		for (i = 0; i < root->degree; i++) 
			freeMem(root->children[i]);
		free(root->children);
	}
	free(root);
}
