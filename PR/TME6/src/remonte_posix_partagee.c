#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

char *adr_att;
int *p_int;
int shm_id;
int taille = 0;

void remonte_partagee(int nb_fils) {

	int indice_fils = 1, val_rand = 0;
	printf("pere %d nb_fils %d \n", getpid(), nb_fils);
	for (indice_fils = 1; indice_fils <= nb_fils; indice_fils++) {
		switch (fork()) {
		case -1:
			perror("fork");
			exit(EXIT_FAILURE);
		case 0:
			/* fils*/

			srand(getpid());
			val_rand = (int) (10 * (float) rand() / RAND_MAX);

			if ((adr_att = mmap(NULL, taille, PROT_READ | PROT_WRITE,
					MAP_SHARED, shm_id, 0)) == -1) {
				perror("mmap");
				exit(-1);
			}

			p_int = (int*) adr_att;
			p_int[indice_fils] = val_rand;
			printf("fils %d, pid %d ==> %d\n", indice_fils, getpid(), val_rand);
			if (indice_fils == nb_fils) {
				p_int[0] = -1;
			}
//			TODO detache
			exit(EXIT_SUCCESS);
			break;
		default:/* pere*/
			break;
		}
	}
}
int main(int argc, char* argv[]) {

	key_t cle;



	int nb_fils, i;

	if (argc != 2) {
		printf("%derror syntaxe : %s nbProcessus \n", argc, argv[0]);
		exit(1);
	}

	nb_fils = atoi(argv[1]);
	/*------------------------------------------------------*/
	/* creation segment memoir	*/
	/*------------------------------------------------------*/

	taille = (nb_fils + 1) * sizeof(int);

	char *shm_nom = "/remonte_posix_partagee";
	if ((shm_id = shm_open(shm_nom, O_CREAT | O_RDWR, 0600)) == -1) {
		perror("shm_open");
		exit(-1);
	}
	if (ftruncate(shm_id, taille) == -1) {
		perror("ftruncate");
		exit(-1);
	}

	if ((adr_att = mmap(NULL, taille, PROT_READ | PROT_WRITE, MAP_SHARED,
			shm_id, 0)) == MAP_FAILED) {
		perror("mmap");
		exit(-1);
	}

	p_int = (int*) adr_att;
	p_int[0] = 0;

	/*------------------------------------------------------*/
	/* traitement liberation memoir partage	*/
	/*------------------------------------------------------*/
	remonte_partagee(nb_fils);
	while (p_int[0] == 0) {
		printf("attend while %d \n", p_int[0]);
		sleep(1);
	}

	int result = 0;
	for (i = 1; i < nb_fils + 1; i++) {

		printf("pere from fils %d ===> %d \n", i, p_int[i]);
		result += p_int[i];
	}
	printf("pere %d : la somme est %d \n", getpid(), result);

	if (shm_unlink(shm_nom) == -1) {
		perror("shm_unlink");
		exit(-1);
	}

	return EXIT_SUCCESS;
}
