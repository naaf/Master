#define _SVID_SOURCE 1

#define _XOPEN_SOURCE 700
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <aio.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

/*
 est-il nécessaire de synchroniser le père avec ses fils pour que les lectures se fassent correctement ? NON
 est-il nécessaire de synchroniser les fils entre eux pour que les écritures se fassent correctement ? OUI
 */

#define NB_FILS 4

#define FILE "remonte_async.txt"

void process(int rang, int desc_ecr) {
	struct aiocb cb_ecr;
	struct aiocb * lio[1];
	int val_rand = 0;

	srand(getpid());
	val_rand = (int) (10 * (float) rand() / RAND_MAX);
	printf("fils pid %d ==> %d\n", getpid(), val_rand);

	/**  init l'ecriture **/
	cb_ecr.aio_fildes = desc_ecr;
	cb_ecr.aio_offset = rang;
	cb_ecr.aio_lio_opcode = LIO_WRITE;
	cb_ecr.aio_buf = &val_rand;
	cb_ecr.aio_nbytes = 1;
	cb_ecr.aio_reqprio = 0;
	cb_ecr.aio_sigevent.sigev_notify = SIGEV_NONE;

	/* write . */
	if (aio_write(&cb_ecr) < 0) {
		perror("aio_write");
		exit(EXIT_FAILURE);
	}

	/** attent fin ecriture**/
	ssize_t nb_octets = 0;
	lio[0] = &cb_ecr;
	if (aio_suspend(lio, 1, NULL) == 0) {
		if (aio_error(lio[0]) == EINPROGRESS) {
			printf("operation encours\n");
		}

		/* Traitement d’erreur */
		if ((nb_octets = aio_return(lio[0])) != lio[0]->aio_nbytes) {
			printf("erreur aio_write\n");
		}

		/* Réussite */
		else {
			printf("Ecr 1 : %ld octets ecrit \n", nb_octets);
		}

	}

}

void nfork(int nb_fils, int desc_ecr) {
	int i = 0;
	for (i = 0; i < nb_fils; i++) {
		switch (fork()) {
		case 0: /* fils*/
			process(i, desc_ecr);
			exit(EXIT_SUCCESS);
			break;
		default:/* pere*/

			break;
		}
	}
}

int main(int argc, char* argv[]) {

	if (argc != 2) {
		printf("error syntaxe : %s  4 \n", argv[0]);
		exit(1);
	}
	struct aiocb cb_lec[NB_FILS];
	struct aiocb * lio[NB_FILS];
	struct sigevent lio_sigev;

	int nb_fils = atoi(argv[1]);
	int result = 0;
	int desc = 0;
	int buffer[NB_FILS] = { 0 };

	if ((desc = open(FILE, O_CREAT | O_RDWR | O_TRUNC, 0666)) <= 0) {
		perror("open");
		exit(EXIT_FAILURE);
	}

	/* fils */
	nfork(nb_fils, desc);

	int i;
	for (i = 0; i < nb_fils; i++) {
		wait(NULL);
	}

	for (i = 0; i < nb_fils; i++) {
		/**  init lecture **/
		cb_lec[i].aio_fildes = desc;
		cb_lec[i].aio_lio_opcode = LIO_READ;
		cb_lec[i].aio_nbytes = 1;
		cb_lec[i].aio_reqprio = 0;
		cb_lec[i].aio_sigevent.sigev_notify = SIGEV_NONE;
		cb_lec[i].aio_buf = &buffer[i];
		cb_lec[i].aio_offset = i;

		/**  affectation a l'ensemble des lectures **/
		lio[i] = &cb_lec[i];
	}
	lio_sigev.sigev_notify = SIGEV_NONE;
	if (lio_listio(LIO_WAIT, lio, nb_fils, &lio_sigev) < 0) {
		perror("lio_listio");
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < nb_fils; i++) {
		// read val fils
		result += buffer[i];
	}
	printf("pere %d : somme %d  \n", getpid(), result);
	close(desc);
	return EXIT_SUCCESS;
}
