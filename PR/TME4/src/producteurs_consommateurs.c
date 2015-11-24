#define _XOPEN_SOUCE 700
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include "../include/producteur_consommateur.h"

#define SHM_TAILLE 100

int ps;

char stack[SHM_TAILLE];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t sem_libre;
sem_t sem_occupe;

void push(char c) {
	sem_wait(&sem_libre);
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
	int nb_thread_consommateurs;
	int nb_thread_producteurs;
	pthread_t *tid_producteurs;
	pthread_t *tid_consommateurs;
	pthread_attr_t attr;
	int i;

	if (argc != 3) {
		printf("syntaxe %s nbProducteurs nbConsommateurs \n", argv[0]);
		exit(EXIT_FAILURE);
	}

	sem_init(&sem_libre, 0, SHM_TAILLE);
	sem_init(&sem_occupe, 0, 0);
	nb_thread_producteurs = atoi(argv[1]);
	nb_thread_consommateurs = atoi(argv[2]);

	tid_producteurs = (pthread_t*) malloc(
			sizeof(pthread_t) * (nb_thread_producteurs));
	tid_consommateurs = (pthread_t*) malloc(
			sizeof(pthread_t) * (nb_thread_consommateurs));

	if (pthread_attr_init(&attr) != 0) {
		perror("pthread init attibuts \n");
		exit(1);
	}
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	/* producteurs*/
	for (i = 0; i < nb_thread_producteurs; i++) {

		if (pthread_create(&tid_producteurs[i], NULL, thread_create, NULL)
				!= 0) {
			perror("pthread_create \n");
			exit(1);
		}
	}
	/* consommateurs*/
	for (i = 0; i < nb_thread_consommateurs; i++) {

		if (pthread_create(&tid_consommateurs[i], NULL, thread_consommateur,
		NULL) != 0) {
			perror("pthread_create \n");
			exit(1);
		}
	}

	for (i = 0; i < nb_thread_producteurs; i++) {

		if (pthread_join(tid_producteurs[i], NULL) != 0) {
			perror("pthread_join");
			exit(1);
		}
	}
	for (i = 0; i < nb_thread_consommateurs; i++) {

		if (pthread_join(tid_consommateurs[i], NULL) != 0) {
			perror("pthread_join");
			exit(1);
		}
	}

	free(tid_producteurs);
	free(tid_consommateurs);

	return EXIT_SUCCESS;
}
