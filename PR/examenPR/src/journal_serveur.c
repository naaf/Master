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

int VRAI = 1;

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
		printf("sc_attente %d \n", port);
		perror("bind");
		exit(1);
	}
	return sc;
}

int main(int argc, char* argv[]) {

	if (argc != 5) {
		printf("error syntaxe : %s nbPort nbPort nbPort nbPort \n", argv[0]);
		exit(1);
	}

	int PORTSERV[N_SOCKET];
	int socket_attentes[N_SOCKET];
	int sc_com; /* Socket de communication */
	struct sockaddr_in dest; /* Nom de la socket distante */
	int fromlen = sizeof(dest);

	fd_set mselect;
	int max_desc = -1;

	int i;
	for (i = 1; i <= N_SOCKET; i++)
		PORTSERV[i - 1] = atoi(argv[i]);

	/** Mise à zéro de l'ensemble **/
	FD_ZERO(&mselect);

	/* creation de la socket et ajoute à l'ensemble*/
	for (i = 0; i < N_SOCKET; i++) {
		socket_attentes[i] = creer_socket(PORTSERV[i]);
		listen(socket_attentes[i], 5);

		/** Ajoute un descripteur à l'ensemble**/
		FD_SET(socket_attentes[i], &mselect);
		if (max_desc < socket_attentes[i]) {
			max_desc = socket_attentes[i];
		}
		printf("test max_desc %d \n", max_desc);
	}

	FILE* file;
	if ((file = fopen("cx.log", "w")) == NULL) {
		perror("fopen");
	}

	while (VRAI) {

		/**Attente simultanée sur l'ensembles de descripteurs**/
		if (select(max_desc + 1, &mselect, NULL, NULL, NULL) <= 0) {
			perror("select");
			break;
		}

		struct sockaddr_in info_local;
		socklen_t len = sizeof(info_local);

		/**traitement des cnx**/
		for (i = 0; i < N_SOCKET; ++i) {

			/** Teste si un descripteur est dans l'ensemble **/
			if (FD_ISSET(socket_attentes[i], &mselect)) {

				/* Etablir la connexion */
				if ((sc_com = accept(socket_attentes[i],
						(struct sockaddr *) &dest, &fromlen)) == -1) {
					perror("accept");
					exit(2);
				}

				/*********** Traitement ***********/

				fprintf(file, "IP = %s  port => %d \n",
						inet_ntoa(dest.sin_addr), dest.sin_port);
				/**get info client**/
				printf("CLIENT IP = %s,port = %d \n", inet_ntoa(dest.sin_addr),
						dest.sin_port);

				/**get info  srv**/
				if (getsockname(sc_com, (struct sockaddr *) &info_local, &len)
						== -1) {
					perror("getsockname");
				}
				printf("SRV IP = %s,port = %d \n", inet_ntoa(info_local.sin_addr),
						ntohs(info_local.sin_port));

				/* Fermer la connexion */
				shutdown(sc_com, 2);
				close(sc_com);
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
