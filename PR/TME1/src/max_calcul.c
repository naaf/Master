#define _XOPEN_SOURCE 700
#include<stdio.h>
#include<stdlib.h>
#include"../include/max_func.h"
int main( int argc, char *argv[]){
	int i ;
	int vecteur[argc];
	for(i = 0; i < argc; i++){
		vecteur[i] = atoi(argv[i]);
	}
	printf("Le maximum est : %d\n", max_func(vecteur, argc));
	return 0;
}
