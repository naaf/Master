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
#include <stdarg.h>
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

void send_request(int sc, int argc, ...) {
	char requete[512];
	memset(requete, 0, 512);

	//construction de requete
	int i;
	va_list ap;
	va_start(ap, argc);
	for (i = 0; i < argc; i++) {
		strcat(requete, va_arg(ap, char*));
		strcat(requete, "/");
	}
	strcat(requete, "\n");
	//send requete
	if (write(sc, requete, strlen(requete)) == -1) {
		perror("write");
		exit(1);
	}
	fprintf(stderr, "send %s %d oct \n", requete, strlen(requete));
}

char* read_response(int sc) {
	char response[512];
	memset(response, 0, 512);

	char c;
	int r, i = 0;
	while (1) {
		r = read(sc, &c, 1);
		if (r == 0 || c == '\n')
			break;
		response[i++] = c;
	}
	response[i] = c;
	int size = 0;
	char **tab = NULL;
	tab = string_to_arraystring(response, &size, '/');
	for (i = 0; i < size; i++) {
		printf("part%d : %s\n", i, tab[i]);
	}

	return NULL; //TODO
}
