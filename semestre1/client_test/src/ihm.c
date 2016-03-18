/*
 * ihm.c
 *
 *  Created on: 13 mars 2016
 *      Author: naaf
 */
#include <stdio.h>
#include <stdlib.h>

#include "../headers/ihm.h"

#define MAP "assets/map.bmp"

void display_plateau(SDL_Renderer *ren, plateau_t pl) {

	Uint8 r, g, b, a;
	int i, j;

	r = 255;
	b = 255;
	g = 0;
	a = 255;

	SDLS_affiche_image(MAP, ren, 0, 0);
	SDL_SetRenderDrawColor(ren, r, g, b, a);

	for (i = 0; i < NB_CASE; i++) {
		for (j = 0; j < NB_CASE; j++) {

			if (pl[i][j] & CHMUR)
				SDL_RenderDrawLine(ren, i * CASE, j * CASE, (1 + i) * CASE,
						j * CASE);
			if (pl[i][j] & CBMUR)
				SDL_RenderDrawLine(ren, i * CASE, (1 + j) * CASE,
						(1 + i) * CASE, (1 + j) * CASE);
			if (pl[i][j] & CDMUR)
				SDL_RenderDrawLine(ren, (i + 1) * CASE, j * CASE,
						(1 + i) * CASE, (j + 1) * CASE);
			if (pl[i][j] & CGMUR)
				SDL_RenderDrawLine(ren, i * CASE, j * CASE, i * CASE,
						(j + 1) * CASE);

		}
	}
	SDL_RenderPresent(ren);

}

void display_enigme(SDL_Renderer *ren, enigme_t *e) {
	int i;
	SDL_Texture * tex;
	for (i = 0; i < NB_ROBOT; i++) {
		printf(" r %s %d %d \n", e->robots[i].path, e->robots[i].x,
				e->robots[i].y);
		tex = IMG_LoadTexture(ren, e->robots[i].path);
		SDLs_dessin(tex, ren, e->robots[i].x * CASE, e->robots[i].y * CASE);
	}
	tex = IMG_LoadTexture(ren, e->cible.path);
	SDLs_dessin(tex, ren, e->cible.x * CASE, e->cible.y * CASE);
	SDL_RenderPresent(ren);
}

void aff() {
	while (1) {
		printf(".");
		sleep(1);
		fflush(stdout);
	}
}
int ihm1() {
	SDL_Window *win = 0;
	SDL_Renderer *ren = 0;
	SDLS_init(WIDTH, HEIGHT, &win, &ren);

	plateau_t pl;
	enigme_t eni;
	char* ch = "(0,0,H)(0,0,G)(0,0,B)(0,0,D)(2,0,H)";
	char* ch1 = "(13r,5r,1b,1b,12j,2j,3v,13v,4c,4c,B)";
	init_plateau(pl);
	parse_plateau(ch, pl);
	parse_enigme(ch1, &eni);

	display_plateau(ren, pl);
	display_enigme(ren, &eni);

	SDL_Delay(9000);
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();
	return 0;
}
void ihm() {
	ihm1();
}
