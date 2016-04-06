/*
 * ihm.h
 *
 *  Created on: 13 mars 2016
 *      Author: naaf
 */

#ifndef IHM_H_
#define IHM_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "ricochet_robot.h"
#include "communication.h"
#include "visualisation.h"
#include "SDLS.h"

#define CASE 32
#define NB_CASE 16
#define HEIGHT ( CASE * NB_CASE)
#define WIDTH ( CASE * NB_CASE)

#define FIN_CONNEXION 	0x0001
#define FIN_TOUR 		0x0002
#define FIN_SESSION 	0x0004

#define UPDATE_U		0x0020
#define UPDATE_L		0x0040
#define SIGALEMENT		0x0080
#define PHASE_SESSION	0x0100
#define PHASE_REFLEX	0x0200
#define PHASE_ENCHERE	0x0400
#define PHASE_RESO		0x0800

bool_t estDirection(enigme_t *e, SDL_MouseMotionEvent* p, int selected, int x,
		int y);
bool_t estContenu(SDL_Rect *rect, SDL_MouseMotionEvent* p);
void update_pos_robot(plateau_t p, robot_t *r, Direction d);

int awaitLoading();
int awaitLoadingTexte(char* msg, int attente);
void ihm();
void display_plateau(plateau_t pl);
void display_enigme(enigme_t *e);
void display_bilan(bilan_t *b);
void displayAccueil();
void displayCoup(SDL_Texture *tmp_Tx, SDL_Rect srcR, SDL_Rect *emptyR);
void displayMsg(char* msg, bool_t phase);
void erreur(char *msg, bool_t fin);
void updateView();

void onclickReset(plateau_t srcPl, enigme_t *srcE, char* coups, char* moves);
#endif /* IHM_H_ */
