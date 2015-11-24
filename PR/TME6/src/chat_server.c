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

char* shmsrv_nom;
int shm_id;

char tab_nom[MAX_USERS][TAILLE_MESS];
int nb_users;

void handler(int s) {
	printf("liberation des ressource\n");
	if (shm_unlink(shmsrv_nom) == -1) {
		perror("shm_unlink");
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}

extern char *getName(char *name) {
	int i = 0;
	if (nb_users == 0)
		return NULL;
	while (i < MAX_USERS)
		if (strlen(tab_nom[i]) > 0 && strcmp(tab_nom[i], name) == 0)
			return name;
	return NULL;
}

void ajout_name(char* name) {
	printf("connexion %s \n", name);
	fflush(stdout);
	strncpy(tab_nom[nb_users], name, TAILLE_MESS);
	nb_users++;

}

void supp_name(char* name) {
	int i = 0;
	while (i < MAX_USERS)
		if (tab_nom[i] != NULL && strcmp(tab_nom[i], name) == 0) {
			memset(tab_nom[i], '\0', TAILLE_MESS);
			break;
		}
}
void send_message(char* shmcli_nom, char* message) {
	int shm_id;
	struct myshm* p_shm;
	struct message* request;
	printf("envois %s =>> %s \n", message, shmcli_nom);
	fflush(stdout);
	if ((shm_id = shm_open(shmcli_nom, O_CREAT | O_RDWR, 0600)) == -1) {
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

	sem_wait(&p_shm->sem);
	request = &(p_shm->messages[p_shm->nb++]);
	strncpy(request->content, message, TAILLE_MESS);
	request->type = DIFFUSION;
	sem_post(&p_shm->sem_message);
	sem_post(&p_shm->sem);

	if (munmap(p_shm, LENGTH_SHM) == -1) {
		perror("munmap");
	}

}
void broad_cast(char* message) {
	printf("message diffusé : %s \n", message);
	fflush(stdout);
	int i;
	for (i = 0; i < MAX_USERS; i++) {

		if (strlen(tab_nom[i]) != 0) {
			send_message(tab_nom[i], message);
		}
	}
}
void traitement(struct message* request) {
	switch (request->type) {
	case DIFFUSION:
		broad_cast(request->content);
		break;
	case CONNEXION:
		ajout_name(request->content);
		break;
	case DECONNEXION:
		supp_name(request->content);
		break;
	default:
		printf("error type de request\n");
	}
}
int main(int argc, char **argv) {
	struct myshm* p_shm;
	struct message* request;

	signal(SIGINT, handler);

	if (argc != 2) {
		printf("error syntaxe ex: %s /server_shm:0\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	shmsrv_nom = argv[1];
	nb_users = 0;
	if ((shm_id = shm_open(shmsrv_nom, O_CREAT | O_EXCL | O_RDWR, 0600))
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

	printf(" shm_id %d \n", shm_id);
	sem_init(&p_shm->sem, 1, 1);
	sem_init(&p_shm->sem_message, 1, 0);
	p_shm->nb = 0;
	printf("serveur %s demarré \n", argv[1]);

	while (1) {
		request = NULL;
		sem_wait(&p_shm->sem_message);
		sem_wait(&p_shm->sem);
		if (p_shm->nb) {
			p_shm->nb--;
			request = &(p_shm->messages[p_shm->nb]);
		}
		sem_post(&p_shm->sem);
		if (request) {
			traitement(request);
		}
	}

	return EXIT_SUCCESS;
}

