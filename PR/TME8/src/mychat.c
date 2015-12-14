#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define MSGBUFSIZE 256

char *ip;
char *user;
short port;
int socket_cnx;

void *thread_lecture(void *arg) {
	int nbytes, addrlen;
	struct ip_mreq mreq;
	char message[MSGBUFSIZE];
	struct sockaddr_in addr;

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);

	/* bind to receive address */
	if (bind(socket_cnx, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		perror("bind");
		exit(1);
	}
	/* use setsockopt() to request that the kernel join a multicast group */
	mreq.imr_multiaddr.s_addr = inet_addr(ip);
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);

	if (setsockopt(socket_cnx, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq,
			sizeof(mreq)) < 0) {
		perror("setsockopt");
		exit(1);
	}

	/* now just enter a read-print loop */
	memset(message, 0, MSGBUFSIZE);
	while (1) {
		addrlen = sizeof(addr);
		if ((nbytes = recvfrom(socket_cnx, message, MSGBUFSIZE, 0,
				(struct sockaddr *) &addr, &addrlen)) < 0) {
			perror("recvfrom");
			exit(1);
		}
		puts(message);

	}
	return NULL;
}

void *thread_ecriture(void *arg) {

	char buffer[152] = { 0 };
	char message[256] = { 0 };
	struct sockaddr_in addr;

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(ip);
	addr.sin_port = htons(port);

	while (1) {
		printf("entrer msg : ");
		fgets(buffer, sizeof(buffer), stdin);
		strcat(message, user);
		strcat(message, " : ");
		strcat(message, buffer);

		if (sendto(socket_cnx, message, sizeof(message), 0,
				(struct sockaddr *) &addr, sizeof(addr)) < 0) {
			perror("sendto");
			exit(1);
		}
	}
	return NULL;
}

int main(int argc, char **argv) {

	pthread_t thread_lec, thread_ecr;

	if (argc != 4) {
		printf("error syntaxe %s \n", argv[0]);
		exit(EXIT_FAILURE);
	}

	ip = argv[1];
	port = (short) atoi(argv[2]);
	user = argv[3];

	if ((socket_cnx = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket");
		exit(1);
	}
	int yes = 1;

	if (setsockopt(socket_cnx, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes))
			< 0) {
		perror("Reusing ADDR failed");
		exit(1);
	}

	if (pthread_create(&thread_lec, NULL, thread_lecture, NULL) != 0) {
		perror("pthread_create \n");
		exit(1);
	}
	if (pthread_create(&thread_ecr, NULL, thread_ecriture, NULL) != 0) {
		perror("pthread_create \n");
		exit(1);
	}
	pthread_join(thread_lec, NULL);
	pthread_join(thread_ecr, NULL);
	return EXIT_SUCCESS;
}

