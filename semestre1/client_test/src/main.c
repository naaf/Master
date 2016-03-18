/*
 * main.c
 *
 *  Created on: 8 mars 2016
 *      Author: naaf
 */

#define _XOPEN_SOURCE 700
#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#include "../headers/ricochet_robot.h"
#include "../headers/communication.h"
#include "../headers/ihm.h"

#define SIG_COM (SIGRTMIN + 3)
#define SIG_IHM (SIGRTMIN + 4)

pthread_t thread_com, thread_ctrl;
int sc, pid_ihm;

void gest_ihm(int signum, siginfo_t * info, void * vide) {
	printf("gest SIG_IHM %d\n", signum);
}

void *run_com(void *arg) {
	int sc;
	char response[1024], *env;
	union sigval valeur;

	memset(response, 0, 1024);
	sc = *((int*) arg);

	printf("thread com demarre :\n");
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	while (1) {
		read_response(sc, response);
		printf("com: recu reponse len %d : %s\n", strlen(response), response);

		env = malloc(strlen(response) + 1);
		strcpy(env, response);
		valeur.sival_ptr = env;
		sigqueue(getpid(), SIG_COM, valeur);

		memset(response, 0, strlen(response) + 1);
	}

	return NULL;
}

void *run_ctrl(void *arg) {
	printf("thread ctrl demarre %d, %d:\n", getpid(), SIG_COM);
	sigset_t sig_wait;
	siginfo_t info;
	char **tab = NULL, *response;
	int size, true;
	union sigval valeur;

	true = 1;

	while (true) {
		sigemptyset(&sig_wait);
		sigaddset(&sig_wait, SIG_COM);
		if (sigwaitinfo(&sig_wait, &info) == -1) {
			perror("sigwaitinfo SIG_COM: ");
		}

		response = info.si_value.sival_ptr;
		printf("ctrl recu %s: ", response);

		tab = string_to_arraystring(response, &size, '/');

		if (!strcmp(SORT, tab[0])) {
			printf("SORT OK\n");
			true = 0;
			pthread_cancel(thread_com);
			sigqueue(pid_ihm, SIGINT, valeur);
		}
		if (!strcmp(TOUR, tab[0])) {
			sigqueue(pid_ihm, SIG_IHM, valeur);
		}

		/*free resources*/
		free_table(tab, size);
		free(response);
	}

	return NULL;
}

int main(int argc, char** argv) {

//	sigset_t mask;
//	struct sigaction action;
//
//	action.sa_sigaction = gest_ihm;
//	action.sa_flags = SA_SIGINFO | SA_RESTART;
//
//	sigfillset(&mask);
//	if (sigprocmask(SIG_SETMASK, &mask, NULL) == -1) {
//		perror("sigprocmask");
//	}
//
//	sigemptyset(&action.sa_mask);
//	if (sigaction(SIG_IHM, &action, NULL) < 0)
//		fprintf(stderr, "SIG_IHM non intercepté \n");
//
//	sc = connex_socket("127.0.0.1", 2016);
//	pid_ihm = fork();
//
//	if (pid_ihm == 0) { // fils proc IHM
//
//		sigdelset(&mask, SIG_IHM);
//		sigdelset(&mask, SIGINT);
//		if (sigprocmask(SIG_SETMASK, &mask, NULL) == -1) {
//			perror("sigprocmask");
//		}
//
//		ihm();
//	} else {
//		printf("pid IHM %d \n", pid_ihm);
//	}
//
//	if (pthread_create(&thread_com, NULL, run_com, &sc) != 0) {
//		perror("pthread_create \n");
//		exit(1);
//	}
//	if (pthread_create(&thread_ctrl, NULL, run_ctrl, NULL) != 0) {
//		perror("pthread_create \n");
//		exit(1);
//	}
//
//	pthread_join(thread_com, NULL);
//	pthread_join(thread_ctrl, NULL);
//
//	wait(NULL);
//	printf("Fin main\n");

	ihm();
	return EXIT_SUCCESS;
}
