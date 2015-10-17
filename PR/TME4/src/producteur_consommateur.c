#define _XOPEN_SOUCE 700
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include "../include/producteur_consommateur.h"

#define TAILLE 100
#define NB_THREAD 2

int stack_size;


char stack[TAILLE];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t sem_libre;
sem_t sem_occupe;

void push(char c) {
	sem_wait(&sem_libre);
	pthread_mutex_lock(&mutex);
	stack[++stack_size] = c;
	pthread_mutex_unlock(&mutex);
	sem_post(&sem_occupe);

}

char pop() {
	char c;
	sem_wait(&sem_occupe);
	pthread_mutex_lock(&mutex);
	c = stack[stack_size--];
	pthread_mutex_unlock(&mutex);
	sem_post(&sem_libre);
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

	sem_init(&sem_libre, 0, TAILLE);
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