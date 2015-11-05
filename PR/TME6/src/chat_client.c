#define _XOPEN_SOURCE 700

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>

#include <sys/mman.h>
#include <sys/stat.h>        /* Pour les constantes des modes */
#include <fcntl.h>           /* Pour les constantes O_* */
#include <semaphore.h>
#include <signal.h>
#include <string.h>

#include "chat_common.h"

#define LENGTH_SHM sizeof(struct myshm)
#define NB_SHM 2
#define SERVEUR 0
#define CLIENT 1

char* shm_nom[NB_SHM];
int shm_id[NB_SHM];
char *adr_att[NB_SHM];

pthread_t tids[NB_SHM];

void handler(int s) {
	int i;
	for (i = 0; i < NB_SHM; ++i) {
		if (shm_unlink(shm_nom[i]) == -1) {
			perror("shm_unlink");
			exit(EXIT_FAILURE);
		}
	}
	exit(EXIT_SUCCESS);
}

void *thread_lecture(void *arg) {
	char* adr_client;
	int buffer_size = 512;
	char buffer[buffer_size];
	int nb_lus = 0;
	if ((adr_client = mmap(NULL, LENGTH_SHM, PROT_READ | PROT_WRITE,
	MAP_SHARED, shm_id[CLIENT], 0)) == MAP_FAILED) {
		perror("mmap");
		exit(-1);
	}

	while (1) {
		nb_lus = read(shm_id[CLIENT], buffer, buffer_size);
		write(stdout, buffer, nb_lus);
	}

	return NULL;
}
void init_myshm(struct myshm *p_shm) {

}

void *thread_ecriture(void *arg) {
	struct myshm *p_shm;
	struct message *request;
	int *nb;

	if ((p_shm = mmap(NULL, LENGTH_SHM, PROT_READ | PROT_WRITE,
	MAP_SHARED, shm_id[SERVEUR], 0)) == MAP_FAILED) {
		perror("mmap");
		exit(-1);
	}

	printf("nb %d \n", LENGTH_SHM);

	p_shm->nb = 15;
	printf("nb %d \n", p_shm->nb);

//	while (1) {
//		fgets(p_shm->messages[p_shm->nb].content, TAILLE_MESS, stdin);
//		nb_lus = strlen(p_shm->messages[p_shm->nb].content);
//		printf("envois serv : nb:%d M:%s s: %d\n", nb_lus,
//				p_shm->messages[p_shm->nb].content, sizeof(request));
//
//		write(shm_id[SERVEUR], request->content, nb_lus);
//
//	}
	return NULL;
}

int main(int argc, char **argv) {
	int i;
	if (argc != 3) {
		printf("error syntaxe ex: %s 5 4 2 1\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	shm_nom[0] = argv[1];
	shm_nom[1] = argv[2];

	for (i = 0; i < NB_SHM; ++i) {
		if ((shm_id[i] = shm_open(shm_nom[i], O_CREAT | O_RDWR, 0600)) == -1) {
			perror("shm_open");
			exit(-1);
		}
		if (ftruncate(shm_id[i], LENGTH_SHM) == -1) {
			perror("ftruncate");
			exit(-1);
		}
	}

	if (pthread_create(&tids[CLIENT], NULL, thread_lecture, NULL) != 0) {
		perror("pthread_create \n");
		exit(1);
	}
	if (pthread_create(&tids[SERVEUR], NULL, thread_ecriture, NULL) != 0) {
		perror("pthread_create \n");
		exit(1);
	}

	signal(SIGINT, handler);
	pause();

	return EXIT_SUCCESS;
}
