#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>

#define PORT_SRV 9999
#define PING "PING"


int socket_srv;

void traitement() {
	/* remplir addr correspondant */
	struct sockaddr_in addr_corres;
	memset(&addr_corres, '\0', sizeof(struct sockaddr_in));
	addr_corres.sin_family = AF_INET;
	addr_corres.sin_port = (in_port_t) htons(9990);

	addr_corres.sin_addr.s_addr = htonl(INADDR_BROADCAST);
	/*** Traitement ***/
	if (sendto(socket_srv, PING, strlen(PING), 0,
			(struct sockaddr *) &addr_corres, sizeof(struct sockaddr_in)) < 0) {
		perror("sendto");
	}
	printf("sent\n");
}

void* func_emmeteur(void* arg) {
	printf("thread_emmeteur demarrer\n");
	sigset_t ens;
	int sig = 0;
	alarm(3);
	sigemptyset(&ens);
	sigaddset(&ens, SIGALRM);
	printf("attent...");
	while (1) {
		sigwait(&ens, &sig);
		if (sig == SIGALRM) {
			// traitement
			traitement();
			alarm(3);
		}
	}
	return NULL;
}

void* func_ecoute(void* arg) {
	printf("thread_ecoute demarrer\n");
	struct sockaddr_in addr_corres;
	int corres_len = sizeof(addr_corres);
	char message[256];
	memset(message, 0, 256);

	while (1) {
		/*** Reception du message ***/
		if (recvfrom(socket_srv, message, sizeof(message), 0,
				(struct sockaddr *) &addr_corres, &corres_len) == -1) {
			perror("recvfrom");
			exit(2);
		}
		fprintf(stdout, "IP = %s,  \n", inet_ntoa(addr_corres.sin_addr));
	}

	return NULL;
}

int main(int argc, char *argv[]) {

	struct sockaddr_in addr_srv;

	/* creation de la socket */
	if ((socket_srv = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket");
		exit(1);
	}

	/** option de socket*/
	int autorisation;
	autorisation = 1;
	setsockopt(socket_srv, SOL_SOCKET, SO_BROADCAST, &autorisation,
			sizeof(autorisation));

	/* remplir addr srv */
	memset((char *) &addr_srv, 0, sizeof(addr_srv));
	addr_srv.sin_addr.s_addr = htonl(INADDR_ANY);
	addr_srv.sin_port = htons(PORT_SRV);
	addr_srv.sin_family = AF_INET;

	/* nommage */
	if (bind(socket_srv, (struct sockaddr *) &addr_srv, sizeof(addr_srv)) < 0) {
		perror("bind");
		exit(2);
	}

	printf("serveur sonar >>>\n");

	/** gestion des threads **/

	pthread_t tid_ecoute;
	pthread_t tid_emmeteur;
	sigset_t ens;
	sigfillset(&ens);
	sigdelset(&ens, SIGINT);
	sigprocmask(SIG_SETMASK, &ens, NULL);

	if (pthread_create(&tid_ecoute, NULL, func_ecoute, NULL) != 0) {
		perror("pthread_create \n");
		exit(1);
	}
	if (pthread_create(&tid_emmeteur, NULL, func_emmeteur, NULL) != 0) {
		perror("pthread_create \n");
		exit(1);
	}

	if (pthread_join(tid_ecoute, NULL) != 0) {
		perror("pthread_join");
		exit(1);
	}
	if (pthread_join(tid_emmeteur, NULL) != 0) {
		perror("pthread_join");
		exit(1);
	}

	close(socket_srv);
	return (0);
}

