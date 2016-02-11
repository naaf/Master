#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>

pthread_mutex_t sync_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t sync_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t create_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_t** tids;
int index;
int N;

void *thread_create(void *arg) {

	pthread_mutex_lock(&create_mutex);
	if (index >= N) {
		pthread_mutex_unlock(&create_mutex);
		return NULL;
	}

	tids[++index] = (pthread_t*) malloc(sizeof(pthread_t));
	printf("pthread %u ==> T%d\n", (unsigned int) pthread_self(), index);
	if (pthread_create(tids[index], NULL, thread_create, NULL) != 0) {
		perror("pthread_create \n");
		exit(1);
	}
	pthread_mutex_unlock(&create_mutex);

	/* point de sync de tous les threads*/
	pthread_mutex_lock(&sync_mutex);
	pthread_cond_wait(&sync_cond, &sync_mutex);
	pthread_mutex_unlock(&sync_mutex);

	return NULL;
}

int main(int argc, char **argv) {

	int signal = 0, i = 0;
	sigset_t mask;

	index = 0;

	if (argc != 2) {
		printf("syntaxe %s nbThread \n", argv[0]);
		exit(EXIT_FAILURE);
	}
	N = atoi(argv[1]);
	tids = (pthread_t**) malloc(sizeof(pthread_t*) * N);
	tids[index] = (pthread_t*) malloc(sizeof(pthread_t));
	sigfillset(&mask);

	if (pthread_sigmask(SIG_SETMASK, &mask, NULL) != 0) {
		perror("pthread_sigmask");
		exit(1);
	}

	printf("pthread %s ==> T%d\n", "main", index);
	if (pthread_create(tids[index], NULL, thread_create, NULL) != 0) {
		perror("pthread_create \n");
		exit(1);
	}
	/* attent le signal SIGINT*/
	sigemptyset(&mask);
	sigaddset(&mask, SIGINT);
	sigwait(&mask, &signal);

	/* reveiller les autres threads*/
	pthread_mutex_lock(&sync_mutex);
	pthread_cond_broadcast(&sync_cond);
	pthread_mutex_unlock(&sync_mutex);

	for (i = 0; i < N; i++) {

		if (pthread_join(*tids[i], NULL) != 0) {
			perror("pthread_join");
			exit(1);
		}
	}

	/* free ressources */
	for (i = 0; i < N; ++i) {
		free(tids[i]);
	}
	free(tids);
	printf("Tous mes descendants se sont terminés\n");

	return EXIT_SUCCESS;
}
