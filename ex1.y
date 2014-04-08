%{
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "syntaxTreeNode.h"
#include "lex.yy.c"
syntaxTreeNode* createNode(char *name, int degree, ...);
void preOrderTraversal(syntaxTreeNode *root, int level);
void freeMem(syntaxTreeNode* root);
int errorTag = 0;
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
%nonassoc ELSE
%right ASSIGNOP
%left AND OR
%left RELOP 
%left PLUS MINUS 
%left STAR DIV

%right NOT
%left LP RP
%left LB RB
%left DOT
%locations
%%

/* high level definition */
Program : ExtDefList {
						$$ = createNode("Program", 1, $1);
						if (!errorTag) 
							preOrderTraversal($$, 1);
						freeMem($$);
					}
		;

ExtDefList : ExtDef ExtDefList  {$$ = createNode("ExtDefList", 2, $1, $2);}
		   |                    {$$ = createNode("ExtDefList", 0);}
		   ;

ExtDef : Specifier ExtDecList SEMI {$$ = createNode("ExtDef", 3, $1, $2, $3);}
	   | Specifier SEMI {$$ = createNode("ExtDef", 2, $1, $2);}
	   | Specifier FunDec CompSt {$$ = createNode("ExtDef", 3, $1, $2, $3); }
	   | error SEMI
	   ;

ExtDecList: VarDec {$$ = createNode("ExtDecList", 1, $1);}
		  | VarDec COMMA ExtDecList {$$ = createNode("ExtDecList", 3, $1, $2, $3); }
		  ;

/* specifiers */
Specifier : TYPE {$$ = createNode("Specifier", 1, $1);}
		  | StructSpecifier {$$ = createNode("Specifier", 1, $1);}
		  ;

StructSpecifier : STRUCT OptTag LC DefList RC {$$ = createNode("StructSpecifier", 5, $1, $2, $3, $4, $5);}
				| STRUCT Tag {$$ = createNode("StructSpecifier", 2, $1, $2);}
				;

OptTag : ID {$$ = createNode("OptTag", 1, $1);} 
	   | {$$ = createNode("OptTag", 0);}
	   ;

Tag : ID {$$ = createNode("Tag", 1, $1);}
	;

/* declarators */
VarDec : ID {$$ = createNode("VarDec", 1, $1);}
	   | VarDec LB INT RB {$$ = createNode("VarDec", 4, $1, $2, $3, $4);}
	   ;

FunDec : ID LP VarList RP {$$ = createNode("FunDec", 4, $1, $2, $3, $4);}
	   | ID LP RP {$$ = createNode("FunDec", 3, $1, $2, $3);}
	   | error RP
	   ;

VarList : ParamDec COMMA VarList {$$ = createNode("VarList", 3, $1, $2, $2);}
		| ParamDec {$$ = createNode("VarList", 1, $1);}
		;

ParamDec : Specifier VarDec {$$ = createNode("ParamDec", 2, $1, $2);}
		 ;

/* statement */
CompSt : LC DefList StmtList RC {$$ = createNode("CompSt", 4, $1, $2, $3, $4);}
	   | error RC

	   ;

StmtList : Stmt StmtList {$$ = createNode("StmtList", 2, $1, $2);}
		 | {$$ = createNode("StmtList", 0);}
		 ;

Stmt : Exp SEMI {$$ = createNode("Stmt", 2, $1, $2);}
	 | CompSt {$$ = createNode("Stmt", 1, $1);}
	 | RETURN Exp SEMI {$$ = createNode("Stmt", 3, $1, $2, $3);}
	 | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE {$$ = createNode("Stmt", 4, $1, $2, $3, $4);}
	 | IF LP Exp RP Stmt ELSE Stmt {$$ = createNode("Stmt", 7, $1, $2, $3, $4, $5, $6, $7);}
	 | WHILE LP Exp RP Stmt {$$ = createNode("Stmt", 4, $1, $2, $3, $4);}
	 | error SEMI 
	 ;

/* local definition */
DefList : Def DefList {$$ = createNode("DefList", 2, $1, $2);}
		| {$$ = createNode("DefList", 0);}
		;

Def : Specifier DecList SEMI {$$ = createNode("Def", 3, $1, $2, $3);}
	| error SEMI
	;

DecList : Dec {$$ = createNode("DecList", 1, $1);}
		| Dec COMMA DecList {$$ = createNode("DecList", 3, $1, $2, $3);}
		;

Dec : VarDec {$$ = createNode("Dec", 1, $1);}
	| VarDec ASSIGNOP Exp {$$ = createNode("Dec", 3, $1, $2, $3);}
	;

/* expressions */
Exp : Exp ASSIGNOP Exp {$$ = createNode("Exp", 3, $1, $2, $3);}
	| Exp AND Exp {$$ = createNode("Exp", 3, $1, $2, $3);}
	| Exp OR Exp {$$ = createNode("Exp", 3, $1, $2, $3);}
	| Exp RELOP Exp {$$ = createNode("Exp", 3, $1, $2, $3);}
	| Exp PLUS Exp {$$ = createNode("Exp", 3, $1, $2, $3);}
	| Exp MINUS Exp {$$ = createNode("Exp", 3, $1, $2, $3);}
	| Exp STAR Exp {$$ = createNode("Exp", 3, $1, $2, $3);}
	| Exp DIV Exp {$$ = createNode("Exp", 3, $1, $2, $3);}
	| LP Exp RP {$$ = createNode("Exp", 3, $1, $2, $3);}
	| MINUS Exp {$$ = createNode("Exp", 2, $1, $2);}
	| NOT Exp {$$ = createNode("Exp", 2, $1, $2);}
	| ID LP Args RP {$$ = createNode("Exp", 4, $1, $2, $3, $4);}
	| ID LP RP {$$ = createNode("Exp", 3, $1, $2, $3);}
	| Exp LB Exp RB {$$ = createNode("Exp", 4, $1, $2, $3, $4);}
	| Exp DOT ID {$$ = createNode("Exp", 3, $1, $2, $3);}
	| ID {$$ = createNode("Exp", 1, $1);}
	| INT {$$ = createNode("Exp", 1, $1);}
	| FLOAT {$$ = createNode("Exp", 1, $1);}
	| error RP
	;
Args : Exp COMMA Args {$$ = createNode("Args", 3, $1, $2, $3);}
	 | Exp {$$ = createNode("Args", 1, $1);}
	 ;
%%

int yyerror(char *s) {
	errorTag = 1;
	printf("Error type 2 at line %d: %s\n", yylloc.first_line, s);
}

int yywrap() {
	return 1;
}

syntaxTreeNode* createNode(char *name, int degree, ...) {
	syntaxTreeNode *newNode;
	int i;
	va_list ap;
	va_start(ap, degree);
	if (!(newNode = (syntaxTreeNode*)malloc(sizeof(syntaxTreeNode)))) {
		printf("error allocating memory");
		exit(1);
	}
	memset(newNode, 0, sizeof(*newNode));
	if (!(newNode->children = (syntaxTreeNode**)malloc(degree * sizeof(syntaxTreeNode*)))) {
		printf("error allocating memory");
		exit(1);
	}
	memset(newNode->children, 0, degree * sizeof(newNode));
	for (i = 0; i < degree; i++) {
		newNode->children[i] = va_arg(ap,syntaxTreeNode*);
	}
	va_end(ap);
	strncpy(newNode->nonterminal, name, 16);
	newNode->lineno = degree == 0 ? 0 : newNode->children[0]->lineno;
	newNode->degree = degree;
	newNode->type= CGRAMMAR;
	return newNode;
}

void preOrderTraversal(syntaxTreeNode* root, int level) {
	int i;
	if (root->type == CGRAMMAR && !root->degree)
		return;
	for (i = 0; i < level; i++) 
		printf("  ");
	if (root->type == CGRAMMAR)  {
		printf("%s (%d)\n", root->nonterminal, root->lineno);
		for (i = 0; i < root->degree; i++) 
			preOrderTraversal(root->children[i], level + 1);
	}
	else if (root->type == CID) {
		printf("%s: %s \n", root->terminal, root->idval);
	}
	else if (root->type == CTYPE) {
		printf("%s: %s\n",  root->terminal, root->otherval);
	}
	else if (root->type == CINT) {
		printf("%s: %d\n", root->terminal, root->ival);
	}
	else if (root->type == CFLOAT) {
		printf("%s: %f\n", root->terminal, root->fval);
	}
	else {
		printf("%s \n", root->terminal);
	}
		
}

void freeMem(syntaxTreeNode* root) {
	int i;
	if (!root) return;
	if (root->type == CGRAMMAR) {
		for (i = 0; i < root->degree; i++) 
			freeMem(root->children[i]);
		free(root->children);
	}
	free(root);
}

		
