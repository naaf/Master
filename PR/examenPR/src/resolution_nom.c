#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>

int ip_nom(char *ip) {
	struct sockaddr_in sin;
	char host[64];

	memset((void*) &sin, 0, sizeof(sin));
	sin.sin_addr.s_addr = inet_addr(ip);
	sin.sin_family = AF_INET;
	if (getnameinfo((struct sockaddr*) &sin, sizeof(sin), host, sizeof(host), 0,
			0, 0) != 0) {
		perror("getnameinfo");
		exit(EXIT_FAILURE);
	}
	printf("Name : %s\n", host);
	return EXIT_SUCCESS;
}

int nom_ip(char* nom_domaine) {
	struct addrinfo *result;

	if (getaddrinfo(nom_domaine, 0, 0, &result) != 0) {
		perror("getaddrinfo");
		exit(EXIT_FAILURE);
	}
	printf("address - %s\n",
			inet_ntoa(((struct sockaddr_in*) result->ai_addr)->sin_addr));
	freeaddrinfo(result); /* No longer needed */
	return (0);
}



int main(int argc, char **argv) {

	printf("127.0.0.1 to nom ");
	ip_nom("127.0.0.1");

	printf("8.8.8.8 to nom ");
	ip_nom("8.8.8.8");

	printf("localhost to ip ");
	nom_ip("localhost");

	return EXIT_SUCCESS;
}
