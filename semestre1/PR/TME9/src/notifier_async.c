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

#define SIGNAL_ECR (SIGRTMIN + 3)

#define BUF_SIZE 256

/** handler sig rt**/
void gest_ecr(int signum, siginfo_t * info, void * vide) {
	struct aiocb * cb;
	ssize_t nb_octets;
	if (info->si_code == SI_ASYNCIO) {
		cb = info->si_value.sival_ptr;
		if (aio_error(cb) == EINPROGRESS)
			return;
		nb_octets = aio_return(cb);
		printf("Ecr 1 : %ld octets ecrit \n", nb_octets);
	}
}

int main(int argc, char *argv[]) {

	if (argc != 3) {
		printf("error syntaxe :%s contenant.txt contenu \n", argv[0]);
		exit(EXIT_FAILURE);
	}
	const char* file = argv[1];
	char* contenu = argv[2];

	struct aiocb cb_ecr;
	struct aiocb cb_lec;
	int desc_ecr;
	int desc_lec;

	char buffer[BUF_SIZE] = { 0 };
	struct sigaction action_ecr;
	sigset_t mask;

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

	cb_ecr.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
	cb_ecr.aio_sigevent.sigev_signo = SIGNAL_ECR;
	cb_ecr.aio_sigevent.sigev_value.sival_ptr = &cb_ecr;

	/** lier gest_ecr avec action_ecr et action_lec**/

	action_ecr.sa_flags = SA_SIGINFO;
	action_ecr.sa_sigaction = gest_ecr;
	if (sigaction(SIGNAL_ECR, &action_ecr, NULL) < 0) {
		perror("sigaction");
		exit(EXIT_FAILURE);
	}

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

	sigemptyset(&mask);
	sigsuspend(&mask);
	printf("fin ecriture \n");

	/** init lecture*/
	cb_lec.aio_fildes = desc_lec;
	cb_lec.aio_offset = 0;
	cb_lec.aio_lio_opcode = LIO_READ;
	cb_lec.aio_buf = buffer;

	cb_lec.aio_nbytes = sizeof(buffer);
	cb_lec.aio_reqprio = 0;

	cb_lec.aio_sigevent.sigev_notify = SIGEV_NONE;

	/* Lancement de lecture */
	if (aio_read(&cb_lec) < 0) {
		perror("aio_read");
		exit(EXIT_FAILURE);
	}

	/** attente fin lecture suspend ***/

	struct aiocb * lio[1];
	ssize_t nb_octets = 0;
	lio[0] = &cb_lec;
	if (aio_suspend(lio, 1, NULL) == 0) {
		if (aio_error(lio[0]) == EINPROGRESS) {
			printf("operation encours\n");
			return 0;
		}
		nb_octets = aio_return(lio[0]);
		printf("Lus 1 : %ld octets ecrit \n", nb_octets);
		char *s = (char *) lio[0]->aio_buf;
		printf("lu %s \n", s);

	}

	close(desc_ecr);
	close(desc_lec);
	return EXIT_SUCCESS;
}
