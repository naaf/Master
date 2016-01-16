#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define LIST "LIST"
#define UPLOAD "UPLOAD"
#define DOWNLOAD "DOWNLOAD"

int main(int argc, char *argv[]) {
	struct sockaddr_in dest; /* Nom du serveur */
	int sock;
	int fromlen = sizeof(dest);

	if (argc != 3) {
		printf("error syntaxe : %s 127.0.0.1 2000 \n", argv[0]);
		exit(1);
	}
	/****/
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}
	char *ip = argv[1];
	short port = (short) atoi(argv[2]);

	/* Remplir la structure dest */
	memset((char *) &dest, 0, sizeof(dest));
	if (inet_aton(ip, &(dest.sin_addr)) == 0) {
		fprintf(stdout, "invalide \n");
		exit(2);
	}
	dest.sin_family = AF_INET;
	dest.sin_port = htons(port);

	/* Etablir la connexion */
	if (connect(sock, (struct sockaddr *) &dest, sizeof(dest)) == -1) {
		perror("connect");
		exit(1);
	}

	char requete[256];
	char reponse[256];

	memset(requete, 0, 256);
	memset(reponse, 0, 256);

	while (1) {
		printf("ftp_client >>> ");
		fscanf(stdin, "%s", requete);
		if (strcmp(requete, LIST) == 0 || strcmp(requete, UPLOAD) == 0
				|| strcmp(requete, DOWNLOAD) == 0) {

			printf("requete %s \n",requete);
			/* envois */
			if (write(sock, requete, sizeof(requete)) == -1) {
				perror("write");
				exit(1);
			}
			/* reponse */
			if (read(sock, reponse, sizeof(reponse)) == -1) {
				perror("read");
				exit(1);
			}
			printf("Reponse : %s\n", reponse);
		} else {
			printf("requete inconnu \n");
		}

	}

	/* Fermer la connexion */
	shutdown(sock, 2);
	close(sock);
	return (0);
}
