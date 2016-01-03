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
#include <time.h>

#define SIGNAL_ECR (SIGRTMIN + 5)
#define BUF_SIZE 256
static const int VAL_TV_NSEC = 50;

char* file;
int boucle;

void gest_ecr(int signum, siginfo_t * info, void * vide) {
	struct aiocb * cb;
	ssize_t nb_octets;
	int desc_lec;
	cb = info->si_value.sival_ptr;
	if (aio_error(cb) == EINPROGRESS) {
		printf("l'opération est en cours ... \n");
		return;
	}

	if ((nb_octets = aio_return(cb)) != cb->aio_nbytes) {
		/* Traitement d’erreur */
		printf("Erreur de nb_octets ... \n");
	}
	/* Réussite */
	printf("Ecr 1 : %ld octets ecrit \n", nb_octets);

	/** partie lecture**/
	if ((desc_lec = open(file, O_RDONLY | 0666)) < 0) {
		perror("open desc_lec");
		exit(EXIT_FAILURE);
	}

	char buffer[BUF_SIZE] = { 0 };
	read(desc_lec, buffer, nb_octets);
	printf("lus %s \n", buffer);

	close(desc_lec);

	boucle = 0;

}

int main(int argc, char *argv[]) {

	if (argc != 3) {
		printf("error syntaxe :%s contenant.txt contenu \n", argv[0]);
		exit(EXIT_FAILURE);
	}

	struct aiocb cb_ecr;
	struct aiocb cb_lec;
	file = argv[1];
	char* contenu = argv[2];
	int desc_ecr;
	int desc_lec;

	struct sigaction action_ecr;
	struct sigaction action_lec;
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

	cb_ecr.aio_sigevent.sigev_notify = SIGEV_NONE;


	/** lier gest_ecr avec action_ecr et action_lec**/

	action_ecr.sa_flags = SA_SIGINFO;
	action_ecr.sa_sigaction = gest_ecr;
	sigemptyset(&action_ecr.sa_mask);
	if (sigaction(SIGNAL_ECR, &action_ecr, NULL) < 0) {
		perror("sigaction SIGNAL_ECR");
		exit(EXIT_FAILURE);
	}

	/* Lancement de l'ecriture */
	if (aio_write(&cb_ecr) < 0) {
		perror("aio_write");
		exit(EXIT_FAILURE);
	}

	/*** timer ***/
	timer_t timerid;
	struct sigevent sev;
	struct itimerspec its;

	sev.sigev_notify = SIGEV_SIGNAL;
	sev.sigev_signo = SIGNAL_ECR;
	sev.sigev_value.sival_ptr = &cb_ecr;

	/***creation temporisateur ***/
	if (timer_create(CLOCK_REALTIME, &sev, &timerid) == -1)
		perror("timer_create");

	/*** init temporisateur ***/
	its.it_value.tv_sec = 0;
	its.it_value.tv_nsec = VAL_TV_NSEC;
	its.it_interval.tv_sec = its.it_value.tv_sec;
	its.it_interval.tv_nsec = its.it_value.tv_nsec;

	/***Armement de temporisateur ***/
	if (timer_settime(timerid, 0, &its, NULL) == -1)
		perror("timer_settime");

	sigemptyset(&mask);
	boucle = 1;
	do {
		printf("timer... \n");
		sigsuspend(&mask);
	} while (boucle);

	close(desc_ecr);
	close(desc_lec);
	return EXIT_SUCCESS;
}
