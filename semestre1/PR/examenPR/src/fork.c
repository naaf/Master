#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

void process() {

}
void un_fork() {

	switch (fork()) {
	case -1: /* error*/
		perror("fork");
		exit(EXIT_FAILURE);
		break;

	case 0: /* fils*/
		printf("pere pid %d ==> fils pid %d\n", getppid(), getpid());

		/** traitement**/
		process();

		exit(EXIT_SUCCESS);
		break;
	default:/* pere*/

		break;
	}
}

void nfork(int nb_fils) {
	int i = 0;

	for (i = 0; i < nb_fils; i++) {
		switch (fork()) {
		case -1: /* error*/
			perror("fork");
			exit(EXIT_FAILURE);
			break;
		case 0: /* fils*/
			printf("pere pid %d ==> fils pid %d\n", getppid(), getpid());
			exit(EXIT_SUCCESS);
			break;
		default:/* pere*/

			break;
		}
	}
}

//int main(void) {
//	int nb_fils = 4;
//	un_fork(nb_fils);
//
//	int i;
//	for (i = 0; i < nb_fils; ++i) {
//		wait(NULL);
//	}
//
//	printf("fin main\n");
//	return EXIT_SUCCESS;
//}
