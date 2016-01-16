#include <setjmp.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

char f_stack[1024], g_stack[1024];
int f_size, g_size;
jmp_buf f_buf, g_buf;
char *top_stack;

void f(){
	int n=0;
	while(1){
		printf(" f : %d \n", n++);
		sleep(1);
		if(setjmp(f_buf) ==0 ){
			//sauvegarde de la pile dans f_stack
			// 1 definir le nb d'octet a recopier
			// memcpy(void *dest, const void *src, size_t n);
			f_size = (void*)top_stack - (void*)&n;
			printf(" char* %d , int * %d\n",sizeof(char*),sizeof(int*));
			memcpy(f_stack, &n, f_size);
			longjmp(g_buf,1);
		}else{
			//restaure la pile f_stack
			memcpy(top_stack - f_size, f_stack, f_size);
		}

	}

}


void g(){
	int m=1000;
	while(1){
		printf(" g : %d \n", m++);
		sleep(1);
		if(setjmp(g_buf) ==0 ){
			//sauvegarde
			g_size = (void*)top_stack - (void*)&m;
			printf(" g_size %d \n",g_size);
			memcpy(g_stack, &m, g_size );
			longjmp(f_buf,1);
		}else{
			//restaure
			memcpy(top_stack - g_size, g_stack, g_size);
		}

	}

}

int main(void){
	char toto;
	//placer @ du sommet de pile ds top_stack
	top_stack = &toto;

	if(setjmp(g_buf)==0){
		f();	
	}else{
		g();	
	}
	return EXIT_SUCCESS;
}

