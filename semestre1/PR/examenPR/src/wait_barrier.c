#define _SVID_SOURCE 1

#include<stdio.h>
#include<stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <signal.h>

int sem_id;
struct sembuf op[1];

void preparePS0() {
	op[0].sem_num = 0;
	op[0].sem_op = -1;
	op[0].sem_flg = 0;
}

void prepareVS0() {
	op[0].sem_num = 0;
	op[0].sem_op = 1;
	op[0].sem_flg = 0;
}

void prepareZS0() {
	op[0].sem_num = 0;
	op[0].sem_op = 0;
	op[0].sem_flg = 0;
}

void wait_barrier( NB_PCS) {
	preparePS0();
	semop(sem_id, &op[0], 1);
	/* process est bloc tant que la sem nest pas null*/
	prepareZS0();
	semop(sem_id, &op[0], 1);

}

void static process(int NB_PCS) {
	printf("avant barriere fils %d \n", getpid());
	wait_barrier(NB_PCS);
	printf("apres barriere fils %d \n", getpid());
	exit(0);
}

void static nfork(int nb_fils) {
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

//int main(int argc, char* argv[]) {
//	if (argc < 2) {
//		printf("%s nbProcessus\n", argv[0]);
//		exit(EXIT_FAILURE);
//	}
//	int const nb_fils = atoi(argv[1]);
//
//	key_t cle;
//	char code = 0;
//
//	/*------------------------------------------------------*/
//	/* creation semaphore	*/
//	/*------------------------------------------------------*/
//	code = (char) getpid();
//	if (code == 0) {
//		exit(1);
//	}
//	if ((cle = ftok(argv[0], code)) == -1) {
//		perror("ftok");
//		exit(-1);
//	}
//	if ((sem_id = semget(cle, 1, IPC_CREAT | IPC_EXCL | 0600)) == -1) {
//		perror("semget");
//		exit(1);
//	}
//
//	/*initialisation de la semaphore*/
//	semctl(sem_id, 0, SETVAL, nb_fils);
//
//	/*
//	 Si l’ensemble est constitué de deux sémaphores, l’appel :
//	 unsigned short arg[2];
//	 arg[0] = 3;
//	 arg[1] = 1;
//	 semctl ( id, 2, SETALL, arg);
//	 */
//
//	nfork(nb_fils);
//
//	int i;
//	for (i = 0; i < nb_fils; i++) {
//		wait(NULL);
//	}
//	/* liberation  de la semaphore */
//	semctl(sem_id, 0, IPC_RMID, 0);
//
//	return EXIT_SUCCESS;
//}
