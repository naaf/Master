
#define _XOPEN_SOUCE 700

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>


#define NB_THREADS 9
int global;
int nb_thread_execute;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_cond = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;



void wait_barrier(int n_thread){
	pthread_mutex_lock (&mutex);
	if(NB_THREADS == ++nb_thread_execute){
		/* printf("if thread %d \n",nb_thread_execute); */
		pthread_cond_broadcast(&cond);
		pthread_mutex_unlock (&mutex);
	}else{
		
		/*printf("el thread %d \n",nb_thread_execute);*/
		pthread_mutex_lock(&mutex_cond);
		pthread_mutex_unlock (&mutex);
		pthread_cond_wait(&cond, &mutex_cond);
	
		pthread_mutex_unlock(&mutex_cond);
	}
	
}

void* thread_func (void *arg) {
   printf ("avant barriere\n");
   wait_barrier (NB_THREADS);
   printf ("après barriere\n");
   pthread_exit ( (void*)0);
}

int main(int argc, char **argv) {

	pthread_t tid[NB_THREADS], thread_affiche;
	pthread_attr_t attr;
	
	int* status;
	int i,j[NB_THREADS];
	
	global = 0;
	nb_thread_execute = 0;

	

	/* cree N thread add */
	for(i = 0; i < NB_THREADS; i++ ){ 
		j[i] = i;
		if(pthread_create(&tid[i],NULL,thread_func, &j[i]) != 0){
			perror("pthread_create \n");
			exit(1);	
		}
		
	}
	
	/* join N thread add */
	for(i = 0; i < NB_THREADS; i++){ 
		
		if(pthread_join(tid[i],(void**)&status) != 0){
			perror("pthread_join");
			exit(1);
		}
	}
	
	printf("fin main global %d \n",nb_thread_execute);
	return EXIT_SUCCESS;
}
