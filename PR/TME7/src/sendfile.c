#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define h_addr h_addr_list[0] /* definition du champs h_addr */

int main(int argc, char *argv[]) {
	struct sockaddr_in dest; /* Nom du serveur */
	int sock;
	int fromlen = sizeof(dest);
	int reponse;
	char buffer[1024];
	if (argc != 4) {
		printf("error syntaxe : %s 127.0.0.1 2000 file \n", argv[0]);
		exit(1);
	}
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}
	char *ip = argv[1];
	short port = (short) atoi(argv[2]);
	char* file = argv[3];

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

	memset(buffer, 0, sizeof(buffer));
	strcpy(buffer, file);
	if (write(sock, buffer, sizeof(buffer)) == -1) {
		perror("write");
		exit(1);
	}

	while (1) {
		printf("msg :");
		fscanf(stdin, "%s", buffer);
		if (write(sock, buffer, sizeof(buffer)) == -1) {
			perror("write");
			exit(1);
		}
		printf("envoye :%s\n", buffer);
	}

	printf("Reponse : %d\n", reponse);
	/* Fermer la connexion */
	shutdown(sock, 2);
	close(sock);
	return (0);
}
