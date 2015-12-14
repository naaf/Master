#define _SVID_SOURCE 1

#define _XOPEN_SOURCE 700
#define _POSIX_SOUCE 1

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <signal.h>

#define N_SOCKET 4

int creer_socket(int port) {
	int sc;
	struct sockaddr_in sin;
	if ((sc = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(1);
	}
	memset((char *) &sin, 0, sizeof(sin));
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_port = htons(port);
	sin.sin_family = AF_INET;
	/* nommage */
	if (bind(sc, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
		perror("bind");
		exit(1);
	}
	return sc;
}

int continued = 1;

void handler(int s) {
	printf("exit server\n");
	continued = 0;
}

int main(int argc, char* argv[]) {

	if (argc != 5) {
		printf("error syntaxe : %s nbPort nbPort nbPort nbPort \n", argv[0]);
		exit(1);
	}
	int PORTSERV[N_SOCKET];
	int socket_attentes[N_SOCKET];
	fd_set mselect;
	int plg_desc = -1;

	int socket_cnx; /* Socket de communication */
	struct sockaddr_in sc_from; /* Nom de la socket distante */
	int sc_from_len = sizeof(sc_from);

	int i;
	for (i = 1; i <= N_SOCKET; i++)
		PORTSERV[i - 1] = atoi(argv[i]);

	FD_ZERO(&mselect);

	/* creation de la socket et ecoute sur la socket*/
	for (i = 0; i < N_SOCKET; i++) {
		socket_attentes[i] = creer_socket(PORTSERV[i]);
		listen(socket_attentes[i], 5);
		FD_SET(socket_attentes[i], &mselect);
		if (plg_desc < socket_attentes[i]) {
			plg_desc = socket_attentes[i];
		}
		printf("test plg_desc %d \n", plg_desc);
	}

	printf("%s >>>\n", argv[0]);

	FILE* file;
	if ((file = fopen("cx.log", "w")) == NULL) {
		perror("fopen");
	}

	signal(SIGINT, handler);

	while (continued) {

		if (select(plg_desc + 1, &mselect, NULL, NULL, NULL) <= 0) {
			perror("select");
			break;
		}
		printf("select interrompu \n");
		for (i = 0; i < N_SOCKET; ++i) {
			if (FD_ISSET(socket_attentes[i], &mselect)) {
				/* Etablir la connexion */
				if ((socket_cnx = accept(socket_attentes[i],
						(struct sockaddr *) &sc_from, &sc_from_len)) == -1) {
					perror("accept");
					exit(2);
				}
				/******** Traitement *********/
				fprintf(stdout, "IP = %s,  \n", inet_ntoa(sc_from.sin_addr));
				fprintf(file, "IP = %s  \n", inet_ntoa(sc_from.sin_addr));
				/* Fermer la connexion */
				shutdown(socket_cnx, 2);
				close(socket_cnx);
			}
		}

		/** reinitialiser l'ensemble d'ecoute sur select**/
		FD_ZERO(&mselect);
		for (i = 0; i < N_SOCKET; i++) {
			FD_SET(socket_attentes[i], &mselect);
		}
	}

	/* Fermer des sockets attentes */
	for (i = 0; i < N_SOCKET; i++) {
		close(socket_attentes[i]);
	}
	fclose(file);

	return EXIT_SUCCESS;
}
