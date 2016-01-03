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
#define MSGBUFSIZE 512

main(int argc, char *argv[]) {

	struct sockaddr_in dest; /* address dest */
	int sc_cnx, nbytes, fromlen;
	struct ip_mreq mreq;
	char msgbuf[MSGBUFSIZE];
	int vrai = 1;

	/* creation UDP socket */
	if ((sc_cnx = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket");
		exit(1);
	}

	/**** OPTION SOCKET ******/

	/*autoriser la reutilisation de port */
	if (setsockopt(sc_cnx, SOL_SOCKET, SO_REUSEADDR, &vrai, sizeof(vrai)) < 0) {
		perror("Reusing ADDR failed");
		exit(1);
	}
	/*** END OPTION */

	/** init dest address**/
	memset(&dest, 0, sizeof(dest));
	dest.sin_family = AF_INET;
	dest.sin_addr.s_addr = htonl(INADDR_ANY); /* N.B.: differs from sender */
	dest.sin_port = htons(port);

	/* bind to receive address */
	if (bind(sc_cnx, (struct sockaddr *) &dest, sizeof(dest)) < 0) {
		perror("bind");
		exit(EXIT_FAILURE);
	}

	/*  setsockopt() rejoindre un multicast group */
	mreq.imr_multiaddr.s_addr = inet_addr(ip);
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	if (setsockopt(sc_cnx, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq))
			< 0) {
		perror("setsockopt");
		exit(1);
	}

	/* lecture des news */
	memset(msgbuf, 0, MSGBUFSIZE);
	while (1) {
		fromlen = sizeof(dest);
		if ((nbytes = recvfrom(sc_cnx, msgbuf, MSGBUFSIZE, 0,
				(struct sockaddr *) &dest, &fromlen)) < 0) {
			perror("recvfrom");
			exit(1);
		}
		puts(msgbuf);

	}
}
