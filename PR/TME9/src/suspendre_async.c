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

#define NB_FILS 256

int main(int argc, char *argv[]) {

	if (argc != 3) {
		printf("error syntaxe :%s contenant.txt contenu \n", argv[0]);
		exit(EXIT_FAILURE);
	}
	struct aiocb cb_ecr;
	struct aiocb cb_lec;
	const char* file = argv[1];
	char* contenu = argv[2];
	int desc_ecr;
	int desc_lec;
	struct aiocb * lio[1];

	char buffer[NB_FILS] = { 0 };

	if ((desc_ecr = open(file, O_CREAT | O_RDWR | O_TRUNC, 0666)) < 0) {
		perror("open");
		exit(EXIT_FAILURE);
	}

	/**  init l'ecriture **/
	cb_ecr.aio_fildes = desc_ecr;
	cb_ecr.aio_offset = 0;
	cb_ecr.aio_lio_opcode = LIO_WRITE;
	cb_ecr.aio_buf = contenu;
	cb_ecr.aio_nbytes = strlen(contenu);
	cb_ecr.aio_reqprio = 0;
	cb_ecr.aio_sigevent.sigev_notify = SIGEV_NONE;

	printf("contenu %s size : %ld\n", contenu, sizeof(char));
	/* Lancement de l'ecriture */
	if (aio_write(&cb_ecr) < 0) {
		perror("aio_write");
		exit(EXIT_FAILURE);
	}

	if ((desc_lec = open(file, O_RDONLY | 0666)) < 0) {
		perror("open desc_lec");
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

	/** init lecture*/
	cb_lec.aio_fildes = desc_lec;
	cb_lec.aio_offset = 0;
	cb_lec.aio_lio_opcode = LIO_READ;
	cb_lec.aio_buf = buffer;
	cb_lec.aio_nbytes = strlen(contenu);
	cb_lec.aio_reqprio = 0;
	cb_lec.aio_sigevent.sigev_notify = SIGEV_NONE;

	/* Lancement de lecture */
	if (aio_read(&cb_lec) < 0) {
		perror("aio_write");
		exit(EXIT_FAILURE);
	}

	/** attent fin lecture**/
	lio[0] = &cb_lec;
	nb_octets = 0;
	if (aio_suspend(lio, 1, NULL) == 0) {
		if (aio_error(lio[0]) == EINPROGRESS) {
			printf("operation encours");
		}

		/* Traitement d’erreur */
		if ((nb_octets = aio_return(lio[0])) != lio[0]->aio_nbytes) {
			printf("erreur aio_read");
		}

		/* Réussite */
		else {
			printf("lec 1 : %ld octets lus \n", nb_octets);
			printf("lu %s \n", (char*) lio[0]->aio_buf);
		}

	}

	close(desc_ecr);
	close(desc_lec);
	return EXIT_SUCCESS;
}
