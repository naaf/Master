#define _XOPEN_SOUCE 700
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include "../include/producteur_consommateur.h"

#define SHM_TAILLE 100
#define NB_THREAD 2

int ps;


char stack[SHM_TAILLE];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t sem_var;
sem_t sem_occupe;

void push(char c) {
	sem_wait(&sem_var);
	pthread_mutex_lock(&mutex);
	stack[++ps] = c;
	pthread_mutex_unlock(&mutex);
	sem_post(&sem_occupe);

}

char pop() {
	char c;
	sem_wait(&sem_occupe);
	pthread_mutex_lock(&mutex);
	c = stack[ps--];
	pthread_mutex_unlock(&mutex);
	sem_post(&sem_var);
	return c;
}

void *thread_create(void *arg) {
	PRODUCTEUR
	return NULL;
}

void *thread_consommateur(void *arg) {
	CONSOMMATEUR
	return NULL;
}

int main(int argc, char **argv) {
	pthread_t tid[NB_THREAD];
	int i;

	sem_init(&sem_var, 0, SHM_TAILLE);
	sem_init(&sem_occupe, 0, 0);

	if (pthread_create(&tid[0], NULL, thread_create, NULL) != 0) {
		perror("pthread_create \n");
		exit(1);
	}
	if (pthread_create(&tid[1], NULL, thread_consommateur, NULL) != 0) {
		perror("pthread_create \n");
		exit(1);
	}

	for (i = 0; i < NB_THREAD; i++) {

		if (pthread_join(tid[i], NULL) != 0) {
			perror("pthread_join");
			exit(1);
		}
	}
	printf("fin\n");
	return EXIT_SUCCESS;
}
