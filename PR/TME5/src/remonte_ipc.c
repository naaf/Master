#define _SVID_SOURCE 1

#include<stdio.h>
#include<stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MSG_SIZE 128

int msg_id;
struct message {
	long type;
	int val_rand;
} msg;

void nfork(int nb_fils) {
	printf("pere %d \n", getpid());
	int i = 0, val_rand = 0;

	for (i = 0; i < nb_fils; i++) {
		switch (fork()) {
		case 0: /* fils*/
			srand(getpid());
			val_rand = (int) (10 * (float) rand() / RAND_MAX);
			msg.type = sizeof(int);
			msg.val_rand = val_rand;
			msgsnd(msg_id, &msg, sizeof(int), 0);
			printf("fils pid %d ==> %d\n", getpid(), val_rand);
			exit(EXIT_SUCCESS);
			break;
		default:/* pere*/

			break;
		}
	}

}
int main(int argc, char* argv[]) {
	int nb_fils, i;
	key_t cle;

	if (argc < 2) {
		printf("error syntaxe : %s nbProcessus \n", argv[0]);
		exit(1);
	}
	/*------------------------------------------------------*/
	/* creation msg	*/
	/*------------------------------------------------------*/
	char code = 0;
	code = getpid() & 255;
	if (code == 0)
		code |= 1;
	if ((cle = ftok(argv[0], code)) == -1) {
		perror("ftok");
		exit(-1);
	}
	msg_id = msgget(cle, IPC_CREAT | IPC_EXCL | 0600);

	/*------------------------------------------------------*/
	/* creation processus	*/
	/*------------------------------------------------------*/
	nb_fils = atoi(argv[1]);
	nfork(nb_fils);
	/*------------------------------------------------------*/
	/* traitement liberation msg	*/
	/*------------------------------------------------------*/
	/* attendre la fin de tous les fils*/
	for(i = 0 ; i < nb_fils; i++){
		wait(NULL);
	}
	int result = 0;
	for (i = 0; i < nb_fils; i++) {
		msgrcv(msg_id, &msg, sizeof(int), sizeof(int), 0);
		printf("pere msg%d recu ==> %d \n", i, msg.val_rand);
		result += msg.val_rand;
	}

	printf("pere %d : somme %d  \n", getpid(),result);
	msgctl(msg_id, IPC_RMID, NULL);

	return EXIT_SUCCESS;
}
