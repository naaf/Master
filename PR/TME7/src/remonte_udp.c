#define _SVID_SOURCE 1

#define _XOPEN_SOURCE 700
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <signal.h>

char *file;

void handler(int sig) {
	unlink(file);
}

void process(const char *file) {
	int sock;
	struct sockaddr_un name;

	sock = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (sock < 0) {
		perror("ouverture de socket");
		exit(1);
	}

	name.sun_family = AF_UNIX;
	strcpy(name.sun_path, file);

	int val_rand = 0;
	srand(getpid());
	val_rand = (int) (10 * (float) rand() / RAND_MAX);
	printf("fils pid %d ==> %d\n", getpid(), val_rand);

	/* Send message. */
	if (sendto(sock, &val_rand, sizeof(val_rand), 0, &name,
			sizeof(struct sockaddr_un)) < 0) {
		perror("sending datagram message");
	}
	close(sock);

}

void nfork(int nb_fils, const char *file) {
	printf("pere %d \n", getpid());
	int i = 0;

	for (i = 0; i < nb_fils; i++) {
		switch (fork()) {
		case 0: /* fils*/
			process(file);
			exit(EXIT_SUCCESS);
			break;
		default:/* pere*/

			break;
		}
	}
}

int main(int argc, char* argv[]) {
	int nb_fils, i;

	if (argc < 3) {
		printf("error syntaxe : %s sockudp 4 \n", argv[0]);
		exit(1);
	}
	signal(SIGINT, handler);
	/* serveur */
	nb_fils = atoi(argv[2]);
	file = argv[1];

	int result = 0;
	int data = 0;

	int sock, length;
	struct sockaddr_un name;

	/* Create socket from which to read. */
	sock = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (sock < 0) {
		perror("opening datagram socket");
		exit(1);
	}

	/* Create name. */
	name.sun_family = AF_UNIX;
	strcpy(name.sun_path, file);

	/* Bind the UNIX domain address to the created socket */
	if (bind(sock, (struct sockaddr *) &name, sizeof(struct sockaddr_un))) {
		perror("binding name to datagram socket");
		exit(1);
	}
	printf("socket -->%s\n", file);

	/* fils */
	nfork(nb_fils, file);

	/* Read from the socket */
	for (i = 0; i < nb_fils; i++) {
		if (read(sock, &data, sizeof(data)) < 0)
			perror("receiving datagram packet");
		result += data;
	}
	for (i = 0; i < nb_fils; i++) {
		wait(NULL);
	}
	printf("pere %d : somme %d  \n", getpid(), result);

	close(sock);
	unlink(file);
	return EXIT_SUCCESS;
}
