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

const char* file = "test.txt";

void f_lire() {
	char buffer[255] = { 0 };
	FILE* f = fopen(file, "r");
	if (f == NULL) {
		perror("fopen");
	}
	fread(buffer, 1, 255, f);
	fprintf(stdout, "%s", buffer);

	/**deuxieme lecture **/
	fseek(f, 0, SEEK_SET);
	char c;
	while ((c = fgetc(f)) != EOF) {
		printf("%c", c);
	}

	fclose(f);
}

void list_file() {
	DIR *pt_Dir;
	struct dirent* dirEnt;
	char *name = NULL;
	char* buff_path = ".";
	struct stat stat_info;

	if ((pt_Dir = opendir(buff_path)) == NULL) {
		perror("erreur opendir \n");
		exit(1);
	}

	/* lire répertoire */
	while ((dirEnt = readdir(pt_Dir)) != NULL) {
		name = dirEnt->d_name;
		if (stat(name, &stat_info) < 0) {
			perror("stat");
		}
		if (S_ISREG(stat_info.st_mode)) {
			printf("nom ==> %s \n", name);
		}
	}

	closedir(pt_Dir);
}

void f_ecrire() {
	char* texte = "test d'ecriture de texte ....";
	FILE* f = fopen(file, "w");
	if (f == NULL) {
		perror("fopen");
	}
	fprintf(f, "%s\n", texte);

	fclose(f);
}

//int main(void) {
//	printf("STDIN_FILENO  %d \n", STDIN_FILENO);
//	f_ecrire();
//	f_lire();
//	list_file();
//	return EXIT_SUCCESS;
//}
