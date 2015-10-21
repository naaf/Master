#define SVID_SOURCE 1

#include<stdio.h>
#include<stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MSG_SIZE 128

int main(int argc, char* argv[]) {
	int nb_fils, i, val_rand;
	int msg_id;
	key_t cle;
	struct message {
		long type;
		int val_rand;
	} msg;
	if (argc < 2) {
		printf("error syntaxe : %s nbProcessus \n", argv[0]);
		exit(1);
	}
	/*------------------------------------------------------*/
	/* creation msg	*/
	/*------------------------------------------------------*/
	char code=0;
	code = getpid()& 255;
	if(code == 0  ) code |= 1;
	if((cle = ftok(argv[0], code)) ==-1){
		perror("ftok");
		exit(-1);
	}
	msg_id = msgget(cle, IPC_CREAT| IPC_CREAT|0600);

	/*------------------------------------------------------*/
	/* creation processus	*/
	/*------------------------------------------------------*/
	nb_fils = atoi(argv[1]);
	printf("pere %d \n", getpid());
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

	/*------------------------------------------------------*/
	/* traitement liberation msg	*/
	/*------------------------------------------------------*/
	int result = 0;
	for (i = 0; i < nb_fils; i++) {
		msgrcv(msg_id, &msg, sizeof(int), sizeof(int), 0);
		printf("msg%d recu %d \n", i, msg.val_rand);
		result += msg.val_rand;
	}
	printf("pere %d : la somme est %d \n",getpid(), result);
	msgctl(msg_id, IPC_RMID, NULL);

	return EXIT_SUCCESS;
}