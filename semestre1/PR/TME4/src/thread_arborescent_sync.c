#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>

int max;
long nbThreads;
int attente;

pthread_cond_t sync_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t sync_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t nbT_mutex = PTHREAD_MUTEX_INITIALIZER;

void* func_ecoute(void* arg) {

	pthread_mutex_lock(&nbT_mutex);
	nbThreads++;
	pthread_mutex_unlock(&nbT_mutex);

	int i, nb;
	int *param;
	int *lvl = (int*) arg;
	pthread_t *tid;

	nb = (*lvl) + 1;

	if (*lvl < max) {
		printf("tid : %u L%d cree %d fils\n", (unsigned int) pthread_self(),
				*lvl, nb);

		pthread_mutex_lock(&sync_mutex);
		while (nb > attente) {
			pthread_cond_wait(&sync_cond, &sync_mutex);
		}
		pthread_mutex_unlock(&sync_mutex);

		param = (int*) malloc(sizeof(int));
		*param = nb;
		tid = calloc(nb, sizeof(pthread_t));
		for (i = 0; i < nb; i++) {
			pthread_create((tid + i), 0, func_ecoute, param);
		}
		for (i = 0; i < nb; i++)
			pthread_join(tid[i], NULL);
	}

	if (*lvl > 1)
		pthread_exit((void*) 0);

	return (void*) 0;
}

int main(int argc, char **argv) {

	int sig = 0, i = 0;
	sigset_t mask;
	pthread_t tid;

	if (argc != 2) {
		printf("syntaxe %s nbThread \n", argv[0]);
		exit(EXIT_FAILURE);
	}

	max = atoi(argv[1]);
	nbThreads = 0;
	attente = 0;
	sigfillset(&mask);
	if (pthread_sigmask(SIG_SETMASK, &mask, NULL) != 0) {
		perror("pthread_sigmask");
		exit(1);
	}
	if (pthread_create(&tid, NULL, func_ecoute, (void*) &i) != 0) {
		perror("pthread_create \n");
		exit(1);
	}
	sigemptyset(&mask);
	sigaddset(&mask,SIGINT);
	int j;
	for (j = 0; j < max; ++j) {
		/* attent le signal SIGINT */
		sigwait(&mask, &sig);
		/* reveiller les  threads de Lvln*/
		pthread_mutex_lock(&sync_mutex);
		attente++;
		pthread_cond_broadcast(&sync_cond);
		pthread_mutex_unlock(&sync_mutex);
	}

	if (pthread_join(tid, NULL) != 0) {
		perror("pthread_join");
		exit(1);
	}
	printf("fin LVL nombre totale de threads %ld  \n ",nbThreads);

	return EXIT_SUCCESS;
}
