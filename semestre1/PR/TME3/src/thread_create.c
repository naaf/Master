
#define _XOPEN_SOUCE 700

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>


#define N 4


void *run_thread(void *args){
	int* nb = (int*) args;
	int *pt = malloc (sizeof (int) );
	printf("Argument recu %d tid %d\n",(*nb), (long unsigned)(int) pthread_self());
	*pt = 2 * (*nb);
	return (void*)pt ;
	 
}

int main(int argc, char **argv) {

	pthread_t tid[N];
	pthread_attr_t attr;
	int* status;
	int i,j[N];
	for(i = 0; i < N; i++ ){ 
		j[i] = i;
		if(pthread_create(&tid[i],NULL,run_thread, &j[i]) != 0){
			perror("pthread_create \n");
			exit(1);	
		}
		
	}
	for(i = 0; i < N; i++){ 
		
		if(pthread_join(tid[i],(void**)&status) != 0){
			perror("pthread_join");
			exit(1);
		}else{
			printf("Thread %d fini avec %d \n",i, *status);
			free(status);
		}
	}
	return EXIT_SUCCESS;
}
