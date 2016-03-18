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
#include "ricochet_robot.h"
#include "SDLS.h"

#define CHMUR 0x0001	/* case mur haut*/
#define CBMUR 0x0002 	/* case mur bas*/
#define CDMUR 0x0004 	/* case mur droit*/
#define CGMUR 0x0008 	/* case mur gauche*/
#define CROBOT 0x0010 	/* case contient robot*/
#define CSMUR 0x0020	/* case sans mur, case libre*/

#define CASE 32
#define NB_CASE 16
#define HEIGHT ( CASE * NB_CASE)
#define WIDTH ( CASE * NB_CASE)

void ihm();
void display_plateau(SDL_Renderer *ren, plateau_t pl);
void display_enigme(SDL_Renderer *ren, enigme_t *e);
#endif /* IHM_H_ */
