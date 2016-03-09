/*
 * communication.c
 *
 *  Created on: 8 mars 2016
 *      Author: naaf
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "../headers/communication.h"

int connex_socket(char* addrIp, int port) {
	struct sockaddr_in srv_addr;
	int srv_addr_len;
	int sock;

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	/* Remplir la structure srv_addr */
	srv_addr_len = sizeof(srv_addr);
	memset((char *) &srv_addr, 0, srv_addr_len);
	if (inet_aton(addrIp, &(srv_addr.sin_addr)) == 0) {
		fprintf(stdout, "invalide \n");
		exit(2);
	}
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port = htons(port);

	/* Etablir la connexion */
	if (connect(sock, (struct sockaddr *) &srv_addr, srv_addr_len) == -1) {
		perror("connect");
		exit(1);
	}

	return sock;
}
//TODO factorise creation de requete voir encode
void connex_session(int socket, char* nameUser) {
	char requete[256];
	memset(requete, 0, 256);
	strcat(requete, CONNEXION);
	strcat(requete, "/");
	strcat(requete, nameUser);
	strcat(requete, "/");
	if (write(socket, requete, strlen(requete)) == -1) {
		perror("write");
		exit(1);
	}
}

void deconnex_session(int socket, char* nameUser) {
	char requete[256];
	memset(requete, 0, 256);
	strcat(requete, SORT);
	strcat(requete, "/");
	strcat(requete, nameUser);
	strcat(requete, "/");
	if (write(socket, requete, strlen(requete)) == -1) {
		perror("write");
		exit(1);
	}
}

void tour_sendreflexion(int socket, char* user, int nbcoups) {

}
void enchere_sendcoups(int socket, char* user, int nbcoups) {

}
void tour_sendsolution(int socket, char* user, char* deplacements) {

}

