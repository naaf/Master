#define _XOPEN_SOURCE 700

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <aio.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#define BUF_SIZE 10

int main(int argc, char *argv[]) {

	if (argc != 3) {
		printf("error syntaxe :%s  source.txt dest.txt \n", argv[0]);
		exit(EXIT_FAILURE);
	}
	const char* source_file = argv[1];
	const char* dest_file = argv[2];
	int desc_source;
	int desc_dest;

	struct aiocb cb_ecr[BUF_SIZE];
	struct aiocb * lio[BUF_SIZE];
	struct sigevent lio_sigev;

	char buffer[BUF_SIZE] = { 0 };

	if ((desc_source = open(source_file, O_RDONLY | 0666)) < 0) {
		perror("open source_file");
		exit(EXIT_FAILURE);
	}

	if ((desc_dest = open(dest_file, O_CREAT | O_RDWR | O_TRUNC, 0666)) < 0) {
		perror("open dest_file");
		exit(EXIT_FAILURE);
	}

	int i;
	for (i = 0; i < BUF_SIZE; ++i) {
		/**  init l'ecriture **/
		cb_ecr[i].aio_fildes = desc_dest;
		cb_ecr[i].aio_lio_opcode = LIO_WRITE;
		cb_ecr[i].aio_nbytes = 1;
		cb_ecr[i].aio_reqprio = 0;
		cb_ecr[i].aio_sigevent.sigev_notify = SIGEV_NONE;
	}

	lio_sigev.sigev_notify = SIGEV_NONE;
	long int octet_lus = 0;
	int nb_tour = 0;
	while ((octet_lus = read(desc_source, buffer, BUF_SIZE)) > 0) {
		printf("buffer lu %ld \n", octet_lus);
		for (i = 0; i < octet_lus; ++i) {
			printf("%c \n", buffer[octet_lus - i - 1]);
			cb_ecr[i].aio_buf = &buffer[octet_lus - i - 1];
			cb_ecr[i].aio_offset = nb_tour * BUF_SIZE + i;
			lio[i] = &cb_ecr[i];
		}
		if (lio_listio(LIO_WAIT, lio, octet_lus, &lio_sigev) < 0) {
			perror("lio_listio");
			exit(EXIT_FAILURE);
		}
		nb_tour++;
	}

	close(desc_source);
	close(desc_dest);
	return EXIT_SUCCESS;
}
