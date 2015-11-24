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
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>

#define LIST "LIST"
#define UPLOAD "UPLOAD"
#define DOWNLOAD "DOWNLOAD"

DIR *pt_Dir;
struct dirent* dirEnt;
char buffer[256];
void listFiles(const char* buff_path, int scom) {

	if ((pt_Dir = opendir(buff_path)) == NULL) {

		perror("erreur opendir \n");
		exit(1);
	}

	memset(buffer, 0, 256);
	/* lire répertoire */
	while ((dirEnt = readdir(pt_Dir)) != NULL) {
		strcat(buffer, dirEnt->d_name);
		strcat(buffer, "\n");
	}
	if (write(scom, buffer, sizeof(buffer)) == -1) {
		perror("write");
		exit(1);
	}

	closedir(pt_Dir);
}

int main(int argc, char* argv[]) {

	if (argc != 3) {
		printf("error syntaxe : %s nbPort repertoire \n", argv[0]);
		exit(1);
	}

	int PORTSERV;
	struct sockaddr_in sin;
	int sc;
	int scom; /* Socket de communication */
	struct hostent *hp;
	struct sockaddr_in exp; /* Nom de la socket distante */
	int fromlen = sizeof(exp);
	char *repertoire;

	PORTSERV = atoi(argv[1]);
	repertoire = argv[2];

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

	char requete[256];
	char reponse[256];

	memset(requete, 0, 256);
	memset(reponse, 0, 256);

	printf("ftp_serveur >>>\n");
	do {
		if ((scom = accept(sc, (struct sockaddr *) &exp, &fromlen)) == -1) {
			perror("accept");
			exit(3);
		}
		/******** Traitement *********/

		if (read(scom, requete, sizeof(requete)) == 0) {
			perror("read");
			break;
		}
		printf("msg %s \n", requete);

		if (strcmp(requete, LIST) == 0) {
			listFiles(repertoire, scom);
		} else {
			if (write(scom, requete, sizeof(requete)) == -1) {
				perror("write");
				exit(1);
			}
		}

		shutdown(scom, 2);
		close(scom);
	} while (1);

	/* Fermer la communication */
	close(sc);

	return EXIT_SUCCESS;
}
