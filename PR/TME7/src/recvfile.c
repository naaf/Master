#define _SVID_SOURCE 1

#define _XOPEN_SOURCE 700
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

void process() {

}

int main(int argc, char* argv[]) {
	int nb_fils, i;

	if (argc != 2) {
		printf("error syntaxe : %s nbPort \n", argv[0]);
		exit(1);
	}
	int PORTSERV;
	struct sockaddr_in sin;
	int sc;
	int scom; /* Socket de communication */
	struct hostent *hp;
	struct sockaddr_in exp; /* Nom de la socket distante */
	int fromlen = sizeof(exp);
	char buffer[1024];

	PORTSERV = atoi(argv[1]);
	/* creation de la socket */
	if ((sc = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(1);
	}
	memset((char *) &sin, 0, sizeof(sin));
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_port = htons(PORTSERV);
	sin.sin_family = AF_INET;
	/* nommage */
	if (bind(sc, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
		perror("bind");
		exit(1);
	}
	listen(sc, 5);
	memset(buffer, 0, sizeof(buffer));

	if ((scom = accept(sc, (struct sockaddr *) &exp, &fromlen)) == -1) {
		perror("accept");
		exit(3);
	}
	FILE *f;
	if (read(scom, buffer, sizeof(buffer)) < 0) {
		perror("read");
		exit(4);
	}
	if ((f = fopen(buffer, "w")) == NULL) {
		perror("fopen");
	}
	do {
		if (read(scom, buffer, sizeof(buffer)) == 0) {
			perror("read");
			break;
		}
		printf("msg %s \n", buffer);
		fprintf(f, "%s ", buffer);

	} while (1);
	/* Fermer la communication */
	shutdown(scom, 2);
	close(scom);
	exit(0);

	close(sc);

	return EXIT_SUCCESS;
}
