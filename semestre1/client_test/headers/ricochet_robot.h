/*
 * ricochet_robot.h
 *
 *  Created on: 8 mars 2016
 *      Author: naaf
 */

#ifndef RICOCHET_ROBOT_H
#define RICOCHET_ROBOT_H

#define CHMUR 0x0001 /* case mur haut*/
#define CBMUR 0x0002 /* case mur bas*/
#define CDMUR 0x0004 /* case mur droit*/
#define CGMUR 0x0008 /* case mur gauche*/
#define CROBOT 0x0010 /* case contient robot*/
#define CSMUR 0x0020 /* case sans mur, case libre*/

typedef struct _list_user {
	char* name;
	int score;
	struct _list_user *next;

} list_user_t;

typedef int plateau_t[16][16];

typedef struct {
	int current_tour;
	list_user_t users;
} bilan_t;

typedef enum {
	Rouge = 'R', Bleu = 'B', Jaune = 'J', Vert = 'V'
} Color;

typedef enum {
	Haut = 'H', Bas = 'B', Gauche = 'G', Droit = 'D'
} Dirction;

typedef struct {
	char* username;
	char* enigme;
	int type;
	union {
		char* deplacements;
		plateau_t plateau;
		bilan_t bilan;
		int nb_coup;
	} content;
} response_t;

response_t parse(char* data);
char* encode(char* requete, response_t data);
char** string_to_arraystring(char* data, int* size, char separator);
void free_table(char** tab, int size);
void add_user(char* name, int score);
void remove_user(char* name);

#endif /* RICOCHET_ROBOT_H */
