#include<stdio.h>
#include<stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <string.h>

#define MUTEX 1
#define BAG_PHILO_DROITE (id_philo - 1 + nb_philo) % nb_philo
#define BAG_GAUCHE id_philo
#define PHILO_GAUCHE (id_philo + 1) % nb_philo
#define TAILLE_NOM 12

sem_t *sem_mutex;
sem_t **sem_philoblock;

char *adr_att;
int *tab_val_random;
int shm_id;
int taille = 0;

const char* sem_nom = "/sem_philo5";
char nom_sembloque[TAILLE_NOM];
const char *shm_nom = "/shm_philo";
int nb_philo, pense, dine, cycle;

void prepare_mange(int id_philo) {
	int peut_mange = 0;
	do {
		sem_wait(sem_mutex);
		if (tab_val_random[BAG_GAUCHE] == 0
				&& tab_val_random[BAG_PHILO_DROITE] == 0) {

			tab_val_random[BAG_GAUCHE] = 1;
			tab_val_random[BAG_PHILO_DROITE] = 1;
			peut_mange = 1;
			sem_post(sem_mutex);

		} else {
			sem_post(sem_mutex);
			sem_wait(&sem_philoblock[id_philo]);
		}

	} while (!peut_mange);

}
void depose(int id_philo) {

	sem_wait(sem_mutex);
	tab_val_random[BAG_GAUCHE] = 0;
	tab_val_random[BAG_PHILO_DROITE] = 0;
	sem_post(&sem_philoblock[BAG_PHILO_DROITE]);
	sem_post(&sem_philoblock[PHILO_GAUCHE]);
	sem_post(sem_mutex);
}

void process(int id_philo) {
	int i = 0;
	if ((adr_att = mmap(NULL, taille, PROT_READ | PROT_WRITE,
	MAP_SHARED, shm_id, 0)) == MAP_FAILED) {
		perror("mmap");
		exit(-1);
	}
	tab_val_random = (int*) adr_att;

	for (i = 0; i < cycle; i++) {
		printf("C%d : philo%d pense\n", i, id_philo);
		sleep(pense); /* pense */
		prepare_mange(id_philo);
		printf("C%d : philo%d mange\n", i, id_philo);
		sleep(dine);
		depose(id_philo);
	}
}

void nfork(int nb_philo) {
	printf("pere %d \n", getpid());
	int i;
	for (i = 1; i <= nb_philo; i++) {
		switch (fork()) {
		case 0: /* fils*/
			process(i);
			exit(EXIT_SUCCESS);
			break;
		default:/* pere*/

			break;
		}
	}
}

int main(int argc, char* argv[]) {
	int i;
	if (argc != 5) {
		printf("error syntaxe ex: %s 5 4 2 1\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	nb_philo = atoi(argv[1]);
	cycle = atoi(argv[2]);
	pense = atoi(argv[3]);
	dine = atoi(argv[4]);

	taille = nb_philo * sizeof(int);
	strcpy(nom_sembloque, "/blo_philo0");
	if ((sem_mutex = sem_open(sem_nom, O_CREAT | O_EXCL | O_RDWR, 0600,
	MUTEX)) == SEM_FAILED) {
		perror("sem_open");
		exit(1);
	}
	sem_philoblock = calloc(nb_philo, sizeof(sem_t*));
	for (i = 0; i < nb_philo; i++) {
		nom_sembloque[TAILLE_NOM - 2] = '1' + i;

		if ((sem_philoblock[i] = sem_open(nom_sembloque,
		O_CREAT | O_EXCL | O_RDWR, 0600,
		MUTEX)) == SEM_FAILED) {
			perror("sem_open");
			exit(1);
		}
	}

	if ((shm_id = shm_open(shm_nom, O_CREAT | O_RDWR, 0600)) == -1) {
		perror("shm_open");
		exit(-1);
	}
	if (ftruncate(shm_id, taille) == -1) {
		perror("ftruncate");
		exit(-1);
	}
	if ((adr_att = mmap(NULL, taille, PROT_READ | PROT_WRITE,
	MAP_SHARED, shm_id, 0)) == -1) {
		perror("mmap");
		exit(-1);
	}

	tab_val_random = (int*) adr_att;

	for (i = 0; i < nb_philo; i++) {
		tab_val_random[i] = 0;
	}

	nfork(nb_philo);

	if (sem_unlink(sem_nom) == -1) {
		perror("sem_unlink");
		exit(-1);
	}
	for (i = 0; i < nb_philo; i++) {
		nom_sembloque[TAILLE_NOM - 2] = '1' + i;
		if (sem_unlink(nom_sembloque) == -1) {
			perror("sem_unlink");
			exit(-1);
		}
	}
	if (shm_unlink(shm_nom) == -1) {
		perror("shm_unlink");
		exit(-1);
	}
	return EXIT_SUCCESS;
}
