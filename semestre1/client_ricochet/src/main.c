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
#define IP_ADDR "132.227.115.112"

/********** variable globale ****************/
pthread_t thread_com, thread_chat;
int sc, pid_ihm, pid_main;
bool_t moiJoue;
char myName[256];
int valideCoups;
int etat;
bool_t quit;

plateau_t pl;
enigme_t enigme;
bilan_t bilan;
plateau_t initPl;
enigme_t initEnigme;

int typeTraitement;
char msg_signal[128];

void gest_ihm(int signum, siginfo_t * info, void * vide);

void traitement(char **tab) {
	union sigval valeur;

	if (tab == NULL) {
		fprintf(stderr, "ERROR traitement commande NULL inconnu \n");
		return;
	}
	fprintf(stderr, "traite %s \n", tab[0]);
	if (!strcmp(BIENVENUE, tab[0])) {
		etat |= FIN_CONNEXION;
		valeur.sival_int = FIN_CONNEXION;
		sigqueue(pid_main, SIG_IHM, valeur);
	} else if (!strcmp(CONNECTE, tab[0])) {
		adduser(tab[1], 0, &bilan.list_users);
		valeur.sival_int = UPDATE_L;
		sigqueue(pid_main, SIG_IHM, valeur);
	} else if (!strcmp(DECONNEXION, tab[0])) {
		removeuser(tab[1], &bilan.list_users);
		valeur.sival_int = UPDATE_L;
		if (bilan.list_users.nb < 2) {
			erreur("Fin de jeu moins de 2 joueurs", TRUE);
		}
		sigqueue(pid_main, SIG_IHM, valeur);
	} else if (!strcmp(SESSION, tab[0])) {
		if (etat & FIN_CONNEXION) {
			etat |= PHASE_SESSION;
			fprintf(stderr, "%s\n", tab[1]);
			init_plateau(pl);
			parse_plateau(tab[1], pl);
			cpyPlateau(pl, initPl);
			valeur.sival_int = PHASE_SESSION;
			sigqueue(pid_main, SIG_IHM, valeur);
		} else {
			erreur("ERREUR Protocol envois SESSION sans CONNEXION", FALSE);
		}
	} else if (!strcmp(VAINQUEUR, tab[0])) {
		etat &= ~PHASE_SESSION;
		parse_bilan(tab[1], &bilan);
		valeur.sival_int = FIN_SESSION;
		sigqueue(pid_main, SIG_IHM, valeur); //TODO
	} else if (!strcmp(TOUR, tab[0])) {
		if (etat & PHASE_SESSION) {
			parse_enigme(tab[1], &enigme);
			parse_bilan(tab[2], &bilan);
			cpyEnigme(&enigme, &initEnigme);
			cpyPlateau(initPl, pl);
			valeur.sival_int = PHASE_REFLEX;
			sigqueue(pid_main, SIG_IHM, valeur);
		} else {
			erreur("ERREUR Protocol envois TOUR sans SESSION", FALSE);
		}

	} else if (!strcmp(TUASTROUVE, tab[0])) {
		memset(msg_signal, 0, sizeof(msg_signal));
		strcpy(msg_signal, "Tu as trouve solution");
		valeur.sival_int = PHASE_ENCHERE | SIGALEMENT;
		sigqueue(pid_main, SIG_IHM, valeur);
	} else if (!strcmp(ILATROUVE, tab[0])) {
		user_t *u = getuser(tab[1], &bilan.list_users);
		if (u == NULL) {
			fprintf(stderr, "user not exist %s \n", tab[1]);
			return;
		}
		u->nb_coups = atoi(tab[2]);

		memset(msg_signal, 0, sizeof(msg_signal));
		sprintf(msg_signal, "%s trouve en %s coups", tab[1], tab[2]);
		valeur.sival_int = PHASE_ENCHERE | SIGALEMENT | UPDATE_L;
		sigqueue(pid_main, SIG_IHM, valeur);
	} else if (!strcmp(FINREFLEXION, tab[0])) {
		memset(msg_signal, 0, sizeof(msg_signal));
		strcpy(msg_signal, "Fin reflexion timeout");
		valeur.sival_int = PHASE_ENCHERE | SIGALEMENT;
		sigqueue(pid_main, SIG_IHM, valeur);
	} else if (!strcmp(VALIDATION, tab[0])) {
		user_t *u = getuser(myName, &bilan.list_users);
		if (u == NULL) {
			fprintf(stderr, "user not exist %s \n", tab[1]);
			return;
		}
		u->nb_coups = valideCoups;
		valideCoups = -1;
		memset(msg_signal, 0, sizeof(msg_signal));
		strcpy(msg_signal, "enchere Valide");
		valeur.sival_int = SIGALEMENT | UPDATE_L;
		sigqueue(pid_main, SIG_IHM, valeur);
	} else if (!strcmp(ECHEC, tab[0])) {
		valideCoups = -1;
		memset(msg_signal, 0, sizeof(msg_signal));
		strcpy(msg_signal, "ECHEC enchere");
		valeur.sival_int = SIGALEMENT;
		sigqueue(pid_main, SIG_IHM, valeur);
	} else if (!strcmp(NOUVELLEENCHERE, tab[0])) {
		memset(msg_signal, 0, sizeof(msg_signal));
		sprintf(msg_signal, "New enchere %s %s", tab[1], tab[2]);
		user_t *u = getuser(tab[1], &bilan.list_users);
		if (u == NULL) {
			fprintf(stderr, "user not exist %s \n", tab[1]);
			return;
		}
		printf("bonojour %s %s %s\n", tab[0], tab[1], tab[2]);
		u->nb_coups = atoi(tab[2]);
		valeur.sival_int = SIGALEMENT | UPDATE_L;
		sigqueue(pid_main, SIG_IHM, valeur);

	} else if (!strcmp(FINENCHERE, tab[0])) {
		if (!strcmp(myName, tab[1])) {
			moiJoue = TRUE;
		}
		valeur.sival_int = PHASE_RESO;
		sigqueue(pid_main, SIG_IHM, valeur);

	} else if (!strcmp(SASOLUTION, tab[0])) {
		user_t *u = getuser(tab[1], &bilan.list_users);
		if (u == NULL) {
			fprintf(stderr, "user not exist %s \n", tab[1]);
			return;
		}
		if (u->solution != NULL) {
			free(u->solution);
		}
		u->solution = strdup(tab[2]);
		u->nb_coups = strlen(tab[2]);

	} else if (!strcmp(BONNE, tab[0])) {
		memset(msg_signal, 0, sizeof(msg_signal));
		strcpy(msg_signal, "VOUS GAGNEZ");
		valeur.sival_int = FIN_TOUR | SIGALEMENT;
		sigqueue(pid_main, SIG_IHM, valeur);

	} else if (!strcmp(MAUVAISE, tab[0])) {
		memset(msg_signal, 0, sizeof(msg_signal));
		if (!strcmp(myName, tab[1])) {
			moiJoue = TRUE;
			strcpy(msg_signal, "A MOI de Jouer");

		} else {
			sprintf(msg_signal, "%s joue", tab[1]);
		}
		valeur.sival_int = SIGALEMENT;
		sigqueue(pid_main, SIG_IHM, valeur);

	} else if (!strcmp(FINRESO, tab[0])) {
		valeur.sival_int = FIN_TOUR;
		sigqueue(pid_main, SIG_IHM, valeur);
	} else if (!strcmp(TROPLONG, tab[0])) {
		memset(msg_signal, 0, sizeof(msg_signal));
		if (!strcmp(myName, tab[1])) {
			moiJoue = TRUE;
			strcpy(msg_signal, "A MOI de Jouer");
		} else {
			sprintf(msg_signal, "%s joue ", tab[1]);
		}
		valeur.sival_int = SIGALEMENT;
		sigqueue(pid_main, SIG_IHM, valeur);
	} else if (!strcmp(CHAT, tab[0])) {
		printf("%s : %s", tab[1], tab[2]);
	} else {
		fprintf(stderr, "ERROR de protocol %s inconnu \n", tab[0]);
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
//		printf("envois : %s", entree);
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

	fprintf(stderr, "thread com demarre :\n");

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
			erreur("ERROR : Connection Socket", TRUE);
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
//	printf("pid Main %d SIG_IHM %d\n", pid_main, SIG_IHM);

	sigset_t mask;
	struct sigaction action;
	/*--------------initialisation vars global--------------*/
	typeTraitement = 0;
	etat = 0;
	moiJoue = FALSE;
	quit = FALSE;
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

	if (argc == 3) {
		sc = connex_socket(argv[1], atoi(argv[2]));
	} else {
		sc = connex_socket(IP_ADDR, PORT_SRV);
	}

	if (pthread_create(&thread_com, NULL, run_com, NULL) != 0) {
		perror("pthread_create \n");
		exit(1);
	}
	if (pthread_create(&thread_chat, NULL, run_chat, NULL) != 0) {
		perror("pthread_create \n");
		exit(1);
	}

	/*--------------------------IHM-------------------------*/

	ihm();
//	test();
//	/*--------------------Fin de main----------------------*/
	pthread_join(thread_com, NULL);
	pthread_join(thread_chat, NULL);
	if (bilan.list_users.nb != 0) {
		free_list_user(&bilan.list_users);
	}
	fprintf(stderr, "Fin main\n");

	return EXIT_SUCCESS;
}
