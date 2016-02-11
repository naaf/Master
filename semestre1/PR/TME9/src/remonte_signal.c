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
#define SIGNAL_BARRIERE (SIGRTMIN + 8)
int result = 0;
int pid_pere;

void gest_barriere(int signum, siginfo_t * info, void * vide) {
	result += info->si_value.sival_int;
	printf("result %d \n", result);
}

void process(int rang) {
	int val_rand = 0;
	union sigval valeur;
	srand(getpid());
	val_rand = (int) (10 * (float) rand() / RAND_MAX);
	printf("fils pid %d ==> %d\n", getpid(), val_rand);

	valeur.sival_int = val_rand;
	sigqueue(pid_pere, SIGNAL_BARRIERE, valeur);
}

void nfork(int nb_fils) {
	int i = 0;
	for (i = 0; i < nb_fils; i++) {
		switch (fork()) {
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
		fprintf(stderr, "SIGNAL_REMONTE non intercepté \n");

	int nb_fils = atoi(argv[1]);
	pid_pere = getpid();
	/* fils */
	nfork(nb_fils);

	int i;
	for (i = 0; i < nb_fils; i++) {
		wait(NULL);
	}

	printf("pere %d : somme %d  \n", getpid(), result);
	return EXIT_SUCCESS;
}
