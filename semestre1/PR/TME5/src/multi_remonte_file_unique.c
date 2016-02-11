#define _SVID_SOURCE 1

#include<stdio.h>
#include<stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MSG_SIZE 1024

/** contenu message **/
typedef struct message {
	long type;
	int val_rand;
	int emetteur;
} msg;

msg file_principal;

int msg_id;
int nb_fils;
int TYPE_MAIN;

void process(int val_rand, int indice_fils) {

}

void nfork() {

	int indice_fils;
	int i, val_rand = 0, result = 0;
	for (indice_fils = 1; indice_fils <= nb_fils; indice_fils++) {
		switch (fork()) {
		case 0: /* fils*/
			srand(getpid());
			val_rand = (int) (10 * (float) rand() / RAND_MAX);
			file_principal.type = TYPE_MAIN;
			file_principal.val_rand = val_rand;
			file_principal.emetteur = indice_fils;
			msgsnd(msg_id, &file_principal, sizeof(int) * 2, 0);
			printf("fils%d pid %d ==> %d\n", indice_fils, getpid(), val_rand);

			for (i = 0; i < val_rand; i++) {
				msgrcv(msg_id, &file_principal, sizeof(int) * 2, indice_fils,
						0);
				printf("fils msg%d recu %d \n", indice_fils,
						file_principal.val_rand);
				result += file_principal.val_rand;
			}

			printf("fils %d : la somme est %d \n", indice_fils, result);

			exit(EXIT_SUCCESS);
			break;
		default:/* pere*/

			break;
		}
	}
}
int main(int argc, char* argv[]) {
	int i, val_rand;
	key_t cle;

	if (argc < 2) {
		printf("error syntaxe : %s nbProcessus \n", argv[0]);
		exit(1);
	}
	/*------------------------------------------------------*/
	/* creation msg	*/
	/*------------------------------------------------------*/
	nb_fils = atoi(argv[1]);
	TYPE_MAIN = nb_fils + 1;
	char code = 0;
	code = getpid() & 255;
	if (code == 0)
		code |= 1;
	if ((cle = ftok(argv[0], code)) == -1) {
		perror("ftok");
		exit(-1);
	}
	msg_id = msgget(cle, IPC_CREAT | 0600);

	/*------------------------------------------------------*/
	/* creation processus	*/
	/*------------------------------------------------------*/
	nfork();

	/*------------------------------------------------------*/
	/* traitement liberation msg	*/
	/*------------------------------------------------------*/
	int j = 0;
	int index = 0;
	printf("pere %d, nb_fils %d \n", getpid(), nb_fils);
	for (i = 0; i < nb_fils; i++) {
		msgrcv(msg_id, &file_principal, sizeof(int) * 2, TYPE_MAIN, 0);
		index = file_principal.emetteur;
		printf("pere recu :  msgs%d ==> %d \n", index, file_principal.val_rand);

		for (j = 0; j < file_principal.val_rand; j++) {
			val_rand = (int) (10 * (float) rand() / RAND_MAX);
			file_principal.type = index;
			file_principal.val_rand = val_rand;
			file_principal.emetteur = TYPE_MAIN;
			msgsnd(msg_id, &file_principal, sizeof(int) * 2, 0);
		}

	}

	/*------------------------------------------------------*/
	/* liberation msg	*/
	/*------------------------------------------------------*/
	msgctl(msg_id, IPC_RMID, NULL);

	return EXIT_SUCCESS;
}
