#define _XOPEN_SOUCE 700

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

int global = 0;
int flag = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t mutex_cond = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

sem_t sem_var;

void section_critic() {
//	 pthread_mutex_init(&mutex[i], NULL); autre methode init

	pthread_mutex_lock(&mutex);
	global++;
	pthread_mutex_unlock(&mutex);
}

void cond_critic() {
//	 pthread_cond_init(&cond[i],NULL);  autre methode init

	pthread_mutex_lock(&mutex_cond);
	while (!flag) {
		printf("avant\n");
		pthread_cond_wait(&cond, &mutex_cond);
	}
	pthread_mutex_unlock(&mutex_cond);
	printf("apres\n");
}

void cond_signal() {
	pthread_mutex_lock(&mutex_cond);
	printf("signaler ...\n");
	flag = 1;
//	pthread_cond_signal(&cond);

	pthread_cond_broadcast(&cond);

	pthread_mutex_unlock(&mutex_cond);
}

void section_sem() {
	/** init sem **/
	sem_init(&sem_var, 0, 1);

	/** operation sur sem **/
	sem_wait(&sem_var);
	global++;
	sem_post(&sem_var);
}

void *run_thread(void *args) {
	int* nb = (int*) args;
	int *pt = malloc(sizeof(int));
	printf("Argument recu %d tid %ld \n", (*nb),
			(unsigned long int) pthread_self());
	*pt = 2 * (*nb);
	cond_critic();
	return (void*) pt;
}

void t_create_thread() {
	pthread_t tid;
	pthread_attr_t attr;

	int* status;
	int arg_thread = 5;

	/** init thread attributs **/
	if (pthread_attr_init(&attr) != 0) {
		perror("pthread init attibuts \n");
		exit(1);
	}
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	if (pthread_create(&tid, &attr, run_thread, &arg_thread) != 0) {
		perror("pthread_create \n");
		exit(1);
	}
	sleep(5);
	cond_signal();
	sleep(3);
	if (pthread_join(tid, (void**) &status) != 0) {
		perror("pthread_join");
		exit(1);
	} else {
		printf("Thread retourne %d \n", *status);
		free(status);
	}

}
//
//int main(void) {
//	flag = 0;
//	t_create_thread();
//
//	return EXIT_SUCCESS;
//}
