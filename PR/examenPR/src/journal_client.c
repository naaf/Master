#define _XOPEN_SOURCE 700

/** Client emetteur de fichiers pas TCP **/

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

int main(int argc, char *argv[]) {
	struct sockaddr_in dest; /* Nom du serveur */
	struct hostent *hp;
	int sock;

	if (argc != 3) {
		fprintf(stderr, "Usage : %s ip port\n", argv[0]);
		exit(1);
	}

	char *ip = argv[1];
	short port = (short) atoi(argv[2]);

	/** creation socket TCP **/
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	/* Remplir la structure dest */
	if (inet_aton(ip, &(dest.sin_addr)) == 0) {
		perror("inet_aton init dest address");
		exit(EXIT_FAILURE);
	}
	memset((void *) &dest, 0, sizeof(dest));

	dest.sin_family = AF_INET;
	dest.sin_port = htons(port);

	/* Etablir la connexion */
	if (connect(sock, (struct sockaddr *) &dest, sizeof(dest)) == -1) {
		perror("connect");
		exit(1);
	}

	printf("CX OK\n");

	/* Fermer la connexion */
	shutdown(sock, 2);
	close(sock);
	return (0);
}
