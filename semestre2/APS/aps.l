%option noyywrap
%{
#include "aps.tab.h"
#include <stdio.h>
#include <stdlib.h>
%}

nombre [0-9]+
ident [a-zA-Z]+
fdl \n

%%


"not" {return not;}
"and" {return and;}
"or" {return or;}
"eq" {return eq;}
"lt" {return lt;}

"add" {return add;}
"sub" {return sub;}
"div" {return div;}
"mul" {return mul;}

"VAR" {return VAR;}
"CONST" {return CONST;}
"SET" {return SET;}
"IF" {return IF;}
"WHILE" {return WHILE;}

{nombre} {yylval = atoi(yytext); return num;}
{ident} { return ident;}

fdl {return FDL;}
[\t]
. {return yytext[0];}



