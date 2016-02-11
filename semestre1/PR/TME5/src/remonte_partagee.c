#define _SVID_SOURCE 1
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

char *adr_att;
int *tab_val_random;
int shm_id;

void remonte_partagee(int nb_fils) {

	int indice_fils = 1, val_rand = 0;
	printf("pere %d nb_fils %d \n", getpid(), nb_fils);
	for (indice_fils = 1; indice_fils <= nb_fils; indice_fils++) {
		switch (fork()) {
		case 0:
			/* fils*/

			srand(getpid());
			val_rand = (int) (10 * (float) rand() / RAND_MAX);

			if ((adr_att = shmat(shm_id, 0, 0600)) == (void *) -1) {
				perror("shmat");
				exit(-1);
			}
			tab_val_random = (int*) adr_att;
			tab_val_random[indice_fils] = val_rand;
			printf("fils %d, pid %d ==> %d\n", indice_fils, getpid(), val_rand);

			if (indice_fils == nb_fils) {
				tab_val_random[0] = -1;
			}
			shmdt(adr_att);
			exit(EXIT_SUCCESS);
			break;
		default:/* pere*/
			break;
		}
	}
}
int main(int argc, char* argv[]) {

	key_t cle;

	int taille = 0;

	int nb_fils, i;

	if (argc != 2) {
		printf("%derror syntaxe : %s nbProcessus \n", argc, argv[0]);
		exit(1);
	}
	nb_fils = atoi(argv[1]);

	/*------------------------------------------------------*/
	/* creation segment memoir	*/
	/*------------------------------------------------------*/

	taille = (nb_fils + 1) * sizeof(int);

	char code = 0;

	code = getpid() & 255;
	if (code == 0) {
		printf("error code = 0");
		exit(-1);
	}
	if ((cle = ftok(argv[0], code)) == -1) {
		perror("ftok");
		exit(-1);
	}
	if ((shm_id = shmget(cle, taille, IPC_CREAT | 0666)) < 0) {
		perror("shmget");
		exit(-1);
	}

	if ((adr_att = shmat(shm_id, NULL, 0600)) == (void *) -1) {
		perror("shmat");
		exit(-1);
	}
	tab_val_random = (int*) adr_att;
	tab_val_random[0] = 0;

	/*------------------------------------------------------*/
	/* traitement liberation memoir partage	*/
	/*------------------------------------------------------*/
	remonte_partagee(nb_fils);
	while (tab_val_random[0] == 0) {
		printf("attend while %d \n", tab_val_random[0]);
		sleep(1);
	}

	int result = 0;
	for (i = 1; i < nb_fils + 1; i++) {

		printf("shm%d recu %d \n", i, tab_val_random[i]);
		result += tab_val_random[i];
	}
	printf("pere %d : la somme est %d \n", getpid(), result);
	shmdt(adr_att);
	shmctl(shm_id, IPC_RMID, NULL);

	return EXIT_SUCCESS;
}
