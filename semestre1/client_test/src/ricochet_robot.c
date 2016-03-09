/*
 ============================================================================
 Name        : ricochet_robot.c
 Author      : naaf
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/ricochet_robot.h"



char** string_to_arraystring(char* data, int* size, char separator) {
	int i, cptLettre, lenght, pos;
	lenght = strlen(data);
	char** tab;
	pos = 0;
	*size = 0;
	for (i = 0; i < lenght; ++i) {
		if (data[i] == separator) {
			*size = *size + 1;
		}
	}

	tab = malloc((*size) * sizeof(char*));
	*size = 0;
	for (i = 0, cptLettre = 1; i < lenght; ++i, cptLettre++) {
		if (data[i] == separator) {
			if (cptLettre > 0) {
				tab[*size] = (char*) malloc(cptLettre);
				strncpy(tab[*size], data + pos, cptLettre - 1);
				tab[*size][cptLettre - 1] = 0;
				*size = *size + 1;
				pos = i + 1;
				cptLettre = 0;
			}

		}
	}

	return tab;
}

void free_table(char** tab, int size) {
	int i;
	for (i = 0; i < size; ++i) {
		free(tab[i]);
	}
	free(tab);
}


response_t parse(char* data) {
	//TODO
}

