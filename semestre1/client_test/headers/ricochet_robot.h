/*
 * ricochet_robot.h
 *
 *  Created on: 8 mars 2016
 *      Author: naaf
 */

#ifndef RICOCHET_ROBOT_H
#define RICOCHET_ROBOT_H

#define CHMUR 0x0001	/* case mur haut*/
#define CBMUR 0x0002 	/* case mur bas*/
#define CDMUR 0x0004 	/* case mur droit*/
#define CGMUR 0x0008 	/* case mur gauche*/
#define CROBOT 0x0010 	/* case contient robot*/
#define CSMUR 0x0020	/* case sans mur, case libre*/

#define CBILAN 	0x0010 	/* type bilan*/
#define CENIGME 0x0002 	/* type enigme*/
#define CDEP 	0x0004 	/* type deplacement*/
#define CNAME 	0x0001 	/* type name */
#define CNB_CP	0x0020 	/* type nb_coup*/
#define CPLATE	0x0008 	/* type plateau*/

#define NB_ROBOT 4
#define ROBOT_B 0
#define ROBOT_J 1
#define ROBOT_R 2
#define ROBOT_V 3

typedef struct _list_user {
	char* name;
	char* score;
	char* nb_coups;
	char* solution;
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
	int x;
	int y;
} robot_t;

typedef struct {
	int x;
	int y;
	Color c;
} cible_t;

typedef struct {
	robot_t robots[4];
	cible_t cible;
} enigme_t;

typedef struct {
	int type;
	char* username;
	enigme_t enigme;
	union {
		char* deplacements;
		plateau_t plateau;
		bilan_t bilan;
		char* nb_coup;
	} content;
} response_t;

response_t parse(char* data);
char** string_to_arraystring(char* data, int* size, char separator);
void free_table(char** tab, int size);

void add_user(char* name, list_user_t users);
void remove_user(char* name, list_user_t users);
char* getuser(char* name, list_user_t users);

#endif /* RICOCHET_ROBOT_H */
