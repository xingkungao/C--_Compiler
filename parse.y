%{
#include <stdio.h>
#include "lex.yy.c"
%}


/*%token INT FLOAT ID SEMI COMMA ASSIGNOP RELOP PLUS MINUS STAR DIV AND OR DOT NOT TYPE LP RP LB RB LC RC STRUCT RETURN IF ELSE WHILE  */
%token INT FLOAT ID SEMI COMMA ASSIGNOP RELOP PLUS MINUS STAR DIV AND OR DOT NOT TYPE LP RP LB RB LC RC STRUCT RETURN IF ELSE WHILE  
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
%%

/* high level definition */
Program : ExtDefList
		;

ExtDefList : ExtDef ExtDefList 
		   |
		   ;

ExtDef : Specifier ExtDecList SEMI
	   | Specifier SEMI
	   | Specifier FunDec CompSt
	   ;

ExtDecList: VarDec
		  | VarDec COMMA ExtDecList
		  ;

/* specifiers */
Specifier : TYPE
		  | StructSpecifier
		  ;

StructSpecifier : STRUCT OptTag LC DefList RC
				|STRUCT Tag
				;

OptTag : ID 
	   |
	   ;

Tag : ID
	;

/* declarators */
VarDec : ID
	   | VarDec LB INT RB
	   ;

FunDec : ID LP VarList RP
	   | ID LP RP
	   ;

VarList : ParamDec COMMA VarList
		| ParamDec
		;

ParamDec : Specifier VarDec
		 ;

/* statement */
CompSt : LC DefList StmtList RC
	   ;

StmtList : Stmt StmtList
		 |
		 ;

Stmt : Exp SEMI
	 | CompSt
	 | RETURN Exp SEMI
	 | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE
	 | IF LP Exp RP Stmt ELSE Stmt
	 | WHILE LP Exp RP Stmt
	 ;

/* local definition */
DefList : Def DefList
		|
		;

Def : Specifier DecList SEMI
	;

DecList : Dec
		| Dec COMMA DecList
		;

Dec : VarDec 
	| VarDec ASSIGNOP Exp
	;

/* expressions */
Exp : Exp ASSIGNOP Exp
	| Exp AND Exp
	| Exp OR Exp
	| Exp RELOP Exp
	| Exp PLUS Exp
	| Exp MINUS Exp
	| Exp STAR Exp
	| Exp DIV Exp
	| LP Exp RP
	| MINUS Exp
	| NOT Exp
	| ID LP Args RP
	| ID LP RP
	| Exp LB Exp RB
	| Exp DOT ID
	| ID
	| INT
	| FLOAT
	;
Args : Exp COMMA Args
	 | Exp
	 ;
%%

int yyerror(char *s) {printf("%s\n", s);}
