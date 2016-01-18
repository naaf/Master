#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NB_THREAD 10

int temp;
int shared_compteur;

void *THREAD_Routine(void *arg) {
	int temp = shared_compteur;
	sched_yield();
	shared_compteur = temp + 1;
	sched_yield();

	printf("compteur ==> %d \n", shared_compteur);
	return NULL;
}

int main(void) {

	pthread_t thread_id[NB_THREAD];
	int i = 0;
	temp = 0;
	shared_compteur = 0;
	for (i = 0; i < NB_THREAD; ++i) {
		if (pthread_create(&thread_id[i], NULL, THREAD_Routine, NULL) != 0) {
			perror("pthread_create");
			exit(EXIT_FAILURE);

		}
	}

	/**
	 * pour enlever les attentes active
	 */
	for (i = 0; i < NB_THREAD; ++i) {
		if (pthread_join(thread_id[i], NULL) != 0) {
			perror("ptread_join");
			exit(EXIT_FAILURE);
		}
	}

//	while (NB_THREAD != shared_compteur) { attente active
//		// ne rien faire
//	}
	printf("TERMINE  NB_THREAD %d == %d shared_compteur  \n", NB_THREAD,
			shared_compteur);
	return EXIT_SUCCESS;
}

