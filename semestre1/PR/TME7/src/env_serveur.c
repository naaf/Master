#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

int main(int argc, char *argv[]) {

	struct sockaddr_in sin; /* address du serveur */
	struct sockaddr_in dest; /* address dest */
	char host[64];
	int sc;
	int fromlen = sizeof(dest);
	char message[256];

	if (argc != 2) {
		printf("error syntaxe : %s nbPort \n", argv[0]);
		exit(1);
	}

	/* creation de la socket */
	if ((sc = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket");
		exit(1);
	}

	int PORTSERV;
	PORTSERV = atoi(argv[1]);

	/* remplir le « nom » */
	memset((char *) &sin, 0, sizeof(sin));
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_port = htons(PORTSERV);
	sin.sin_family = AF_INET;

	/* nommage */
	if (bind(sc, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
		perror("bind");
		exit(2);
	}

	printf("serveur env >>>\n");
	char requete;
	char identificateur[256];
	char valeur[256];
	char* ret;
	int boucle = 1;

	do {
		/*** Reception du message ***/
		if (recvfrom(sc, message, sizeof(message), 0, (struct sockaddr *) &dest,
				&fromlen) == -1) {
			perror("recvfrom");
			exit(2);
		}

		/*** Traitement ***/

		sscanf(message, "%c", &requete);
		printf("requete %c \n", requete);
		switch (requete) {
		case 'S':
			sscanf(message, "%c %s %s", &requete, identificateur, valeur);
			setenv(identificateur, valeur, 0);
			printf("enregistrement %s = %s \n", identificateur, valeur);
			break;
		case 'G':
			sscanf(message, "%c %s", &requete, identificateur);
			ret = getenv(identificateur);
			ret = (ret == NULL) ? "inconnu" : ret;
			printf("send pour %s ==> %s \n", identificateur, ret);
			if (sendto(sc, ret, sizeof(ret), 0, (struct sockaddr *) &dest,
					fromlen) == -1) {
				perror("sendto");
				exit(4);
			}
			break;
		case 'Q':
			boucle = 0;
			printf("quit \n");
			break;
		default:
			printf("error requete inconnu\n");
		}
	} while (boucle);

	close(sc);
	return (0);
}
