
#define _XOPEN_SOUCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define TAILLE_FILE 100

int main(int argc, char **argv) {

	int f_in, f_out;
	char file_out[TAILLE_FILE];
	printf("file : %s \n", argv[1]);
	if ((f_in = open("test", O_RDONLY)) == -1) {
		perror("open");
	}
	memcpy(file_out, argv[1], strlen(argv[1]));
	strcat(file_out, "_inverse");

	if ((f_out = open(file_out, O_WRONLY | O_CREAT | O_TRUNC, 0600)) == -1) {
		perror("open");
	}

	char carac = '\0';
	off_t tete_lecture = lseek(f_in, 0, SEEK_END);
	tete_lecture -= 1;
	while (pread(f_in, &carac, 1, tete_lecture) > -1) {
		write(f_out, &carac, 1);
		tete_lecture -= 1;
	}

	close(f_in);
	close(f_out);
	puts("terminer\n");
	return EXIT_SUCCESS;
}
