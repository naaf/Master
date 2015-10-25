#define _SVID_SOURCE 1

#include<stdio.h>
#include<stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MSG_SIZE 1024

typedef struct message {
	long type;
	int val_rand;
	int emetteur;
} msg;

msg *msgs;
int msg_id;
int nb_fils;

void remonte_ipc() {

	int indice_fils;
	int i, val_rand = 0, result = 0;
	for (indice_fils = 0; indice_fils < nb_fils; indice_fils++) {
		switch (fork()) {
		case 0: /* fils*/
			srand(getpid());
			val_rand = (int) (10 * (float) rand() / RAND_MAX);
			msgs[nb_fils].type = 1L;
			msgs[nb_fils].val_rand = val_rand;
			msgs[nb_fils].emetteur = indice_fils;
			msgsnd(msg_id, &msgs[nb_fils], 8, 0);
			printf("fils%d pid %d ==> %d\n", indice_fils, getpid(), val_rand);

			for (i = 0; i < val_rand; i++) {
				msgrcv(msg_id, &msgs[indice_fils], sizeof(int) * 2, sizeof(int),
						0);
				printf("fils msg%d recu %d \n", indice_fils,
						msgs[indice_fils].val_rand);
				result += msgs[indice_fils].val_rand;
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
	char code = 0;
	code = getpid() & 255;
	if (code == 0)
		code |= 1;
	if ((cle = ftok(argv[0], code)) == -1) {
		perror("ftok");
		exit(-1);
	}
	msg_id = msgget(cle, IPC_CREAT | 0600);

	msgs = calloc(nb_fils + 1, sizeof(msg));

	/*------------------------------------------------------*/
	/* creation processus	*/
	/*------------------------------------------------------*/
	remonte_ipc();
	/*------------------------------------------------------*/
	/* traitement liberation msg	*/
	/*------------------------------------------------------*/
	int j = 0;
	int index = 0;
	printf("pere %d, nb_fils %d \n", getpid(), nb_fils);
	for (i = 0; i < nb_fils; i++) {
		printf("boucle%d \n", i);
		msgrcv(msg_id, &msgs[nb_fils], 8, 1L, 0);
		index = msgs[nb_fils].emetteur;
		printf("pere recu :  msgs%d ==> %d \n", index, msgs[nb_fils].val_rand);

		for (j = 0; j < msgs[nb_fils].val_rand; j++) {
			val_rand = (int) (10 * (float) rand() / RAND_MAX);
			printf("%d, ", val_rand);
			msgs[index].type = sizeof(int);
			msgs[index].val_rand = val_rand;
			msgs[index].emetteur = nb_fils;
			msgsnd(msg_id, &msgs[index], sizeof(int) * 2, 0);
		}

		printf("\n");
	}

	printf("pere %d : END\n", getpid());
	free(msgs);
	msgctl(msg_id, IPC_RMID, NULL);

	return EXIT_SUCCESS;
}
