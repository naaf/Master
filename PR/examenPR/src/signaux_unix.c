#define _XOPEN_SOURCE 700
#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <errno.h>

#define VRAI 1
#define FAUX 0

void handler(int sig) {
	printf(" sig reçu %d %s\n", sig, strsignal(sig));
}

int processus_present(pid_t pid) {
	if (kill(pid, 0) == 0)
		return VRAI;
	if (errno == EPERM)
		return VRAI;
	return FAUX;
}

void t_sigpending() {
	sigset_t mask;
	sigset_t pendant;

	printf("pid %d \n", getpid());

	sigfillset(&mask);

	sigemptyset(&pendant);

	if (sigprocmask(SIG_SETMASK, &mask, NULL) == -1) {
		perror("sigprocmask");
	}

	while (VRAI) {
		sleep(1);

		/** obtenir la liste des signaux pendants*/
		if (sigpending(&pendant) == -1) {
			perror("sigpending");
		}

		if (sigismember(&pendant, SIGINT)) {
			printf("le sig SIGINT est pendant \n");
		}

	}
}

void t_sigaction() {
	sigset_t mask;
	sigset_t mask_suspend;
	sigset_t sig_wait;
	struct sigaction action;

	printf("pid %d \n", getpid());

	/** masquer les signals  **/
	sigfillset(&mask);
	sigdelset(&mask, SIGINT);
	if (sigprocmask(SIG_SETMASK, &mask, NULL) == -1) {
		perror("sigprocmask");
	}

	/** lier action avec signal  **/
	action.sa_flags = 0;
	sigemptyset(&action.sa_mask);
	action.sa_handler = handler;

	if (sigaction(SIGINT, &action, NULL) < 0) {
		perror("sigaction SIGINT:  \n");
	}
	if (sigaction(SIGUSR1, &action, NULL) < 0) {
		perror("sigaction SIGUSR1:  \n");
	}

	/** sigsuspend */
	sigfillset(&mask_suspend);
	sigdelset(&mask_suspend, SIGINT);
	sigdelset(&mask_suspend, SIGUSR1);
	sigsuspend(&mask_suspend);

	/** sigwait **/
//	int sig = 0;
//	sigemptyset(&sig_wait);
//	sigaddset(&sig_wait, SIGINT);
//	if (sigwait(&sig_wait, &sig) != 0) {
//		perror("sigaction SIGINT: ");
//	}
//	printf("retourne sig %d \n", sig);
}

//int main(void) {
//	t_sigaction();
//
//	return EXIT_SUCCESS;
//}
