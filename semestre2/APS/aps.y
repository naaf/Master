%{

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

%}

%token  num ident
%token  add sub mul div
%token	not and or eq lt
%token  FDL

%%
 /** **/
Prog:
	'['Cmds']';

Cmds:
	Stat
	|Dec; Cmds
	|Stat; Cmds;
Dec:
	VAR ident Type
	|CONST ident Type Expr;
Stat:
	SET ident Expr
	|IF Expr Prog Prog
	|WHILE Expr Prog;
Expr: 
	true|false
	|num|ident
	|'('not Expr')'|'('and Expr Expr')'|'('or Expr Expr')'
	|'('eq Expr Expr')'|'('lt Expr Expr')'
	|'('add Expr Expr')'|'('sub Expr Expr')'
	|'('mul Expr Expr')'|'('div Expr Expr')';
Type:
	'void'|'bool'|'int';


Expr:
num {$$ = $1;}
|true {$$ = $1;}
|false {$$ = $1;}
|ident {$$ = $1;}
| '('add' 'Expr' 'Expr')'  {printf("app(add,%d,%d)",$,2);}
| '('sub' 'Expr' 'Expr')'  {printf("app(sub,%d,%d)",$,2);}
| '('mul' 'Expr' 'Expr')'  {printf("app(mul,%d,%d)",$,2);}
| '('div' 'Expr' 'Expr')'  {printf("app(div,%d,%d)",$,2);}





%%
int yyerror(void){
  fprintf(stderr, "erreur de syntaxe\n");
  return 1;
}

int main(int argc, char** argv) {
    yyparse();
    return 0;
}
