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
#define IP_ADDR "127.0.0.1"

/********** variable globale ****************/
pthread_t thread_com, thread_chat;
int sc, pid_ihm, pid_main;
plateau_t pl;
enigme_t enigme;
bilan_t bilan;
int coupure;
char msg_signal[128];

void gest_ihm(int signum, siginfo_t * info, void * vide);

void traitement(char **tab) {
	union sigval valeur;

	if (tab == NULL) {
		printf("ERROR traitement commande NULL inconnu \n");
		return;
	}

	if (!strcmp(BIENVENUE, tab[0])) {
		// casser l'attente TODO
		valeur.sival_int = FIN_CONNEXION;
		sigqueue(pid_main, SIG_IHM, valeur);
	} else if (!strcmp(CONNECTE, tab[0])) {
		adduser(tab[1], 0, &bilan.list_users);//TODO
	} else if (!strcmp(DECONNEXION, tab[0])) {
		removeuser(tab[1], &bilan.list_users);
	} else if (!strcmp(SESSION, tab[0])) {
		fprintf(stderr, "%s\n", tab[1]);
		init_plateau(pl);
		parse_plateau(tab[1], pl);
		valeur.sival_int = PHASE_SESSION;
		sigqueue(pid_main, SIG_IHM, valeur);
	} else if (!strcmp(VAINQUEUR, tab[0])) {
		parse_bilan(tab[1], &bilan);
		valeur.sival_int = FIN_SESSION;
		memset(msg_signal, 0, sizeof(msg_signal));
		strcpy(msg_signal, "FIN SESSION");
		sigqueue(pid_main, SIG_IHM, valeur);
	} else if (!strcmp(TOUR, tab[0])) {
		memset(msg_signal, 0, sizeof(msg_signal));
		strcpy(msg_signal, "PHASE REFLEXTION");
		parse_enigme(tab[1], &enigme);
		parse_bilan(tab[2], &bilan);
		valeur.sival_int = PHASE_REFLEX;
		sigqueue(pid_main, SIG_IHM, valeur);

	} else if (!strcmp(TUASTROUVE, tab[0])) {
		memset(msg_signal, 0, sizeof(msg_signal));
		strcpy(msg_signal, "Tu as trouve solution");
		valeur.sival_int = FIN_REFLEX | SIGALEMENT;
		sigqueue(pid_main, SIG_IHM, valeur);
	} else if (!strcmp(ILATROUVE, tab[0])) {
		user_t *u = getuser(tab[1], &bilan.list_users);
		u->nb_coups = atoi(tab[2]);

		memset(msg_signal, 0, sizeof(msg_signal));
		sprintf(msg_signal, "%s as trouve solution en %d coups", tab[1],
				tab[2]);
		valeur.sival_int = FIN_REFLEX | SIGALEMENT;

	} else if (!strcmp(FINREFLEXION, tab[0])) {
		memset(msg_signal, 0, sizeof(msg_signal));
		strcpy(msg_signal, "Fin reflexion timeout");
		valeur.sival_int = FIN_REFLEX;
		sigqueue(pid_main, SIG_IHM, valeur);
	} else if (!strcmp(VALIDATION, tab[0])) {
		memset(msg_signal, 0, sizeof(msg_signal));
		strcpy(msg_signal, "enchere Valide");
		valeur.sival_int = 0;
		sigqueue(pid_main, SIG_IHM, valeur);
	} else if (!strcmp(ECHEC, tab[0])) {
		memset(msg_signal, 0, sizeof(msg_signal));
		strcpy(msg_signal, "ECHEC enchere");
		valeur.sival_int = 0;
		sigqueue(pid_main, SIG_IHM, valeur);
	} else if (!strcmp(NOUVELLEENCHERE, tab[0])) {
		memset(msg_signal, 0, sizeof(msg_signal));
		sprintf(msg_signal, "New enchere %s %s", tab[1], tab[2]);
		valeur.sival_int = 0;
		sigqueue(pid_main, SIG_IHM, valeur);

	} else if (!strcmp(FINENCHERE, tab[0])) {
		memset(msg_signal, 0, sizeof(msg_signal));
		strcpy(msg_signal, "FIN enchere");
		valeur.sival_int = FIN_ENCHERE;
		sigqueue(pid_main, SIG_IHM, valeur);

	} else if (!strcmp(SASOLUTION, tab[0])) {
		user_t *u = getuser(tab[1], &bilan.list_users);
		if (u->solution == NULL) {
			free(u->solution);
		}
		u->solution = strdup(tab[2]);

	} else if (!strcmp(BONNE, tab[0])) {

	} else if (!strcmp(MAUVAISE, tab[0])) {

	} else if (!strcmp(FINRESO, tab[0])) {

	} else if (!strcmp(TROPLONG, tab[0])) {

	} else if (!strcmp(CHAT, tab[0])) {
		printf("%s : %s", tab[1], tab[2]);
	} else {
		printf("ERROR de protocol %s inconnu \n", tab[0]);
	}
}

void aff(char c) {
	while (1) {
		printf("%c", c);
		sleep(1);
		fflush(stdout);
	}
}

void *run_chat(void *arg) {
	char entree[512];
	memset(entree, 0, sizeof(entree));

	/*--------------Blocquer les signaux------------------*/
	sigset_t mask;
	sigfillset(&mask);
	pthread_sigmask(SIG_SETMASK, &mask, NULL);

	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	while (TRUE) {
		printf("entrez votre msg : ");
		fflush(stdout);
		fgets(entree, sizeof(entree), stdin);
		printf("envois : %s", entree);
		send_request(sc, 2, CHAT, entree);
		memset(entree, 0, strlen(entree) + 2);
	}
	return NULL;
}

void *run_com(void *arg) {
	char response[1024];
	char **tab = NULL;
	int size;

	memset(response, 0, 1024);

	printf("thread com demarre :\n");

	/*--------------Blocquer les signaux------------------*/
	sigset_t mask;
	sigfillset(&mask);
	pthread_sigmask(SIG_SETMASK, &mask, NULL);

	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

	while (TRUE) {
		read_response(sc, response);
		printf("com: recu reponse len %zu : %s\n", strlen(response), response);

		if (0 == strlen(response)) {
			fprintf(stderr, "ERROR : Connection Socket");
			break;
		}

		tab = string_to_arraystring(response, &size, '/');

		traitement(tab);

		/*free resources*/
		free_table(tab, size);
		memset(response, 0, strlen(response) + 1);
	}
	pthread_cancel(thread_chat); //TODO
	return NULL;
}

int main(int argc, char** argv) {

	pid_main = getpid();
	printf("pid Main %d SIG_IHM %d\n", pid_main, SIG_IHM);

	sigset_t mask;
	struct sigaction action;

	/*--------Initialisation et gestions des signaux--------*/

	action.sa_sigaction = gest_ihm;
	action.sa_flags = SA_SIGINFO | SA_RESTART;
	sigfillset(&mask);
	sigdelset(&mask, SIG_IHM);
	sigdelset(&mask, SIGINT);
	if (sigprocmask(SIG_SETMASK, &mask, NULL) == -1) {
		perror("sigprocmask");
	}
	sigemptyset(&action.sa_mask);
	if (sigaction(SIG_IHM, &action, NULL) < 0) {
		fprintf(stderr, "SIG_IHM non intercepté \n");
	}
//
//
//	if (argc == 3) {
//		sc = connex_socket(argv[1], atoi(argv[2]));
//	} else {
//		sc = connex_socket(IP_ADDR, PORT_SRV);
//	}
//
//	if (pthread_create(&thread_com, NULL, run_com, NULL) != 0) {
//		perror("pthread_create \n");
//		exit(1);
//	}
//	if (pthread_create(&thread_chat, NULL, run_chat, NULL) != 0) {
//		perror("pthread_create \n");
//		exit(1);
//	}

	/*--------------------IHM----------------------*/
	ihm();
//	test();
//	/*--------------------Fin de main----------------------*/
//	pthread_join(thread_com, NULL);
//	pthread_join(thread_chat, NULL);
	if (bilan.list_users.nb != 0) {
		free_list_user(&bilan.list_users);
	}
	printf("Fin main\n");

	return EXIT_SUCCESS;
}
