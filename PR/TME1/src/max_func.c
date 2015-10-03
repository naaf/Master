#define _XOPEN_SOURCE 700
#include<stdio.h>
#include<stdlib.h>
int max_func(int vecteur[], int taille){
	if( taille == 0){
		printf(" ERROR taille 0");
		return 0;
	}
	if(taille == 1){
		return vecteur[0];
	}
	else{
		int max = vecteur[0];
		int i = 1;
		for(i; i < taille; i++){
			if( vecteur[i] > max){
				max = vecteur[i];			
			}
		} 
		return max;
	}
}
