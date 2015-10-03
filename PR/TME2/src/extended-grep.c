#define _XOPEN_SOUCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <stdbool.h>

extern int errno;

#define _POSIX_SOUCE 1
#define TAILLE_PATH 100

#define SIZE_BUFFER 256

char buff_path[TAILLE_PATH];
DIR *pt_Dir;
struct dirent* dirEnt;

bool find_expression(const char* expression, const char *file_name) {
	char buffer[SIZE_BUFFER];
	char file_path[SIZE_BUFFER];

	memset(file_path, '\0', SIZE_BUFFER);
	memset(buffer, '\0', SIZE_BUFFER);

	strcat(file_path, buff_path);
	strcat(file_path, file_name);


	FILE* f = fopen(file_path, "r");
	bool found = false;

	while (fgets(buffer, SIZE_BUFFER, f) != NULL) {
		if (strstr(buffer, expression) != NULL) {
			found = true;
			printf("%s \n", file_name);
			break;
		}
	}

	fclose(f);
	return found;
}

int main(int argc, char* argv[]) {
	const char* error_not_found = "Aucun fichier valide";
	bool found_one = false;
	if (argc < 3) {
		/* repertoir courant : obtenir le nom */
		fprintf(stderr, "error : extended-grep <expr> <chemin>");
		exit(1);

	} else {

		int len = strlen(argv[1]);
		if (TAILLE_PATH <= len) {
			fprintf(stderr, "taille chemin est trop grand");
			exit(1);
		}
		memcpy(buff_path, argv[1], strlen(argv[1]));
		if (buff_path[len - 1] != '/') {
			buff_path[len] = '/';
			buff_path[len + 1] = '\0';
		}

	}

	if ((pt_Dir = opendir(buff_path)) == NULL) {
		if (errno == ENOENT) {
			/* repertoire n'existe pas - créer le répertoire */
			if (mkdir(buff_path, S_IRUSR | S_IWUSR | S_IXUSR) == -1) {
				perror("erreur mkdir\n");
				exit(1);
			} else
				return 0;
		} else {
			perror("erreur opendir \n");
			exit(1);
		}
	}

	/* lire répertoire */
	while ((dirEnt = readdir(pt_Dir)) != NULL) {
		found_one = find_expression(argv[2], dirEnt->d_name) || found_one;
	}

	if (found_one == false) {
		printf("%s \n", error_not_found);
	}
	closedir(pt_Dir);

	return 0;
}
