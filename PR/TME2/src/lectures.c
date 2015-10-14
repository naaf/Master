

#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>

void fonction_lis_affiche(FILE* desc){
       char buffer;
       printf("appel \n");
       while(fread(&buffer,1,1,desc) > 0){
               printf(" proc %d lis %c \n",getpid(), buffer);
       }

}

int main (int argc, char **argv) {
       FILE* fd1;
       if ((fd1 = fopen (argv[1],"r")) == NULL){
               perror("fopen");
               return EXIT_FAILURE;
       }
       setvbuf(fd1,NULL,_IONBF,0);
       if(fork() == 0){//fils
               fonction_lis_affiche(fd1);
       }else{
               if(fork() == 0){//fils
                       fonction_lis_affiche(fd1);
               }else{//pere
                       fonction_lis_affiche(fd1);
               }
       }

 return EXIT_SUCCESS;


}


/* version fopen fread */

#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>

void fonction_lis_affiche(FILE* desc){
       char buffer;
       printf("appel \n");
       while(fread(&buffer,1,1,desc) > 0){
               printf(" proc %d lis %c \n",getpid(), buffer);
       }

}

int main (int argc, char **argv) {
       FILE* fd1;
       if ((fd1 = fopen (argv[1],"r")) == NULL){
               perror("fopen");
               return EXIT_FAILURE;
       }
       setvbuf(fd1,NULL,_IONBF,0);
       if(fork() == 0){//fils
               fonction_lis_affiche(fd1);
       }else{
               if(fork() == 0){//fils
                       fonction_lis_affiche(fd1);
               }else{//pere
                       fonction_lis_affiche(fd1);
               }
       }

 return EXIT_SUCCESS;


}
