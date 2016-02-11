%option noyywrap
%{

#include <stdlib.h>
#include "aps_struct.h"
#include "aps.tab.h"
%}

nombre [0-9]+
ident [a-zA-Z]+

%%

"not" {return NOT;}
"and" {return AND;}
"or" {return OR;}
"eq" {return EQ;}
"lt" {return LT;}

"add" {return ADD;}
"sub" {return SUB;}
"div" {return DIV;}
"mul" {return MUL;}

"int" {yylval.t_string=strdup(yytext);return INT;}
"bool" {yylval.t_string=strdup(yytext);return BOOL;}
"void" {yylval.t_string=strdup(yytext);return VOID;}

"true" {yylval.t_string=strdup(yytext);return TRUE;}
"false" {yylval.t_string=strdup(yytext);return FALSE;}

"VAR" {return VAR;}
"CONST" {return CONST;}
"SET" {return SET;}
"IF" {return IF;}
"WHILE" {return WHILE;}

{nombre} {yylval.t_int = atoi(yytext); return NUM;}
{ident} { yylval.t_string=strdup(yytext); return IDENT;}
[ \t\n]
. {return yytext[0];}



