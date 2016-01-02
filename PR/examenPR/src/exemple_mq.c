#define _XOPEN_SOURCE 700

#include<stdio.h>
#include<stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <mqueue.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>

int nb_fils = 4;
const char* mqname = "/messageMQ2";

void static nfork() {

	int indice_fils;
	int i, val_rand = 0;
	char buf[10] = { 0 };
	unsigned int prio = 0;
	mqd_t mqdes;
	for (indice_fils = 1; indice_fils <= nb_fils; indice_fils++) {
		switch (fork()) {
		case 0: /* fils*/
			srand(getpid());
			val_rand = (int) (10 * (float) rand() / RAND_MAX);
			printf("fils%d pid %d ==> %d\n", indice_fils, getpid(), val_rand);
			sprintf(buf, "%d", val_rand);
			/**retrouve mqdes **/
			if ((mqdes = mq_open(mqname, O_RDWR | O_CREAT, 0666, NULL)) == -1) {
				perror("mq_open");
				exit(EXIT_FAILURE);
			}

			if (mq_send(mqdes, buf, sizeof(buf), prio) == -1) {
				perror("mq_send");
			}

			exit(EXIT_SUCCESS);
			break;
		default:/* pere*/

			break;
		}
	}
}

void mq() {
	mqd_t mqdes;
	char buf[100] = { 0 };

	/** attribut init **/
	struct mq_attr attr;
	attr.mq_flags = 0;
	attr.mq_maxmsg = 10;
	attr.mq_msgsize = 14;
	attr.mq_curmsgs = 0;

	/** creation ou ouverture de mq **/
	if ((mqdes = mq_open(mqname, O_RDWR | O_CREAT, 0666, &attr)) == -1) {
		perror("mq_open");
		exit(EXIT_FAILURE);
	}

	nfork(4);

	int i;
	for (i = 0; i < nb_fils; i++) {
		/** receive  **/
		if (mq_receive(mqdes, buf, sizeof(buf), NULL) == -1) {
			perror("mq_receive");
		}
		printf("pere recoit %s \n", buf);
	}

	for (i = 0; i < nb_fils; i++) {
		wait(NULL);
	}

	/** fermeture et liberation **/
	mq_close(mqdes);
	mq_unlink(mqname);
}

/** exemple caht_client.c tme6**/
part_mem() {

	char* p_shm;
	struct message* request;
	int shm_id;
	int taille_location;
	char* shm_nom = "/exemple";

	/** creation exclusif  **/
	if ((shm_id = shm_open(shm_nom, O_CREAT | O_EXCL | O_RDWR, 0666)) == -1) {
		perror("shm_open");
		exit(-1);
	}

	/** alloucation **/
	if (ftruncate(shm_id, taille_location) == -1) {
		perror("ftruncate");
		exit(-1);
	}

	/** attachement **/
	if ((p_shm = mmap(NULL, taille_location, PROT_READ | PROT_WRITE,
	MAP_SHARED, shm_id, 0)) == MAP_FAILED) {
		perror("mmap");
		exit(EXIT_FAILURE);
	}
	/** detachement **/
	if (munmap(p_shm, taille_location) == -1) {
		perror("munmap");
	}

	/** fermeture et liberation **/
	if (shm_unlink(shm_nom) == -1) {
		perror("shm_unlink");
		exit(EXIT_FAILURE);
	}

}

//int main(int argc, char** argv) {
//
//	return EXIT_SUCCESS;
//}
