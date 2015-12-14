/*

 est-il possible de résoudre cet exercice sans utiliser l’un des processus comme coordinateur ? sans semaphore et avec que des signaux
 ne n'est pas possible

 */
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

#define SIGNAL_BARRIERE (SIGRTMIN + 7)
#define NB_FILS 4

int nb_fils;
int nb_fils_wait = 1;
int tab_pid[NB_FILS] = { 0 };

void gest_barriere(int signum, siginfo_t * info, void * vide) {
	tab_pid[nb_fils_wait] = info->si_value.sival_int;
	nb_fils_wait++;
}

void wait_barrier(int rang) {
	union sigval valeur;
	sigset_t mask;
	sigemptyset(&mask);

	if (rang > 0) {
		valeur.sival_int = getpid();
		sigqueue(tab_pid[0], SIGNAL_BARRIERE, valeur);
		sigsuspend(&mask);
	} else {
		while (nb_fils_wait < nb_fils) {
			sigsuspend(&mask);
		}

		int i;
		for (i = 1; i < nb_fils; i++) {
			printf("reveillez %d proc endormis \n", tab_pid[i]);
			sigqueue(tab_pid[i], SIGNAL_BARRIERE, valeur);
		}
	}
}

void process(int rang) {

	printf("avant barriere fils %d \n", getpid());
	wait_barrier(rang);
	printf("apres barriere fils %d \n", getpid());
}

void nfork(int nb_fils) {
	int i = 0;
	for (i = 0; i < nb_fils; i++) {
		switch (tab_pid[i] = fork()) {
		case 0: /* fils*/
			process(i);
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

	struct sigaction action;

	action.sa_sigaction = gest_barriere;
	action.sa_flags = SA_SIGINFO;
	sigemptyset(&action.sa_mask);
	if (sigaction(SIGNAL_BARRIERE, &action, NULL) < 0)
		fprintf(stderr, "SIGNAL_BARRIERE non intercepté \n");

	nb_fils = atoi(argv[1]);
	/* fils */
	nfork(nb_fils);

	int i;
	for (i = 0; i < nb_fils; i++) {
		wait(NULL);
	}

	return EXIT_SUCCESS;
}
