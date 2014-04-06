%{
#include <stdio.h>
#include "syntax.tab.h"
%}
INT [-+]?[0-9]+
FLOAT [-+]?([0-9]*\.?[0-9]+|[0-9]+\.)

%%
[ \t] {printf("SPACE\n");}
"\n" {printf("NEWLINE\n");}
{INT} {printf("INT\n");}
{FLOAT} {printf("FLOAT\n");}
";" {printf("SEMI\n"); }
"," {printf("COMMA\n"); }
"=" {printf("ASSIGNOP\n"); }
">"|"<"|">="|"<="|"=="|"!=" {printf("RELOP\n"); }
"+" {printf("PLUS\n"); }
"-" {printf("SUB\n"); }
"*" {printf("STAR\n"); }
"/" {printf("DIV\n"); }
"&&" {printf("AND\n"); }
"||" {printf("OR\n"); }
"!" {printf("NOT\n"); }
"." {printf("DOT\n"); }
"(" {printf("LP\n"); }
")" {printf("RP\n"); }
"[" {printf("LB\n"); }
"]" {printf("RB\n"); }
"{" {printf("LC\n"); }
"}" {printf("RC\n"); }
"struct" {printf("STRUCT\n");}
"return" {printf("RETURN\n");}
"if" {printf("IF\n");}
"else" {printf("ELSE\n");}
"while" {printf("WHILE\n");}
"int"|"float" {printf("TYPE\n");}
[_A-Za-z]+[A-Za-z0-9_]* {printf("ID\n");}
. {printf("Error type A at line %d: Mysterious character \'%s\'\n", yylineno, yytext);}
%%
[ \t] {printf("SPACE\n");}
"\n" {printf("NEWLINE\n");}
{INT} {printf("INT\n");}
{FLOAT} {printf("FLOAT\n");}
";" {printf("SEMI\n"); }
"," {printf("COMMA\n"); }
"=" {printf("ASSIGNOP\n"); }
">"|"<"|">="|"<="|"=="|"!=" {printf("RELOP\n"); }
"+" {printf("PLUS\n"); }
"-" {printf("SUB\n"); }
"*" {printf("STAR\n"); }
"/" {printf("DIV\n"); }
"&&" {printf("AND\n"); }
"||" {printf("OR\n"); }
"!" {printf("NOT\n"); }
"." {printf("DOT\n"); }
"(" {printf("LP\n"); }
")" {printf("RP\n"); }
"[" {printf("LB\n"); }
"]" {printf("RB\n"); }
"{" {printf("LC\n"); }
"}" {printf("RC\n"); }
"struct" {printf("STRUCT\n");}
"return" {printf("RETURN\n");}
"if" {printf("IF\n");}
"else" {printf("ELSE\n");}
"while" {printf("WHILE\n");}
"int"|"float" {printf("TYPE\n");}
[_A-Za-z]+[A-Za-z0-9_]* {printf("ID\n");}
. {printf("Error type A at line %d: Mysterious character \'%s\'\n", yylineno, yytext);}
%%
