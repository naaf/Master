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

#define BUF_SIZE 4

#define SIGNAL_PRINT (SIGRTMIN + 4)

int tab_pid[BUF_SIZE] = { 0 };
int nb_fils;

void gest(int signum, siginfo_t * info, void * vide) {
	printf("sig recu du %d\n",info->si_pid);
}

void process(int rang) {
	sigset_t mask;
	sigemptyset(&mask);

	/** vars sig rt**/
	union sigval valeur;
	struct sigaction action;

	/** attache handler au sig**/
	action.sa_sigaction = gest;
	action.sa_flags = SA_SIGINFO;
	sigemptyset(&action.sa_mask);
	if (sigaction(SIGNAL_PRINT, &action, NULL) < 0)
		fprintf(stderr, "SIGNAL_PRINT non intercepté \n");

	/** TRAITEMENT **/
	if (rang != nb_fils - 1){
		sigsuspend(&mask);
	}

	printf("mon rang : %d  pid : %d\n", rang, getpid());


	/** envois sig rt**/
	valeur.sival_int = 0;
	if (rang != 0) {
		printf("reveil du rang %d\n", rang - 1);
		sigqueue(tab_pid[rang - 1], SIGNAL_PRINT, valeur);
	}
	printf("******************\n");
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
	nb_fils = atoi(argv[1]);

	/* fils */
	nfork(nb_fils);

	int i;
	for (i = 0; i < nb_fils; i++) {
		wait(NULL);
	}
	for (i = 0; i < nb_fils; i++) {
		printf("rang : %d  pid : %d, ", i, tab_pid[i]);
	}
	printf(" main \n");

	return EXIT_SUCCESS;
}
