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
#include "SDLS.h"


#define CASE 32
#define NB_CASE 16
#define HEIGHT ( CASE * NB_CASE)
#define WIDTH ( CASE * NB_CASE)

void ihm();
void display_plateau(SDL_Renderer *ren, plateau_t pl);
void display_enigme(SDL_Renderer *ren, enigme_t *e);
#endif /* IHM_H_ */
