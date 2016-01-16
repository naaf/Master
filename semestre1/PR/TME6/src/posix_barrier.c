#include<stdio.h>
#include<stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>

sem_t *sem_mutex;
sem_t *sem_block;
const char* sem_nom1 = "/posix_barrier1";
const char* sem_nom2 = "/posix_barrier2";

void wait_barrier( NB_PCS) {
	int valeur = 0;
	sem_wait(sem_mutex);
	sem_getvalue(sem_mutex, &valeur);
	if (valeur) {
		sem_wait(sem_block);
	} else {
		int i;
		for(i = 1 ; i < NB_PCS; i++){
			sem_post(sem_block);
		}
	}
}

void process(int NB_PCS) {
	printf("avant barriere fils %d \n", getpid());
	wait_barrier(NB_PCS);
	printf("apres barriere fils %d \n", getpid());
	if (sem_close(sem_mutex) == -1) {
		perror("sem_close");
		exit(-1);
	}
}

void nfork(int nb_fils) {
	printf("pere %d \n", getpid());
	int i;
	for (i = 0; i < nb_fils; i++) {
		switch (fork()) {
		case 0: /* fils*/
			process(nb_fils);
			exit(EXIT_SUCCESS);
			break;
		default:/* pere*/

			break;
		}
	}
}

int main(int argc, char* argv[]) {

	if (argc < 2) {
		printf("%s nbProcessus\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	int const nb_fils = atoi(argv[1]);

	if ((sem_mutex = sem_open(sem_nom1, O_CREAT | O_EXCL | O_RDWR, 0600, nb_fils))
			== SEM_FAILED) {
		perror("sem_open");
		exit(1);
	}
	if ((sem_block = sem_open(sem_nom2, O_CREAT | O_EXCL | O_RDWR, 0600, 0))
			== SEM_FAILED) {
		perror("sem_open");
		exit(1);
	}
	nfork(nb_fils);

	if (sem_close(sem_mutex) == -1) {
		perror("sem_close");
		exit(-1);
	}
	if (sem_unlink(sem_nom1) == -1) {
		perror("sem_unlink");
		exit(-1);
	}
	if (sem_unlink(sem_nom2) == -1) {
		perror("sem_unlink");
		exit(-1);
	}

	return EXIT_SUCCESS;
}
