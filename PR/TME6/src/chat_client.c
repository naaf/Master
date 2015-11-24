#include "../include/chat_common.h"
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

const char* shmsrv_nom;
const char* shmcli_nom;

void handler(int s) {
	printf("liberation des ressource\n");
	if (shm_unlink(shmcli_nom) == -1) {
		perror("shm_unlink");
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}

void *thread_lecture(void *arg) {

	struct myshm* p_shm;
	struct message* request;
	int shm_id;

	if ((shm_id = shm_open(shmcli_nom, O_CREAT | O_EXCL | O_RDWR, 0600))
			== -1) {
		perror("shm_open");
		exit(-1);
	}
	if (ftruncate(shm_id, LENGTH_SHM) == -1) {
		perror("ftruncate");
		exit(-1);
	}

	if ((p_shm = (struct myshm*) mmap(NULL, LENGTH_SHM, PROT_READ | PROT_WRITE,
	MAP_SHARED, shm_id, 0)) == MAP_FAILED) {
		perror("mmap");
		exit(-1);
	}
	p_shm->nb = 0;
	sem_init(&p_shm->sem, 1, 1);
	sem_init(&p_shm->sem_message, 1, 0);
	while (1) {

		sem_wait(&p_shm->sem_message);
		sem_wait(&p_shm->sem);
		if (p_shm->nb) {
			p_shm->nb--;
			request = &(p_shm->messages[p_shm->nb]);
			printf("msg recu ==> %s\n", request->content);
		}
		sem_post(&p_shm->sem);

	}
	return NULL;
}

void *thread_ecriture(void *arg) {
	struct myshm* p_shm;
	struct message* request;
	int shm_id;
	if ((shm_id = shm_open(shmsrv_nom, O_CREAT | O_RDWR, 0600)) == -1) {
		perror("shm_open");
		exit(-1);
	}
	if (ftruncate(shm_id, LENGTH_SHM) == -1) {
		perror("ftruncate");
		exit(-1);
	}

	if ((p_shm = (struct myshm*) mmap(NULL, LENGTH_SHM, PROT_READ | PROT_WRITE,
	MAP_SHARED, shm_id, 0)) == MAP_FAILED) {
		perror("mmap");
		exit(-1);
	}



	printf("Client %s demarre avec srv %s \n", shmcli_nom, shmsrv_nom);
	int nb = 0;
	char buffer[TAILLE_MESS] = { 0 };

	/* connexion */
	sem_wait(&p_shm->sem);
	request = &(p_shm->messages[p_shm->nb++]);
	strncpy(request->content, shmcli_nom, TAILLE_MESS);
	request->type = CONNEXION;
	sem_post(&p_shm->sem_message);
	sem_post(&p_shm->sem);

	while (1) {

		printf("\ncl%d ===> mess: ", getpid());
		fgets(buffer, TAILLE_MESS, stdin);

		sem_wait(&p_shm->sem);
		request = &(p_shm->messages[p_shm->nb++]);
		strcpy(request->content, buffer);
		request->type = DIFFUSION;
		sem_post(&p_shm->sem_message);
		sem_post(&p_shm->sem);

	}
	return NULL;
}

int main(int argc, char **argv) {

	pthread_t thread_lec, thread_ecr;

	if (argc != 3) {
		printf("error syntaxe ex: %s /client_shm:0 /serveur_shm:0\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	shmcli_nom = argv[1];
	shmsrv_nom = argv[2];

	if (pthread_create(&thread_lec, NULL, thread_lecture, NULL) != 0) {
		perror("pthread_create \n");
		exit(1);
	}
	if (pthread_create(&thread_ecr, NULL, thread_ecriture, NULL) != 0) {
		perror("pthread_create \n");
		exit(1);
	}
	signal(SIGINT, handler);
	pthread_join(thread_lec, NULL);
	pthread_join(thread_ecr, NULL);
	return EXIT_SUCCESS;
}

