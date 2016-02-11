#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define port 12345
#define ip "225.0.0.37"

main(int argc, char *argv[]) {
	struct sockaddr_in addr;
	int sc_cnx, cnt;
	struct ip_mreq mreq;
	char* message = "Hello,world!";

	/* creation UDP socket */
	if ((sc_cnx = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket");
		exit(1);
	}

	/* init destination address */
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(ip);
	addr.sin_port = htons(port);

	/* now just sendto() our destination! */
	while (1) {
		if (sendto(sc_cnx, message, sizeof(message), 0,
				(struct sockaddr *) &addr, sizeof(addr)) < 0) {
			perror("sendto");
			exit(1);
		}
		sleep(1);
	}
}
