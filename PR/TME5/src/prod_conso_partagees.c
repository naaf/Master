#define _SVID_SOURCE 1

#include<stdio.h>
#include<stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <signal.h>
#include "../include/producteur_consommateur.h"

#define SHM_TAILLE 104
#define SEM_TAILLE 3
#define SEM_OCCUPE 0
#define SEM_MUTEX 1
#define SEM_LIBRE 2

char *adr_att;
char* adr_stack;
int *tab_val_random;
int shm_id;

int sem_id;
struct sembuf op[SEM_TAILLE];

void prepareP(int sem_type) {

	op[sem_type].sem_num = sem_type;
	op[sem_type].sem_op = -1;
	op[sem_type].sem_flg = 0;
}

void prepareV(int sem_type) {

	op[sem_type].sem_num = sem_type;
	op[sem_type].sem_op = 1;
	op[sem_type].sem_flg = 0;
}

void push(char c) {

	prepareP(SEM_LIBRE);
	semop(sem_id, &op[SEM_LIBRE], 1);
	tab_val_random = (int*) adr_att;
	adr_stack = (adr_att + sizeof(int));

	prepareP(SEM_MUTEX);
	semop(sem_id, &op[SEM_MUTEX], 1);
	adr_stack[tab_val_random[0]] = c;
	printf("===>push %d, sem_occupe %d\n", tab_val_random[0],
			semctl(sem_id, SEM_OCCUPE, GETVAL, 0));
	tab_val_random[0] += 1;

	prepareV(SEM_MUTEX);
	semop(sem_id, &op[SEM_MUTEX], 1);

	prepareV(SEM_OCCUPE);
	semop(sem_id, &op[SEM_OCCUPE], 1);

}

char pop() {
	char c;
	printf("=================>entre pop, sem_occupe %d\n",
			semctl(sem_id, SEM_OCCUPE, GETVAL, 0));
	prepareP(SEM_OCCUPE);
	semop(sem_id, &op[SEM_OCCUPE], 1);

	adr_stack = (adr_att + sizeof(int));
	tab_val_random = (int*) adr_att;

	prepareP(SEM_MUTEX);
	semop(sem_id, &op[SEM_MUTEX], 1);
	tab_val_random[0] -= 1;
	c = adr_stack[tab_val_random[0]];
	printf("===>pop %d\n", tab_val_random[0]);
	prepareV(SEM_MUTEX);
	semop(sem_id, &op[SEM_MUTEX], 1);

	prepareV(SEM_LIBRE);
	semop(sem_id, &op[SEM_LIBRE], 1);
	return c;
}

void process_producteur() {
	adr_att = shmat(shm_id, NULL, 0600);
	PRODUCTEUR
	shmdt(adr_att);
}
void process_consommateur() {
	adr_att = shmat(shm_id, NULL, 0600);
	CONSOMMATEUR
	shmdt(adr_att);
}
void nfork(int nb_fils, int producteur) {
	printf("--pere %d cree %d %s\n", getpid(), nb_fils,
			producteur == 0 ? "consommateur" : "producteur");
	int i;
	for (i = 0; i < nb_fils; i++) {
		switch (fork()) {
		case 0: /* fils*/
			if (producteur) {
				printf("--producteur%i pid %d: \n", i, getpid());
				process_producteur();
			} else {
				printf("--consommateur%i pid %d : \n", i, getpid());
				process_consommateur();
			}
			exit(EXIT_SUCCESS);
			break;
		default:/* pere*/

			break;
		}
	}
}

void handler(int s) {
	printf("fin_main liberation des ressource\n");
	shmdt(adr_att);
	shmctl(shm_id, IPC_RMID, NULL);
	semctl(sem_id, 0, IPC_RMID, 0);
	exit(0);
}

int main(int argc, char* argv[]) {
	if (argc != 3) {
		printf("%s nbProcducteurs nbConsommateurs\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	int const nb_producteur = atoi(argv[1]);
	int const nb_consommateur = atoi(argv[2]);

	if ((shm_id = shmget(IPC_PRIVATE, SHM_TAILLE, IPC_CREAT | IPC_EXCL | 0600))
			== -1) {
		perror("shmget");
		exit(1);
	}
	if ((sem_id = semget(IPC_PRIVATE, SEM_TAILLE, IPC_CREAT | IPC_EXCL | 0600))
			== -1) {
		perror("semget");
		exit(1);
	}
	unsigned short arg[SEM_TAILLE];
	arg[SEM_MUTEX] = 1;
	arg[SEM_OCCUPE] = 0;
	arg[SEM_LIBRE] = SHM_TAILLE - sizeof(int);

	semctl(sem_id, SEM_TAILLE, SETALL, arg);

	adr_att = shmat(shm_id, NULL, 0600);
	tab_val_random = (int*) adr_att;
	tab_val_random[0] = 0; /* ps pointer stack*/

	printf(" libre = %d occupe = %d sem_mutex %d\n", semctl(sem_id, SEM_LIBRE, GETVAL, 0),
			semctl(sem_id, SEM_OCCUPE, GETVAL, 0),semctl(sem_id, SEM_MUTEX, GETVAL, 0));

	nfork(nb_producteur, 1);
	nfork(nb_consommateur, 0);

	signal(SIGINT, handler);
	pause();
	printf("sortie de pause\n");
	return EXIT_SUCCESS;
}
