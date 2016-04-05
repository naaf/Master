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
#include "../headers/ihm.h"

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
	if (*size == 0) {
		return NULL;
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
void cpyEnigme(enigme_t* src, enigme_t *dst) {
	int i;
	dst->cible.c = src->cible.c;
	dst->cible.x = src->cible.x;
	dst->cible.y = src->cible.y;
	strcpy(dst->cible.path, src->cible.path);
	for (i = 0; i < NB_ROBOT; i++) {
		strcpy(dst->robots[i].path, src->robots[i].path);
		dst->robots[i].c = src->robots[i].c;
		dst->robots[i].x = src->robots[i].x;
		dst->robots[i].y = src->robots[i].y;
	}
}

void cpyPlateau(plateau_t plsrc, plateau_t pldst) {
	int i, j;
	for (i = 0; i < NB_CASE; i++) {
		for (j = 0; j < NB_CASE; j++) {
			pldst[i][j] = plsrc[i][j];
		}
	}
}
void bind_enigme_plateau(plateau_t p, enigme_t *e) {
	int i;
	for (i = 0; i < NB_ROBOT; ++i) {
		p[e->robots[i].x][e->robots[i].y] |= CROBOT;
	}
	p[e->cible.x][e->cible.y] |= CCIBLE;
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
			if (y != 0)
				plateau[x][y - 1] |= CBMUR;
			break;
		case Bas:
			plateau[x][y] |= CBMUR;
			if (y != NB_CASE - 1)
				plateau[x][y + 1] |= CHMUR;
			break;
		case Gauche:
			plateau[x][y] |= CGMUR;
			if (x != 0)
				plateau[x - 1][y] |= CDMUR;
			break;
		case Droit:
			plateau[x][y] |= CDMUR;
			if (x != NB_CASE - 1)
				plateau[x + 1][y] |= CGMUR;
			break;
		default:
			fprintf(stderr, "ERREUR Parse CASE %d %d type %c -> inconnu\n", x,
					y, c);
			break;
		}
		i += nb_carac_separator + (x / 10 + 1) + (y / 10 + 1) + 1;
	}
}

void parse_enigme(char* data, enigme_t *enig) {

	if (strlen(data) < 13) {
		return;
	}

	sscanf(data, "(%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%c)", &enig->robots[ROBOT_R].x,
			&enig->robots[ROBOT_R].y, &enig->robots[ROBOT_B].x,
			&enig->robots[ROBOT_B].y, &enig->robots[ROBOT_J].x,
			&enig->robots[ROBOT_J].y, &enig->robots[ROBOT_V].x,
			&enig->robots[ROBOT_V].y, &enig->cible.x, &enig->cible.y,
			&enig->cible.c);

	enig->robots[ROBOT_R].c = Rouge;
	sprintf(enig->robots[ROBOT_R].path, "%srobotr%s", ASSETS,
	EXT_IMG);

	enig->robots[ROBOT_B].c = Bleu;
	sprintf(enig->robots[ROBOT_B].path, "%srobotb%s", ASSETS,
	EXT_IMG);

	enig->robots[ROBOT_J].c = Jaune;
	sprintf(enig->robots[ROBOT_J].path, "%srobotj%s", ASSETS,
	EXT_IMG);

	enig->robots[ROBOT_V].c = Vert;
	sprintf(enig->robots[ROBOT_V].path, "%srobotv%s", ASSETS,
	EXT_IMG);

	sprintf(enig->cible.path, "%scible%c%s", ASSETS, enig->cible.c,
	EXT_IMG);

}

void parse_bilan(char* data, bilan_t* bil) {
	int offset, score, len, i;
	char buffer[NAME_SIZE], c;
	len = strlen(data);

	if (len < 11)
		return;
	if (bil == NULL)
		return;
	if (bil->list_users.first != NULL)
		free_list_user(&bil->list_users);
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

			if (getuser(buffer, &bil->list_users) == NULL) {
				adduser(buffer, score, &(bil->list_users));
			}

			memset(buffer, 0, NAME_SIZE);
			break;
		}
		if (cnom) {
			buffer[i++] = c;
		}
	}
}

int adduser(char* name, int score, list_user_t* list_users) {
	if (getuser(name, list_users)) {
		return 0;
	}
	user_t *new = (user_t*) malloc(sizeof(user_t));
	strncpy(new->name, name, NAME_SIZE);
	new->score = score;
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
	while (cur != NULL && list_users->nb > 0) {
		pr = cur;
		cur = cur->next;
		free_user(pr);
	}
	list_users->first = NULL;
}

