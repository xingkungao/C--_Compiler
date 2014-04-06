%{
#include <stdio.h>
#include "parse.tab.h"
%}
INT [-+]?[0-9]+
FLOAT [-+]?([0-9]*\.?[0-9]+|[0-9]+\.)

%%
[ \t]
"\n" 
{INT} {return INT;}
{FLOAT} {return FLOAT;}
";" {return SEMI;}
"," {return COMMA;}
"=" {return ASSIGNOP;}
">"|"<"|">="|"<="|"=="|"!=" {return RELOP;}
"+" {return PLUS;}
"-" {return MINUS;}
"*" {return STAR;}
"/" {return DIV;}
"&&" {return AND;}
"||" {return OR;}
"!" {return NOT;}
"." {return DOT;}
"(" {return LP;}
")" {return RP;}
"[" {return LB;}
"]" {return RB;}
"{" {return LC;}
"}" {return RC;}
"struct" {return STRUCT;}
"return" {return RETURN;}
"if" {return IF;}
"else" {return ELSE;}
"while" {return WHILE;}
"int"|"float" {return TYPE;}
[_A-Za-z]+[A-Za-z0-9_]* {return ID;}
. {printf("Error type A at line %d: Mysterious character \'%s\'\n", yylineno, yytext);}
%%
