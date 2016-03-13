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
	const int nb_carac_separator = 4;
	len = strlen(data);
	if (len < 7)
		return;
	i = 0;
	len--;
	while (i < len) {

		sscanf(data + i, "(%d,%d,%c)", &x, &y, &c);
//		printf("(%d,%d,%c)\n", x, y, c); // DEBUG
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
		i += nb_carac_separator + (x / 10 + 1) + (y / 10 + 1) + 1;
	}
}

void parse_enigme(char* data, enigme_t *enig) {
	int i, x, y, offset;
	char c;

	if (strlen(data) < 33) {
		return;
	}

	const int nb_carac_separator = 2;
	for (i = 0, offset = 1; i < NB_ROBOT + 1; i++) {
		sscanf(data + offset, "%d%c,%d%c,", &x, &c, &y, &c);
//		printf("%d%c,%d%c,\n", x, c, y, c); // DEBUG
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
			offset += nb_carac_separator + (x / 10 + 1) + (y / 10 + 1) + 2;
			sscanf(data + offset, "%c", &enig->cible.c);
//			printf("color %c\n", enig->cible.c); // DEBUG
			break;
		}
		offset += nb_carac_separator + (x / 10 + 1) + (y / 10 + 1) + 2;
	}

}

void parse_bilan(char* data, bilan_t* bil) {
	int offset, score, len, i;
	char buffer[NAME_SIZE], c;
	len = strlen(data);

	if (len < 11)
		return;

	sscanf(data, "%d", &(bil->current_tour));
	memset(buffer, 0, NAME_SIZE);
	int cnom;
	c = cnom = i = 0;
	for (offset = 1; offset < len; offset++) {
		sscanf(data + offset, "%c", &c);
		switch (c) {
		case '(':
			cnom = 1;
			i = 0;
			continue;
			break;
		case ',':
			cnom = 0;
			sscanf(data + offset, ",%d", &score);

			if (getuser(buffer, &bil->list_users) != NULL) {
				printf("mod %s %d\n", buffer, score);
			} else {
				adduser(buffer, &(bil->list_users));
				printf("cre %s %d\n", buffer, score);
			}

			memset(buffer, 0, NAME_SIZE);
			break;
		}
		if (cnom) {
			buffer[i++] = c;
		}
	}
}

int adduser(char* name, list_user_t* list_users) {
	if (getuser(name, list_users)) {
		return 0;
	}
	user_t *new = (user_t*) malloc(sizeof(user_t));
	strncpy(new->name, name, NAME_SIZE);
	new->score = 0;
	new->nb_coups = 0;
	new->solution = NULL;

	new->next = list_users->first;
	list_users->first = new;
	list_users->nb += 1;
	return 1;
}
int removeuser(char* name, list_user_t* list_users) {
	if (list_users->nb == 0)
		return 0;
	user_t *pr, *cur;
	cur = list_users->first;
	pr = NULL;
	while (cur != NULL) {
		if (!strcmp(name, cur->name)) {
			if (pr == NULL) {
				list_users->first = cur->next;
				free_user(cur);
			} else {
				pr->next = cur->next;
				free_user(cur);
			}
			list_users->nb -= 1;
			return 1;
		}
		pr = cur;
		cur = cur->next;
	}
	return 0;
}
user_t* getuser(char* name, list_user_t* list_users) {
	if (list_users->nb == 0)
		return NULL;
	user_t *cur;
	cur = list_users->first;

	while (cur != NULL && strcmp(name, cur->name))
		cur = cur->next;
	return cur;
}
void free_user(user_t *user) {
	if (user->solution)
		free(user->solution);
	free(user);
}
void free_list_user(list_user_t* list_users) {
	user_t *pr, *cur;
	cur = list_users->first;
	pr = NULL;
	while (cur != NULL) {
		pr = cur;
		cur = cur->next;
		free_user(pr);
	}
	list_users->first = NULL;
}

