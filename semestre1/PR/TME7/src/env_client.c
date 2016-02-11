#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

int main(int argc, char *argv[]) {

	struct sockaddr_in dest;
	int sock;
	int fromlen = sizeof(dest);

	if (argc != 3) {
		printf("error syntaxe : %s @addr nbPort \n", argv[0]);
		exit(1);
	}

	/** creation socket udp **/
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	char *ip = argv[1];
	short PORTSERV = (short) atoi(argv[2]);

	/** init dest address**/
	memset((char *) &dest, 0, sizeof(dest));
	if (inet_aton(ip, &(dest.sin_addr)) == 0) {
		fprintf(stdout, "invalide \n");
		exit(2);
	}
	dest.sin_family = AF_INET;
	dest.sin_port = htons(PORTSERV);

	/*** traitement ***/
	char message[256];
	char reponse[256];
	int boucle = 1;
	char requete;
	memset(message, 0, 256);
	memset(reponse, 0, 256);

	do {

		printf("client_env >>> ");
		fgets(message, 256, stdin);

		sscanf(message, "%c", &requete);

		if (requete == 'Q' || requete == 'S' || requete == 'G') {
			/** envois a dest **/
			if (sendto(sock, message, strlen(message) + 1, 0,
					(struct sockaddr *) &dest, sizeof(dest)) == -1) {
				perror("sendto");
				exit(1);
			}
			if (requete == 'Q') {
				boucle = 0;

			} else if (requete == 'G') {
				/** recoit du dest **/
				if (recvfrom(sock, reponse, sizeof(reponse), 0,
						(struct sockaddr *) &dest, &fromlen) == -1) {
					perror("recvfrom");
					exit(1);
				}
				printf("Reponse : %s\n", reponse);
			}
		} else {
			printf("error requete inconnu\n");
		}

	} while (boucle);

	close(sock);
	return (0);
}
