
#ifndef APS_STRUCT_H
#define APS_STRUCT_H

#include <stdio.h>
#include <string.h>
#include <malloc.h>

/** struct **/

typedef enum { typeCon, typeOpr } nodeEnum;
typedef enum { cons_integer, cons_string, cons_bool } typeValue;

/* constants */
typedef struct {
   typeValue typeConst;
   int integer; 
   char* string;
} conNodeType;


/* operators */
typedef struct {
 int oper; /* operator */
 int nops; /* number of operands */
 struct nodeTypeTag **op; /* operands */
} oprNodeType;

typedef struct nodeTypeTag {
 nodeEnum type; /* type of node */
 union {
   conNodeType con; /* constants */
   oprNodeType opr; /* operators */
 };
} nodeType;



#endif