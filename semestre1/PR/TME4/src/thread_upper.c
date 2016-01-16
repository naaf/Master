
#define _XOPEN_SOUCE 700
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>

int upper(char *path);

int main(int argc, char **argv) {

	if (argc < 2) {
		printf("syntaxe %s pathFile ...\n", argv[0]);
	}

	pthread_t *tid = malloc(sizeof(pthread_t) * (argc - 1));
	int i;

	for (i = 1; i < argc; i++) {

		if (pthread_create(&tid[i], NULL, (void * (*)(void *)) upper, argv[i])
				!= 0) {
			perror("pthread_create \n");
			exit(1);
		}
	}
	for (i = 1; i < argc; i++) {

		if (pthread_join(tid[i], NULL) != 0) {
			perror("pthread_join");
			exit(1);
		}
	}

	return EXIT_SUCCESS;
}
