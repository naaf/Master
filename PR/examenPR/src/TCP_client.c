#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/stat.h>
#include <fcntl.h>

#define LIST "LIST"
#define UPLOAD "UPLOAD"
#define DOWNLOAD "DOWNLOAD"

char requete[256];
char reponse[256];

void listFiles_cli(int sc_cnx) {
	printf("requete %s \n", requete);
	/* envois */
	if (write(sc_cnx, requete, sizeof(requete)) == -1) {
		perror("write");
		exit(1);
	}
	/* reponse */
	if (read(sc_cnx, reponse, sizeof(reponse)) == -1) {
		perror("read");
		exit(1);
	}
	printf("Reponse : %s\n", reponse);
}

void upload_cli(int sc_cnx) {
	printf("srv  requete %s \n", requete);

	if (write(sc_cnx, requete, sizeof(requete)) == -1) {
		perror("read");
		exit(1);
	}

	printf("nom de fichier >>> ");
	fscanf(stdin, "%s", requete);

	if (write(sc_cnx, requete, sizeof(requete)) == -1) {
		perror("read");
		exit(1);
	}

	int f;
	if ((f = open(requete, O_RDONLY | O_EXCL, 0600)) == -1) {
		perror("open");
	}
	memset(requete, 0, 256);
	while (read(f, requete, sizeof(requete)) > 0) {
		printf("r : %s\n", requete);
		if (write(sc_cnx, requete, sizeof(requete)) == -1) {
			perror("read");
			exit(1);
		}
	}
	close(f);

}
void download_cli(int sc_cnx) {
	printf("srv  requete %s \n", requete);

	if (write(sc_cnx, requete, sizeof(requete)) == -1) {
		perror("read");
		exit(1);
	}

	printf("nom de fichier >>> ");
	fscanf(stdin, "%s", requete);

	if (write(sc_cnx, requete, sizeof(requete)) == -1) {
		perror("read");
		exit(1);
	}

	if (read(sc_cnx, requete, sizeof(requete)) == -1) {
		perror("read");
		exit(1);
	}
	printf("retour : %s\n",requete);
	if (strcmp(requete, "ERROR") == 0) {
		printf("nom de fichier inexistant %s", requete);
		exit(-1);
	}

	int f;
	if ((f = open(requete, O_WRONLY | O_CREAT | O_TRUNC, 0600)) == -1) {
		perror("open");
	}

	printf("telechargement\n");
	memset(requete, 0, 256);
	while (read(sc_cnx, requete, sizeof(requete)) > 0) {
		printf("r : %s\n", requete);
		if (write(f, requete, sizeof(requete)) == -1) {
			perror("read");
			exit(1);
		}
	}
	close(f);

}

int main(int argc, char *argv[]) {

	int sc_cnx;
	struct sockaddr_in dest; /* Nom du serveur */
	int fromlen = sizeof(dest);

	if (argc != 3) {
		printf("error syntaxe : %s 127.0.0.1 2000 \n", argv[0]);
		exit(1);
	}

	/* creation TCP socket */
	if ((sc_cnx = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}
	char *ip = argv[1];
	short port = (short) atoi(argv[2]);

	/* Remplir address dest */
	memset((char *) &dest, 0, sizeof(dest));
	if (inet_aton(ip, &(dest.sin_addr)) == 0) {
		fprintf(stdout, "invalide \n");
		exit(2);
	}
	dest.sin_family = AF_INET;
	dest.sin_port = htons(port);

	/* Etablir la connexion */
	if (connect(sc_cnx, (struct sockaddr *) &dest, sizeof(dest)) == -1) {
		perror("connect");
		exit(1);
	}

	/****** TRAITEMENT *********/
	memset(requete, 0, 256);
	memset(reponse, 0, 256);

	printf("ftp_client >>> ");
	fscanf(stdin, "%s", requete);

	if (strcmp(requete, LIST) == 0) {
		listFiles_cli(sc_cnx);
	} else if (strcmp(requete, UPLOAD) == 0) {
		upload_cli(sc_cnx);
	} else if (strcmp(requete, DOWNLOAD) == 0) {
		download_cli(sc_cnx);
	} else {
		printf("requete inconnu \n");
	}

	/* Fermer la connexion */
	shutdown(sc_cnx, 2);
	close(sc_cnx);
	return (0);
}
