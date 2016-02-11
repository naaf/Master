#define _SVID_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>

#define TYPE_MSG 1L

/** contenu message **/
struct message {
	long type;
	/** données ...**/
	int val_rand;
	int emetteur;
} msg;

/* exemple  multi_remonte_file_unique.c tme5 */
void message(char* file) {
	key_t cle;
	char code = 0;
	int msg_id;

	/*------------------------------------------------------*/
	/* creation msg	*/
	/*------------------------------------------------------*/

	code = (char) getpid();
	if (code == 0)
		code |= 1;
	if ((cle = ftok(file, code)) == -1) {
		perror("ftok");
		exit(EXIT_FAILURE);
	}
	/**cree ou retrouve id**/
	msg_id = msgget(cle, IPC_CREAT | 0600);

	/*------------------------------------------------------*/
	/* send msg	*/
	/*------------------------------------------------------*/
	msg.type = TYPE_MSG;
	msg.val_rand = 10;
	msg.emetteur = 21;

	msgsnd(msg_id, &msg, sizeof(msg) - sizeof(long), 0);

	/*------------------------------------------------------*/
	/* receive msg	*/
	/*------------------------------------------------------*/

	msgrcv(msg_id, &msg, sizeof(msg) - sizeof(long), TYPE_MSG, 0);

	/*------------------------------------------------------*/
	/* suppression, liberation msg	*/
	/*------------------------------------------------------*/
	msgctl(msg_id, IPC_RMID, NULL);
}

/* exemple remonte_partagee.c tme5*/
void memoire(char* file) {
	key_t cle;
	char code = 0;
	int shm_id;
	int taille;
	char *adr_att;
	/*------------------------------------------------------*/
	/* creation segment memoire	*/
	/*------------------------------------------------------*/
	code = (char) getpid();
	if (code == 0)
		code |= 1;
	if ((cle = ftok(file, code)) == -1) {
		perror("ftok");
		exit(EXIT_FAILURE);
	}
	if ((shm_id = shmget(cle, taille, IPC_CREAT | 0666)) < 0) {
		perror("shmget");
		exit(-1);
	}
	/*------------------------------------------------------*/
	/* attache segment memoire	*/
	/*------------------------------------------------------*/
	if ((adr_att = shmat(shm_id, NULL, 0600)) == (void *) -1) {
		perror("shmat");
		exit(-1);
	}
	/*------------------------------------------------------*/
	/* detache segment memoire	*/
	/*------------------------------------------------------*/
	shmdt(adr_att);

	/*------------------------------------------------------*/
	/* liberation segment memoire	*/
	/*------------------------------------------------------*/
	shmctl(shm_id, IPC_RMID, NULL);

}
//
//int main(int argc, char*  argv[]) {
//	printf("int *2  %ld  <==> %ld \n", sizeof(int) * 2,
//			sizeof(msg) - sizeof(long));
//
//	printf("msg %d %d ", msg.emetteur, msg.val_rand);
//	message(argv[0]);
//	printf("msg %d %d ", msg.emetteur, msg.val_rand);
//
//	return EXIT_SUCCESS;
//}
