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

int attenteTraitement;
char msg_signal[256];

void gest_ihm(int signum, siginfo_t * info, void * vide);

void traitement(char **tab, int size) {
	if (tab == NULL) {
		fprintf(stderr, "ERROR traitement commande NULL inconnu \n");
		return;
	}
	union sigval valeur;
	SigMsg *sigMsg = malloc(sizeof(SigMsg));
	sigMsg->data = NULL;
	sigMsg->val = 0;
	fprintf(stderr, "traite %s \n", tab[0]); //DEBUG
	if (!strcmp(BIENVENUE, tab[0])) {
		etat |= FIN_CONNEXION;
		sigMsg->val = FIN_CONNEXION;
		valeur.sival_ptr = sigMsg;
		sigqueue(pid_main, SIG_IHM, valeur);
	} else if (!strcmp(CONNECTE, tab[0])) {
		if (size < 2) {
			fprintf(stderr, "ERREUR protocol %s \n", tab[0]);
			return;
		}
		adduser(tab[1], 0, &bilan.list_users);
		sigMsg->val = UPDATE_L;
		valeur.sival_ptr = sigMsg;
		sigqueue(pid_main, SIG_IHM, valeur);
	} else if (!strcmp(DECONNEXION, tab[0])) {
		if (size < 2) {
			fprintf(stderr, "ERREUR protocol %s \n", tab[0]);
			return;
		}
		removeuser(tab[1], &bilan.list_users);
		sigMsg->val = UPDATE_L;
		valeur.sival_ptr = sigMsg;
		sigqueue(pid_main, SIG_IHM, valeur);
	} else if (!strcmp(SESSION, tab[0])) {
		if (size < 2) {
			fprintf(stderr, "ERREUR protocol %s \n", tab[0]);
			return;
		}
		if (etat & FIN_CONNEXION) {
			etat |= PHASE_SESSION;
			init_plateau(pl);
			parse_plateau(tab[1], pl);
			cpyPlateau(pl, initPl);
			sigMsg->val = PHASE_SESSION;
			valeur.sival_ptr = sigMsg;
			sigqueue(pid_main, SIG_IHM, valeur);
		} else {
			erreur("ERREUR Protocol envois SESSION sans CONNEXION", FALSE);
		}
	} else if (!strcmp(VAINQUEUR, tab[0])) {
		if (size < 2) {
			fprintf(stderr, "ERREUR protocol %s \n", tab[0]);
			return;
		}
		etat &= ~PHASE_SESSION;
		parse_bilan(tab[1], &bilan);
		sigMsg->val = FIN_SESSION;
		valeur.sival_ptr = sigMsg;
		sigqueue(pid_main, SIG_IHM, valeur); //TODO
	} else if (!strcmp(TOUR, tab[0])) {
		if (size < 3) {
			fprintf(stderr, "ERREUR protocol %s \n", tab[0]);
			return;
		}
		if (etat & PHASE_SESSION) {
			parse_enigme(tab[1], &enigme);
			parse_bilan(tab[2], &bilan);
			cpyEnigme(&enigme, &initEnigme);
			cpyPlateau(initPl, pl);
			bind_enigme_plateau(pl, &enigme);
			sigMsg->val = PHASE_REFLEX;
			valeur.sival_ptr = sigMsg;
			sigqueue(pid_main, SIG_IHM, valeur);
		} else {
			erreur("ERREUR Protocol envois TOUR sans SESSION", FALSE);
		}

	} else if (!strcmp(TUASTROUVE, tab[0])) {
		memset(msg_signal, 0, sizeof(msg_signal));
		strcpy(msg_signal, "Tu as trouve solution");
		sigMsg->val = PHASE_ENCHERE | SIGALEMENT;
		valeur.sival_ptr = sigMsg;
		sigqueue(pid_main, SIG_IHM, valeur);
	} else if (!strcmp(ILATROUVE, tab[0])) {
		if (size < 3) {
			fprintf(stderr, "ERREUR protocol %s \n", tab[0]);
			return;
		}
		user_t *u = getuser(tab[1], &bilan.list_users);
		if (u == NULL) {
			fprintf(stderr, "user not exist %s \n", tab[1]);
			return;
		}
		u->nb_coups = atoi(tab[2]);

		memset(msg_signal, 0, sizeof(msg_signal));
		sprintf(msg_signal, "%s trouve en %s coups", tab[1], tab[2]);
		sigMsg->val = PHASE_ENCHERE | SIGALEMENT | UPDATE_L;
		valeur.sival_ptr = sigMsg;
		sigqueue(pid_main, SIG_IHM, valeur);
	} else if (!strcmp(FINREFLEXION, tab[0])) {

		memset(msg_signal, 0, sizeof(msg_signal));
		strcpy(msg_signal, "Fin reflexion timeout");
		sigMsg->val = PHASE_ENCHERE | SIGALEMENT;
		valeur.sival_ptr = sigMsg;
		sigqueue(pid_main, SIG_IHM, valeur);
	} else if (!strcmp(VALIDATION, tab[0])) {
		if (size < 1) {
			fprintf(stderr, "ERREUR protocol %s \n", tab[0]);
			return;
		}
		user_t *u = getuser(myName, &bilan.list_users);
		if (u == NULL) {
			erreur("fatal erreur ", TRUE);
		}
		u->nb_coups = valideCoups;
		valideCoups = -1;
		memset(msg_signal, 0, sizeof(msg_signal));
		strcpy(msg_signal, "enchere Valide");
		sigMsg->val = SIGALEMENT | UPDATE_L;
		valeur.sival_ptr = sigMsg;
		sigqueue(pid_main, SIG_IHM, valeur);
	} else if (!strcmp(ECHEC, tab[0])) {
		valideCoups = -1;
		memset(msg_signal, 0, sizeof(msg_signal));
		strcpy(msg_signal, "ECHEC enchere");
		sigMsg->val = SIGALEMENT;
		valeur.sival_ptr = sigMsg;
		sigqueue(pid_main, SIG_IHM, valeur);
	} else if (!strcmp(NOUVELLEENCHERE, tab[0])) {
		if (size < 3) {
			fprintf(stderr, "ERREUR protocol %s \n", tab[0]);
			return;
		}
		memset(msg_signal, 0, sizeof(msg_signal));
		sprintf(msg_signal, "New enchere %s %s", tab[1], tab[2]);
		user_t *u = getuser(tab[1], &bilan.list_users);
		if (u == NULL) {
			fprintf(stderr, "user not exist %s \n", tab[1]);
			return;
		}
		u->nb_coups = atoi(tab[2]);
		sigMsg->val = SIGALEMENT | UPDATE_L;
		valeur.sival_ptr = sigMsg;
		sigqueue(pid_main, SIG_IHM, valeur);

	} else if (!strcmp(FINENCHERE, tab[0])) {
		if (size < 3 || tab[1] == NULL) {
			fprintf(stderr, "ERREUR protocol %s \n", tab[0]);
			return;
		}
		moiJoue = FALSE;
		if (!strcmp(myName, tab[1])) {
			moiJoue = TRUE;
		}
		sigMsg->val = PHASE_RESO;
		valeur.sival_ptr = sigMsg;
		sigqueue(pid_main, SIG_IHM, valeur);

	} else if (!strcmp(SASOLUTION, tab[0])) {
		if (size < 3) {
			fprintf(stderr, "ERREUR protocol %s \n", tab[0]);
			return;
		}
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
		sigMsg->val = FIN_TOUR | SIGALEMENT;
		valeur.sival_ptr = sigMsg;
		sigqueue(pid_main, SIG_IHM, valeur);

	} else if (!strcmp(MAUVAISE, tab[0])) {
		if (size < 2 || tab[1] == NULL) {
			fprintf(stderr, "ERREUR protocol %s \n", tab[0]);
			return;
		}
		memset(msg_signal, 0, sizeof(msg_signal));
		if (!strcmp(myName, tab[1])) {
			moiJoue = TRUE;
			strcpy(msg_signal, "A MOI de Jouer");

		} else {
			moiJoue = FALSE;
			sprintf(msg_signal, "%s joue", tab[1]);
		}
		sigMsg->val = SIGALEMENT;
		valeur.sival_ptr = sigMsg;
		sigqueue(pid_main, SIG_IHM, valeur);

	} else if (!strcmp(FINRESO, tab[0])) {
		sigMsg->val = FIN_TOUR;
		valeur.sival_ptr = sigMsg;
		sigqueue(pid_main, SIG_IHM, valeur);
	} else if (!strcmp(TROPLONG, tab[0])) {
		if (size < 2 || tab[1] == NULL) {
			fprintf(stderr, "ERREUR protocol %s \n", tab[0]);
			return;
		}
		memset(msg_signal, 0, sizeof(msg_signal));
		if (!strcmp(myName, tab[1])) {
			moiJoue = TRUE;
			strcpy(msg_signal, "A MOI de Jouer");
		} else {
			moiJoue = FALSE;
			sprintf(msg_signal, "%s joue ", tab[1]);
		}
		sigMsg->val = SIGALEMENT;
		valeur.sival_ptr = sigMsg;
		sigqueue(pid_main, SIG_IHM, valeur);
	} else if (!strcmp(CHAT, tab[0])) {
		if (size < 3) {
			fprintf(stderr, "ERREUR protocol %s \n", tab[0]);
			return;
		}
		printf("%s : %s", tab[1], tab[2]);
	} else {
		fprintf(stderr, "ERROR de protocol %s inconnu \n", tab[0]);
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
		read_response(STDIN_FILENO, entree);
//		printf("envois : %s", entree);
		send_request(sc, 3, CHAT, myName, entree);
		memset(entree, 0, strlen(entree) + 2);
	}
	return NULL;
}

void *run_com(void *arg) {
	char response[512];
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
		fprintf(stderr, "com: recu reponse len %zu : %s\n", strlen(response),
				response);

		if (0 == strlen(response)) {
			fprintf(stderr, "ERROR : Connection Socket");
			erreur("ERROR : Connection Socket", TRUE);
			break;
		}

		tab = string_to_arraystring(response, &size, '/');

		traitement(tab, size);

		/*free resources*/
		free_table(tab, size);
		memset(response, 0, sizeof(response));
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
	attenteTraitement = 0;
	etat = 0;
	moiJoue = FALSE;
	quit = FALSE;
	/*--------Initialisation et gestions des signaux--------*/

	action.sa_sigaction = gest_ihm;
	action.sa_flags = SA_SIGINFO|SA_RESTART;
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
	/*--------------------Fin de main----------------------*/
	pthread_join(thread_com, NULL);
	pthread_join(thread_chat, NULL);
	if (bilan.list_users.nb != 0) {
		free_list_user(&bilan.list_users);
	}
	fprintf(stderr, "Fin main\n");

	return EXIT_SUCCESS;
}
