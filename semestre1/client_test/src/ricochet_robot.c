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

void init_plateau(plateau_t plateau) {
	int i, j;
	for (i = 0; i < 16; ++i) {
		for (j = 0; j < 16; ++j) {
			plateau[i][j] = 0;
		}
	}
}

void parse_plateau(char* data, plateau_t plateau) {
	int i, x, y, len;
	char c;
	len = strlen(data);
	if (len < 7)
		return;
	i = 0;
	len--;
	while (i < len) {

		sscanf(data + i, "(%d,%d,%c)", &x, &y, &c);
		switch (c) {
		case Haut:
			plateau[x][y] |= CHMUR;
			break;
		case Bas:
			plateau[x][y] |= CBMUR;
			break;
		case Gauche:
			plateau[x][y] |= CGMUR;
			break;
		case Droit:
			plateau[x][y] |= CDMUR;
			break;
		}
		i += 7;
	}
}

void parse_enigme(char* data, enigme_t *enig) {
	int i, x, y;
	char c;

	if (strlen(data) < 33) {
		return;
	}
	data++;
	for (i = 0; i < NB_ROBOT + 1; i++) {
		sscanf(data + i * 6, "%d%c,%d%c,", &x, &c, &y, &c);
		switch (c) {
		case 'r':
			enig->robots[ROBOT_R].x = x;
			enig->robots[ROBOT_R].y = y;
			break;
		case 'b':
			enig->robots[ROBOT_B].x = x;
			enig->robots[ROBOT_B].y = y;
			break;
		case 'j':
			enig->robots[ROBOT_J].x = x;
			enig->robots[ROBOT_J].y = y;
			break;
		case 'v':
			enig->robots[ROBOT_V].x = x;
			enig->robots[ROBOT_V].y = y;
			break;
		case 'c':
			enig->cible.x = x;
			enig->cible.y = y;
			i++;
			sscanf(data + i * 6, "%c", &(enig->cible.c));
			break;
		}
	}
}

void parse_bilan(char* data, bilan_t* bil){

}
