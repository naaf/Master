#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT_SRV 9990
#define PONG "PONG"

int main(int argc, char *argv[]) {

	struct sockaddr_in addr_ponger;
	struct sockaddr_in addr_corres;
	int socket_cli;
	int corres_len = sizeof(addr_corres);

	if ((socket_cli = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	memset((char *) &addr_ponger, 0, sizeof(addr_ponger));
	addr_ponger.sin_addr.s_addr = htonl(INADDR_ANY);
	addr_ponger.sin_family = AF_INET;
	addr_ponger.sin_port = htons(PORT_SRV);

	if (bind(socket_cli, (struct sockaddr *) &addr_ponger, sizeof(addr_ponger))
			< 0) {
		perror("bind");
		exit(2);
	}

	char reponse[256];
	int boucle = 1;

	memset(reponse, 0, 256);
	fprintf(stdout, "attente de ping ....\n");

	if (recvfrom(socket_cli, reponse, sizeof(reponse), 0,
			(struct sockaddr *) &addr_corres, &corres_len) == -1) {
		perror("recvfrom");
		exit(1);
	}
	printf("recu %s \n", reponse);
	if (sendto(socket_cli, PONG, strlen(PONG), 0,
			(struct sockaddr *) &addr_corres, sizeof(struct sockaddr_in)) < 0) {
		perror("sendto");
	}

	close(socket_cli);
	return (0);
}
