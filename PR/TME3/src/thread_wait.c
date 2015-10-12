
#define _XOPEN_SOUCE 700

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>


#define N 4
int global;
int nb_thread_execute;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_cond = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void *run_thread(void *args){
	srand((int) pthread_self() ) ;
	int* nb = (int*) args;
	int *pt =(int*) malloc (sizeof (int) );
	printf("Argument recu %d tid %d\n",(*nb), (long unsigned)(int) pthread_self());
	*pt = (int) (10*((double)rand())/ RAND_MAX);

	pthread_mutex_lock (&mutex);
	if(N == ++nb_thread_execute)
		pthread_cond_signal(&cond);
	global += *pt ;
	pthread_mutex_unlock (&mutex);

	return (void*)pt ;
	 
}

void* affiche_global(void *args){
	pthread_mutex_lock(&mutex_cond);
	pthread_cond_wait(&cond, &mutex_cond);
	
	printf("thread_affiche global %d \n",global);
	pthread_mutex_unlock(&mutex_cond);
}

int main(int argc, char **argv) {

	pthread_t tid[N], thread_affiche;
	pthread_attr_t attr;
	int* status;
	int i,j[N];
	
	global = 0;
	nb_thread_execute = 0;

	/* create thread affiche */
	if(pthread_create(&thread_affiche,NULL,affiche_global, NULL) != 0){
		perror("pthread_create \n");
		exit(1);	
	}

	/* cree N thread add */
	for(i = 0; i < N; i++ ){ 
		j[i] = i;
		if(pthread_create(&tid[i],NULL,run_thread, &j[i]) != 0){
			perror("pthread_create \n");
			exit(1);	
		}
		
	}
	/* join N thread add */
	for(i = 0; i < N; i++){ 
		
		if(pthread_join(tid[i],(void**)&status) != 0){
			perror("pthread_join");
			exit(1);
		}else{
			printf("Thread %d fini avec %d \n",i, *status);
			free(status);
		}
	}
	if(pthread_join(thread_affiche,(void**)&status) != 0){
		perror("pthread_join");
		exit(1);
	}
	printf("fin main global %d \n",global);
	return EXIT_SUCCESS;
}
