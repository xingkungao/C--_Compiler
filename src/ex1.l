%{
#include <stdio.h>
#include <string.h>
#include "ex1.tab.h"
#include "syntax.h"
#include "symbol.h"
extern int errorTag;

syntaxTreeNode* createLeaf(char *s, void *p, type_t type, int line); 
int yycolumn = 1;
#define  YY_USER_ACTION yylloc.first_line = yylloc.last_line = yylineno; \
	yylloc.first_column = yycolumn; yylloc.last_column = yycolumn + yyleng -1; \
	yycolumn += yyleng;
%}
%option yylineno
%x COMMENTS

DECIMAL ([1-9]+[0-9]*)|0
OCTAL 0[0-7]*
HEXADECIMAL 0[xX][0-9A-Fa-f]+
FLOAT ([0-9]+\.[0-9]+)|((([0-9]*\.[0-9]+)|([0-9]+\.[0-9]*))[eE][-+][0-9]+)
ID [_A-Za-z]+[A-Za-z0-9_]* 
whitespace [ \t]
newline [\r\n]
RELOP ">"|"<"|">="|"<="|"=="|"!="

%%
"//"		{ 
				char c = input();
				while (c != '\n' && c != EOF)
					c = input();
				if (c == '\n')
					yylineno++;
			//	printf("dump singleline comment\n");
			}

"/*"		{BEGIN(COMMENTS);}
<COMMENTS>"*/" {BEGIN(INITIAL);}
<COMMENTS>"\n" {}
<COMMENTS>. {}

{whitespace}

{newline}	{
				yycolumn = 1;
//				return NEWLINE;
			}

{OCTAL}		{
				int val = 0;
				int i;
				for (i = 1; i < yyleng; i++) {
					val = 8 * val +  (yytext[i] - '0');
				}
				yylval.node = createLeaf("INT", (void*)&val, TINT, yylineno);
				return INT;
			}

{DECIMAL}	{
				int val = atoi(yytext);
				yylval.node = createLeaf("INT", (void*)&val, TINT, yylineno);
				return INT;
			}

{HEXADECIMAL} {
				int val = 0;
				int i;
				for (i = 2; i < yyleng; i++) {
					if ('0' <= yytext[i] && yytext[i] <= '9')
						val = 16 * val +  (yytext[i] - '0');
					else if ('A' <= yytext[i] && yytext[i] <= 'F')
						val = 16 * val +  (yytext[i] - 'A' + 10);
					else if ('a' <= yytext[i] && yytext[i] <= 'f')
						val = 16 * val +  (yytext[i] - 'a' + 10);
				}
				yylval.node = createLeaf("INT", (void*)&val, TINT, yylineno);
				return INT;
			}

{FLOAT}		{
				float val = atof(yytext);
				yylval.node = createLeaf("FLOAT", (void*)&val, TFLOAT, yylineno);
				return FLOAT;
			}

";"			{
				yylval.node = createLeaf("SEMI", yytext, TSEMI, yylineno);
				return SEMI;
			}

","			{
				yylval.node = createLeaf("COMMA", yytext, TCOMMA, yylineno);
				return COMMA;
			}

"="			{
				yylval.node = createLeaf("ASSIGNOP", yytext, TASSIGNOP, yylineno);
				return ASSIGNOP;
			}

{RELOP}     {
				yylval.node = createLeaf("RELOP", yytext, TRELOP, yylineno);
				return RELOP;
			}

"+"			{
				yylval.node = createLeaf("PLUS", yytext, TPLUS, yylineno);
				return PLUS;
			}

"-"			{
				yylval.node = createLeaf("MINUS", yytext, TMINUS, yylineno);
				return MINUS;
			}

"*"			{
				yylval.node = createLeaf("STAR", yytext, TSTAR, yylineno);
				return STAR;
			}

"/"			{
				yylval.node = createLeaf("DIV", yytext, TDIV, yylineno);
				return DIV;
			}

"&&"		{
				yylval.node = createLeaf("AND", yytext, TAND, yylineno);
				return AND;
			}

"||"		{
				yylval.node = createLeaf("OR", yytext, TOR, yylineno);
				return OR;
			}

"!"			{
				yylval.node = createLeaf("NOT", yytext, TNOT, yylineno);
				return NOT;
			}

"."			{
				yylval.node = createLeaf("DOT", yytext, TDOT, yylineno);
				return DOT;
			}

"("			{
				yylval.node = createLeaf("LP", yytext, TLP, yylineno);
				return LP;
			}

")"			{
				yylval.node = createLeaf("RP", yytext, TRP, yylineno);
				return RP;
			}

"["			{
				yylval.node = createLeaf("LB", yytext, TLB, yylineno);
				return LB;
			}

"]"			{
				yylval.node = createLeaf("RB", yytext, TRB, yylineno);
				return RB;
			}

"{"			{
				yylval.node = createLeaf("LC", yytext, TLC, yylineno);
				return LC;
			}

"}"			{
				yylval.node = createLeaf("RC", yytext, TRC, yylineno);
				return RC;
			}

"struct"	{
				yylval.node = createLeaf("STRUCT", yytext, TSTRUCT, yylineno);
				return STRUCT;
			}

"return"	{
				yylval.node = createLeaf("RETURN", yytext, TRETURN, yylineno);
				return RETURN;
			}

"if"		{
				yylval.node = createLeaf("IF", yytext, TIF, yylineno);
				return IF;
			}

"else"		{
				yylval.node = createLeaf("ELSE", yytext, TELSE, yylineno);
				return ELSE;
			}

"while"		{
				yylval.node = createLeaf("WHILE", yytext, TWHILE, yylineno);
				return WHILE;
			}

"int"|"float" {
				yylval.node = createLeaf("TYPE", yytext, TTYPE, yylineno);
				return TYPE;
			  }

{ID}		{ 
				yylval.node = createLeaf("ID", yytext, TID, yylineno);
				return ID;
			}

.			{printf("Error type 1 at line %d: unknown char: %s\n", yylineno, yytext);}
%%

syntaxTreeNode* createLeaf(char* specifier, void *val, type_t type, int line) {
	syntaxTreeNode *newNode;
	if (!(newNode = (syntaxTreeNode*)malloc(sizeof(syntaxTreeNode)))) {
			printf("error allocating memory");
			exit(1);
	}
	memset(newNode, 0, sizeof(*newNode));
	newNode->type = type;
	newNode->lineno = line;
	strncpy(newNode->name, specifier, 16);
	if (type == TINT) {
		newNode->ival = *(int*)val;
	}
	else if (type == TFLOAT) {
		newNode->fval = *(float*)val;
	}
	else if (type == TID) {
		strncpy(newNode->idval, (char*)val, 32);
	}
	else {
		strncpy(newNode->otherval, (char*)val, 32);
	}
	return newNode;
}
