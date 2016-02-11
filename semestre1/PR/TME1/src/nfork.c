#define _XOPEN_SOURCE 700
#include<stdio.h>
#include<stdlib.h>

int nfork (int nb_fils){
	int i = 0;
	int ret;
	for(i = 0; i < nb_fils; i++){
		switch(ret = fork()){
		case -1: return i;
		break;
		case 0: return 0;
		break;
		default :break;
		}
	}
}

 int main (int arg, char* argv []) {
   int p;
   int i=1; int N = 3;
   do {
     p = nfork (i) ;
       if (p != 0 )
              printf ("%d \n",p);
    } while ((p ==0) && (++i<=N));
   printf ("FIN \n");    
   return EXIT_SUCCESS;
 }
