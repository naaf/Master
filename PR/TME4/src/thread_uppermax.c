#define _REENTRANT

#define _XOPEN_SOUCE 700
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>

int upper(char *path);
char **tasks;
int nb_task_todo;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void get_task(int* ind_task) {
	pthread_mutex_lock(&mutex);
	if (nb_task_todo) {
		nb_task_todo--;
		*ind_task = 2 + nb_task_todo;
	}else{
		*ind_task = 0;
	}
	pthread_mutex_unlock(&mutex);
}

void *thread_run(void *arg) {

	int ind_task;
	get_task(&ind_task);
	while (ind_task) {

		printf("thread %u => %d \n", (unsigned int) pthread_self(), ind_task);
		upper(tasks[ind_task]);
		get_task(&ind_task);
	}

	return NULL;
}

int main(int argc, char **argv) {
	int nb_thread;
	pthread_t *tid = NULL;
	int i = 0;

	if (argc < 3) {
		printf("syntaxe %s nbThread pathFile ...\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	tasks = argv;
	nb_thread = atoi(argv[1]);
	nb_task_todo = argc - 2;

	if (nb_thread <= 0 || nb_thread > nb_task_todo) {
		printf("nbThread doit etre sup à 0 et inf nbFichier\n");
		exit(EXIT_FAILURE);
	}

	tid = (pthread_t*) malloc(sizeof(pthread_t) * (nb_thread));

	printf("argc => %d , nbT => %d , nbDO => %d\n", argc, nb_thread,
			nb_task_todo);

	for (i = 0; i < nb_thread; i++) {

		if (pthread_create(&tid[i], NULL, thread_run, NULL) != 0) {
			perror("pthread_create \n");
			exit(1);
		}
	}
	for (i = 0; i < nb_thread; i++) {

		if (pthread_join(tid[i], NULL) != 0) {
			perror("pthread_join");
			exit(1);
		}
	}
	free(tid);
	return EXIT_SUCCESS;
}
