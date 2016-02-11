%{
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "aps_struct.h"

/* prototypes */
nodeType *opr(int oper, int nops, ...);
nodeType *id(int i);
nodeType *con(int value);
void freeNode(nodeType *p);
int ex(nodeType *p);
nodeType *constant(typeValue typeConst, int integer, char* string);
void display_tree(nodeType *p);

%}

%union {
   int t_int;
   int t_bool;
   char* t_string;
   nodeType *t_noeud;
}


%error-verbose
%token<t_int>  NUM
%token<t_string>  IDENT PROG
%token  ADD SUB MUL DIV
%token  NOT AND OR EQ LT 

%token  VAR CONST SET IF WHILE
%token<t_string>  INT BOOL VOID
%token<t_string>  FALSE TRUE

%type <t_noeud> Expr Stat Dec Cmds Prog
%type <t_string> Type

%start Affichage

%%
 /** **/

Affichage : 
         Prog {
            nodeType* prog = opr(PROG, 1, $1);
            display_tree(prog); 
            printf("\n");
            freeNode($1);
         }
         ;
Prog:
        '['Cmds']' {$$ = $2;}
         ;
Cmds:
         Stat            {$$ = $1; }
         |Dec ';' Cmds   {$$ = opr(';', 2, $1, $3); }
         |Stat ';' Cmds  {$$ = opr(';', 2, $1, $3); }
         ;
Dec:
         VAR IDENT Type {
            nodeType* ident = constant(cons_string, 0, $2);
            nodeType* type = constant(cons_string, 0, $3);
            $$ = opr(VAR,2,ident,type);
         }
         |CONST IDENT Type Expr {
            nodeType* ident = constant(cons_string, 0, $2);
            nodeType* type = constant(cons_string, 0, $3);
            $$ = opr(CONST,3,ident,type, $4);
         }
         ;
Stat:
        SET IDENT Expr {
            nodeType* ident = constant(cons_string, 0, $2);
            $$ = opr(SET,2,ident,$3);
         }
        |IF Expr Prog Prog {$$ = opr(IF,3,$2,$3,$4);}
        |WHILE Expr Prog {$$ = opr(WHILE,2,$2,$3);}
        ;
Expr: 
        NUM {$$=constant(cons_integer, $1, NULL);}
        |IDENT {$$=constant(cons_string, 0, $1);}
        |FALSE {$$=constant(cons_string, 0, $1);}
        |TRUE  {$$=constant(cons_string, 0, $1);}
        
        |'('NOT Expr')' {$$ = opr(NOT, 1, $3);}
        |'('AND Expr Expr')' {$$ = opr(AND, 2, $3, $4);}
        |'('OR Expr Expr')'  {$$ = opr(OR, 2, $3, $4);}
        |'('EQ Expr Expr')'  {$$ = opr(EQ, 2, $3, $4);}
        |'('LT Expr Expr')'  {$$ = opr(LT, 2, $3, $4);}
        |'('ADD Expr Expr')' {$$ = opr(ADD, 2, $3, $4);}
        |'('SUB Expr Expr')' {$$ = opr(SUB, 2, $3, $4);}
        |'('MUL Expr Expr')' {$$ = opr(MUL, 2, $3, $4);}
        |'('DIV Expr Expr')' {$$ = opr(DIV, 2, $3, $4);}
        ;
        
Type:
        VOID 
        |BOOL 
        |INT 
;



%%

nodeType *constant(typeValue typeConst, int integer, char* string) {
   nodeType *p;
   /* allocate node */
   if ((p = malloc(sizeof(nodeType))) == NULL)
      yyerror("out of memory");
   /* copy information */
   p->type = typeCon;
   p->con.typeConst = typeConst;
   if( typeConst == cons_integer)
      p->con.integer = integer;
   else
        p->con.string = strdup(string);
   return p;
}

nodeType *opr(int oper, int nops, ...) {
   va_list ap;
   nodeType *p;
   int i;
   /* allocate node */
   if ((p = malloc(sizeof(nodeType))) == NULL)
      yyerror("out of memory");
   if ((p->opr.op = malloc(nops * sizeof(nodeType *))) == NULL)
      yyerror("out of memory");
   /* copy information */
   p->type = typeOpr;
   p->opr.oper = oper;
   p->opr.nops = nops;
   va_start(ap, nops);
   for (i = 0; i < nops; i++)
      p->opr.op[i] = va_arg(ap, nodeType*);
   va_end(ap);
   return p;
}

void freeNode(nodeType *p) {
   int i;
   if (!p) return;
   if (p->type == typeOpr) {
   for (i = 0; i < p->opr.nops; i++)
      freeNode(p->opr.op[i]);
   free(p->opr.op);
   }
   if(p->type == typeCon)
      if( p->con.typeConst == cons_string)
         free(p->con.string);
   free (p);
}

int yyerror(char *s) {
        printf("yyerror : %s\n",s);
        return 0;
}

void display_tree(nodeType *p){
   if(p == NULL)
      return;
   if(p->type == typeCon){
      if(p->con.typeConst == cons_integer)
         printf ("%d ", p->con.integer);
      else {
         printf ("%s ", p->con.string);
      }
   }
   else{
      char* s;
      switch(p->opr.oper){
         case WHILE: s = "WHILE"; break;
         case IF: s = "IF"; break;
         case PROG: s = "PROG"; break;
         case SET: s = "SET"; break;
         case VAR: s = "VAR"; break;
         case CONST: s = "CONST"; break;
         case ADD: s = "add"; break;
         case SUB: s = "sub"; break;
         case MUL: s = "mul"; break;
         case DIV: s = "div"; break;
         case EQ: s = "eq"; break;
         case LT: s = "lt"; break;
         case ';': s = ";"; break;
      }
      printf("%s ", s);
      int i;
      for (i = 0; i < p->opr.nops; i++)
         display_tree(p->opr.op[i]);
      
   }
}


int main(int argc, char** argv) {
   extern FILE * yyin;
  
   if(argc > 1){
      printf("lecture par fichier %s \n",argv[1]);
      yyin = fopen(argv[1], "r");
      if(yyin == NULL){
         perror("fopen yyin");
      }
   }
   do {
      yyparse();
   } while (!feof(yyin));
   fclose(yyin);
   return 0;
}
