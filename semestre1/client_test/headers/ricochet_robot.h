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

#define NAME_SIZE 256

typedef struct _user {
	char name[NAME_SIZE];
	int score;
	int nb_coups;
	char* solution;
	struct _user *next;

} user_t;

typedef struct {
	int nb;
	user_t* first;
} list_user_t;

typedef int plateau_t[16][16];

typedef struct {
	int current_tour;
	list_user_t list_users;
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
	char c;
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
void parse_plateau(char* data, plateau_t plateau);
void parse_enigme(char* data, enigme_t *enig);
void parse_bilan(char* data, bilan_t* bil);

char** string_to_arraystring(char* data, int* size, char separator);
void free_table(char** tab, int size);

int adduser(char* name, list_user_t* list_users);
int removeuser(char* name, list_user_t* list_users);
user_t* getuser(char* name, list_user_t* list_users);
void free_user(user_t *user);
void free_list_user(list_user_t* list_users);

#endif /* RICOCHET_ROBOT_H */
