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
#include <sys/stat.h>
#include <fcntl.h>

#define LIST "LIST"
#define UPLOAD "UPLOAD"
#define DOWNLOAD "DOWNLOAD"

DIR *pt_Dir;
struct dirent* dirEnt;
char buffer[256];

char requete[256];
char reponse[256];
char *repertoire;
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

void upload_srv(int sock) {
	printf("srv  requete %s \n", requete);

	if (read(sock, requete, sizeof(requete)) == -1) {
		perror("read");
		exit(1);
	}
	char name[255] = { 0 };
	strcpy(name, repertoire);
	strcat(name, "/");
	strcat(name, requete);
	printf("nom de fichier >>> %s", name);

	int f;
	if ((f = open(name, O_WRONLY | O_CREAT | O_TRUNC, 0600)) == -1) {
		perror("open");
	}
	memset(requete, 0, 256);
	while (read(sock, requete, sizeof(requete)) > 0) {
		printf("r : %s\n", requete);
		if (write(f, requete, sizeof(requete)) == -1) {
			perror("read");
			exit(1);
		}
	}
	close(f);

}

void download_srv(int scom) {
	printf("srv  requete %s \n", requete);

	if (read(scom, requete, sizeof(requete)) == -1) {
		perror("read");
		exit(1);
	}
	char name[255] = { 0 };
	strcpy(name, repertoire);
	strcat(name, "/");
	strcat(name, requete);
	printf("nom de fichier >>> %s", name);

	int f;
	if ((f = open(name, O_RDONLY, 0600)) == -1) {
		perror("open");
		if (write(scom, "ERROR", sizeof("ERROR")) == -1) {
			perror("write");
		}
		exit(1);
	}

	if (write(scom, requete, sizeof(requete)) == -1) {
		perror("write");
		exit(1);
	}
	memset(requete, 0, 256);
	while (read(f, requete, sizeof(requete)) > 0) {
		if (write(scom, requete, sizeof(requete)) == -1) {
			perror("read");
			exit(1);
		}
	}
	close(f);
}

int main(int argc, char* argv[]) {

	if (argc != 3) {
		printf("error syntaxe : %s nbPort repertoire \n", argv[0]);
		exit(1);
	}

	int port;
	struct sockaddr_in local;
	int sc_cnx;
	int scom; /* Socket de communication */
	struct sockaddr_in dest; /* Nom de la socket distante */
	int fromlen = sizeof(dest);

	port = atoi(argv[1]);
	repertoire = argv[2];

	/* creation TCP socket */
	if ((sc_cnx = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(1);
	}

	/* Remplir address local */
	memset((char *) &local, 0, sizeof(local));
	local.sin_addr.s_addr = htonl(INADDR_ANY);
	local.sin_port = htons(port);
	local.sin_family = AF_INET;

	/* nommage */
	if (bind(sc_cnx, (struct sockaddr *) &local, sizeof(local)) < 0) {
		perror("bind");
		exit(1);
	}

	listen(sc_cnx, 5);

	/****** TRAITEMENT *********/

	memset(requete, 0, 256);
	memset(reponse, 0, 256);

	printf("ftp_serveur >>>\n");
	do {
		if ((scom = accept(sc_cnx, (struct sockaddr *) &dest, &fromlen))
				== -1) {
			perror("accept");
			exit(3);
		}

		switch (fork()) {
		case 0: /* fils */

			/******** Traitement *********/
			close(sc_cnx);

			if (read(scom, requete, sizeof(requete)) == 0) {
				perror("read");
				break;
			}
			printf("msg %s \n", requete);

			if (strcmp(requete, LIST) == 0) {
				listFiles(repertoire, scom);
			} else if (strcmp(requete, UPLOAD) == 0) {
				upload_srv(scom);
			} else if (strcmp(requete, DOWNLOAD) == 0) {
				download_srv(scom);
			} else {
				char* error = "requete inconnu";
				if (write(scom, error, sizeof(error)) == -1) {
					perror("write");
					exit(1);
				}
			}

			exit(EXIT_SUCCESS);
			break;
		case -1:
			perror("fork");
			exit(EXIT_FAILURE);
			break;
		default: /* père */
			close(scom);
			break;
		}

		shutdown(scom, 2);
		close(scom);
	} while (1);

	/* Fermer la communication */
	close(sc_cnx);

	return EXIT_SUCCESS;
}
