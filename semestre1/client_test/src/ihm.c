/*
 * ihm.c
 *
 *  Created on: 13 mars 2016
 *      Author: naaf
 */
#include <stdio.h>
#include <stdlib.h>

#include "../headers/ihm.h"
#include"../headers/EntreeSortie.h"
#define MAP "assets/map.bmp"
#define ACC "assets/accueil.png"

static SDL_Texture *empty_Tx;
char requete[REQUETE_SIZE];
extern plateau_t pl;
extern enigme_t enigme;
extern int coupure;
bool_t quit = FALSE;

bool_t estDirection(enigme_t *e, SDL_MouseMotionEvent* p, int selected, int x,
		int y) {
	return (e->robots[selected].x + x == p->x / CASE)
			&& (e->robots[selected].y + y == p->y / CASE);
}

void onclickReset() {

}
void onclickSend(char* message) {
	printf("send %s \n", message);

}


void test(int d){
	if( d & CHMUR) printf("CHMUR OK \n");
	if( d & CBMUR) printf("CBMUR OK \n");
	if( d & CGMUR) printf("CGMUR OK \n");
	if( d & CDMUR) printf("CDMUR OK \n");
	if( d & CROBOT) printf("CROBOT OK \n");
	if( d & CSMUR) printf("CSMUR OK \n");

}

bool_t estContenu(SDL_Rect *rect, SDL_MouseMotionEvent* p) {
	if (rect->x > p->x || p->x > rect->x + rect->w)
		return FALSE;
	if (rect->y > p->y || p->y > rect->y + rect->h)
		return FALSE;
	return TRUE;
}
void update_pos_robot(plateau_t p, robot_t *r, Direction d) {
	p[r->x][r->y] &= ~CROBOT;
	test(p[0][0]);
	switch (d) {
	case Haut:
		while (!(p[r->x][r->y] & CHMUR) && r->y > 0
				&& !(p[r->x][r->y - 1] & CROBOT))
			r->y -= 1;
		break;
	case Bas:
		while (!(p[r->x][r->y] & CBMUR) && r->y < NB_CASE - 1
				&& !(p[r->x][r->y + 1] & CROBOT))
			r->y += 1;
		break;
	case Gauche:
		while (!(p[r->x][r->y] & CGMUR) && r->x > 0
				&& !(p[r->x - 1][r->y] & CROBOT))
			r->x -= 1;
		break;
	case Droit:
		while (!(p[r->x][r->y] & CDMUR) && r->x < NB_CASE - 1
				&& !(p[r->x + 1][r->y] & CROBOT))
			r->x += 1;
		break;
	case NONE:
		break;
	}
	p[r->x][r->y] |= CROBOT;

}

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
		tex = IMG_LoadTexture(ren, e->robots[i].path);
		SDLs_dessin(tex, ren, e->robots[i].x * CASE, e->robots[i].y * CASE);
	}
	tex = IMG_LoadTexture(ren, e->cible.path);
	SDLs_dessin(tex, ren, e->cible.x * CASE, e->cible.y * CASE);
	SDL_RenderPresent(ren);
}

void awaitLoading(SDL_Renderer *ren) {
	SDL_Texture *loading_Tx;
	loading_Tx = IMG_LoadTexture(ren, "assets/loading.png");

	SDL_Rect dst_loading = { 12 * CASE, 12 * CASE, 64, 64 };
	SDL_Rect src_loading = { 0, 0, 64, 64 };

	int i = 0;
	coupure = 1;
	while (coupure) {
		src_loading.x = 2 * 64;
		SDL_RenderCopy(ren, loading_Tx, &src_loading, &dst_loading);
		src_loading.x = i * 64;
		SDL_RenderCopy(ren, loading_Tx, &src_loading, &dst_loading);
		SDL_RenderPresent(ren);
		SDL_Delay(200);
		i = (i + 1) % 2;
	}

}
SDL_Texture* txt2Texture(SDL_Renderer * ren, TTF_Font *font, SDL_Color *color,
		char* msg) {
	SDL_Surface *surf;
	SDL_Texture *tex;
	surf = TTF_RenderText_Blended(font, msg, *color);
	tex = SDL_CreateTextureFromSurface(ren, surf);

	SDL_FreeSurface(surf);
	return tex;
}

void awaitLoadingTexte(SDL_Renderer *ren, char* msg) {
//SDLS_init(768, 608, &win, &ren);
	SDL_Texture *msg_Tx;
	SDL_Texture *emptyInput_Tx = IMG_LoadTexture(ren, "assets/inputField.png");
	SDL_Rect rectSrc = { 0, 32, 96, 32 };
	SDL_Rect rectDst = { 0, 576, 224, 32 };
	TTF_Font *font = TTF_OpenFont("assets/dayrom.TTF", 20);
	SDL_Color color = { 255, 0, 0, 0 };

	msg_Tx = txt2Texture(ren, font, &color, msg);
	SDL_RenderCopy(ren, emptyInput_Tx, &rectSrc, &rectDst);
	rectDst.w -= 64;
	SDL_RenderCopy(ren, msg_Tx, NULL, &rectDst);
	SDL_RenderPresent(ren);

	SDL_Rect rectDst2 = { 160, 576, 64, 32 };
	SDL_Rect rectSrc2 = { 0, 0, 64, 32 };
	int i = 0;
	char *ps;
	coupure = 1;
	while (coupure) {
		ps = i == 0 ? "." : i == 1 ? "..." : "....";
		msg_Tx = txt2Texture(ren, font, &color, ps);
		SDL_RenderCopy(ren, emptyInput_Tx, &rectSrc2, &rectDst2);
		SDL_RenderCopy(ren, msg_Tx, NULL, &rectDst2);
		SDL_RenderPresent(ren);
		i = (i + 1) % 3;
		SDL_Delay(500);
	}
	SDL_RenderCopy(ren, emptyInput_Tx, &rectSrc, &rectDst);
	SDL_RenderPresent(ren);
	TTF_CloseFont(font);
}

void displayAccueil(SDL_Window *win, SDL_Renderer *ren) {
	bool_t quitAccueil = FALSE;
	SDL_Event event;
	SDL_Color color = { 0, 0, 0, 0 };
	if (TTF_Init() == -1) {
		fprintf(stderr, "Erreur d'initialisation de TTF_Init : %s\n",
		TTF_GetError());
	}
	TTF_Font *font = TTF_OpenFont("assets/dayrom.TTF", 20);

	SDL_Texture *inputField_Tx;
	SDL_Texture *emptyInput_Tx = IMG_LoadTexture(ren, "assets/inputField.png");
	SDL_Rect rectInputField = { 8 * CASE, 8 * CASE, 0, 0 };
	SDL_Rect rectEmptyField = { 8 * CASE, 8 * CASE, 9 * CASE, 64 };
	SDL_Rect rectSend = { 17 * CASE, 8 * CASE, 64, 64 };

	char name[256];
	memset(name, 0, 256);
	int len = 0;
	bool_t shift, updateInput;

	shift = updateInput = FALSE;
	SDLS_affiche_image(ACC, ren, 0, 0);

	while (!quitAccueil) {
		SDL_WaitEvent(&event);
		switch (event.type) {
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case SDLK_DELETE:
				len = (len == 0) ? len : len - 1;
				name[len] = 0;
				updateInput = TRUE;
				break;
			case SDLK_BACKSPACE:
				len = (len == 0) ? len : len - 1;
				name[len] = 0;
				updateInput = TRUE;
				break;
			case SDLK_KP_ENTER:
				break;
			case SDLK_LSHIFT:
			case SDLK_RSHIFT:
				shift = TRUE;
				break;
			default:
				if (event.key.keysym.sym >= 'a'
						&& event.key.keysym.sym <= 'z') {
					name[len++] = event.key.keysym.sym
							- ((shift == TRUE) ? 32 : 0);
					updateInput = TRUE;
				}

				break;
			}
			break;
		case SDL_KEYUP:
			switch (event.key.keysym.sym) {
			case SDLK_LSHIFT:
			case SDLK_RSHIFT:
				shift = FALSE;
				break;
			}
			break;
		case SDL_MOUSEBUTTONDOWN:
			if (estContenu(&rectSend, &event.motion)) {
				if (len <= 0) {
					SDL_ShowSimpleMessageBox(0, "ERROR", "nom vide", win);
				} else {
					memset(requete, 0, REQUETE_SIZE);
					sprintf(requete, "%s/%s/\n", CONNEXION, name);

					onclickSend(requete);
					awaitLoading(ren);
//					TODO
					quitAccueil = TRUE;
				}
			}
			break;
		case SDL_MOUSEBUTTONUP:
			break;

		case SDL_QUIT:
			quit = TRUE;
			quitAccueil = TRUE;
			break;
		}
		if (updateInput) {
			updateInput = FALSE;
			printf("name %s\n", name);
			inputField_Tx = txt2Texture(ren, font, &color, name);
			SDL_QueryTexture(inputField_Tx, NULL, NULL, &rectInputField.w,
					&rectInputField.h);
			SDL_RenderCopy(ren, emptyInput_Tx, NULL, &rectEmptyField);
			SDL_RenderCopy(ren, inputField_Tx, NULL, &rectInputField);
			SDL_RenderPresent(ren);

		}
	}

	TTF_CloseFont(font);
}

void cpyPlateau(plateau_t plsrc, plateau_t pldst) {
	int i, j;
	for (i = 0; i < NB_CASE; i++) {
		for (j = 0; j < NB_CASE; j++) {
			pldst[i][j] = plsrc[i][j];
		}
	}
}
void displayCoup(SDL_Renderer *ren, SDL_Texture *tmp_Tx, SDL_Rect srcR,
		SDL_Rect emptyR) {
	srcR.x = 13 * CASE;
	srcR.y = 18 * CASE - 8;
	srcR.w = 64;

	SDL_RenderCopy(ren, empty_Tx, &emptyR, &srcR);
	SDL_QueryTexture(tmp_Tx, NULL, NULL, &srcR.w, &srcR.h);
	SDL_RenderCopy(ren, tmp_Tx, NULL, &srcR);
}

int estEntier(char *s) {
	int nb = atoi(s);
	if (nb > 0 || (nb == 0 && !strcmp(s, "0")))
		return nb;
	return -1;
}


int ihm1() {
	SDL_Window *win = 0;
	SDL_Renderer *ren = 0;
	TTF_Font *font;
	SDL_Color color = { 0, 0, 0, 0 };
	SDL_Texture *tmp_Tx;
	SDL_Event event;

	SDL_Rect rectLabelCoup = { 10 * CASE, 16 * CASE + 16, 0, 0 };
	SDL_Rect rectSendMoves = { 7 * CASE, 17 * CASE, 64, 64 };
	SDL_Rect rectReset = { 10 * CASE, 17 * CASE, 64, 64 };
	SDL_Rect rectCoup = { 12 * CASE, 17 * CASE, CASE * 4, 64 };
	SDL_Rect rectEmpty = { 0, 0, 64, 32 };

	plateau_t oldPl;
	enigme_t oldEni;
	Direction dr;
	int selected;
	bool_t focusCoup = FALSE;
	bool_t phaseEchere = FALSE;
	bool_t focusChat = FALSE;

	char message[256];
	char moves[512];
	char move[3];
	char coups[5];
	char* labelCoup = "Veuillez entrer le nombre de coups :";

	int x, y, i;

	/*initialisation*/
	SDLS_init(768, 608, &win, &ren);
	if (TTF_Init() == -1) {
		fprintf(stderr, "Erreur d'initialisation de TTF_Init : %s\n",
		TTF_GetError());
	}

	memset(message, 0, sizeof(message));
	memset(moves, 0, sizeof(moves));
	memset(move, 0, sizeof(move));
	memset(coups, 0, sizeof(coups));
	empty_Tx = IMG_LoadTexture(ren, "assets/inputField.png");
	font = TTF_OpenFont("assets/dayrom.TTF", 12);

	displayAccueil(win, ren);

	SDLS_affiche_image("assets/pl.png", ren, 0, 0);

	awaitLoadingTexte(ren, "attente de session ");
	bind_enigme_plateau(pl, &enigme);
	display_plateau(ren, pl);
	awaitLoadingTexte(ren, "attente d'enigme ");
	display_enigme(ren, &enigme);

	oldEni = enigme;
	cpyPlateau(pl, oldPl);
	selected = -1;
	dr = NONE;

	tmp_Tx = txt2Texture(ren, font, &color, labelCoup);
	SDL_QueryTexture(tmp_Tx, NULL, NULL, &rectLabelCoup.w, &rectLabelCoup.h);
	SDL_RenderCopy(ren, tmp_Tx, NULL, &rectLabelCoup);
	SDL_RenderPresent(ren);
	TTF_CloseFont(font);
	font = TTF_OpenFont("assets/dayrom.TTF", 24);

	SDL_StartTextInput();

	while (!quit) {
		SDL_WaitEvent(&event);

		switch (event.type) {
		case SDL_MOUSEBUTTONDOWN:
			if (estContenu(&rectSendMoves, &event.motion)) {
				SDL_ShowSimpleMessageBox(0, "SENDMOVES", moves, win);
				onclickSend(moves);
			}
			if (estContenu(&rectReset, &event.motion)) {
				SDL_ShowSimpleMessageBox(0, "DEBUG", "RESET", win);
				if (strlen(moves) > 0) {
					enigme = oldEni;
					cpyPlateau(oldPl, pl);
					memset(moves, 0, sizeof(moves));
					sprintf(coups, "%d", 0);

					display_plateau(ren, pl);
					display_enigme(ren, &enigme);
					tmp_Tx = txt2Texture(ren, font, &color, coups);
					displayCoup(ren, tmp_Tx, rectCoup, rectEmpty);
					SDL_RenderPresent(ren);

				}
			}
			if (estContenu(&rectCoup, &event.motion)) {
				SDL_ShowSimpleMessageBox(0, "DEBUG", "COUPS", win);
				focusCoup = TRUE;
			}

			//handle move
			x = event.motion.x / CASE;
			y = event.motion.y / CASE;
			for (i = 0; i < NB_ROBOT; ++i) {
				if (enigme.robots[i].x == x && enigme.robots[i].y == y)
					selected = i;
			}

			if (selected > -1) {
				dr = estDirection(&enigme, &event.motion, selected, -1, 0)
						== TRUE ? Gauche : dr;
				dr = estDirection(&enigme, &event.motion, selected, 1, 0)
						== TRUE ? Droit : dr;
				dr = estDirection(&enigme, &event.motion, selected, 0, -1)
						== TRUE ? Haut : dr;
				dr = estDirection(&enigme, &event.motion, selected, 0, 1)
						== TRUE ? Bas : dr;
			}

			if (dr != NONE) {
				update_pos_robot(pl, &enigme.robots[selected], dr);
				/* update move */
				move[0] = enigme.robots[selected].c;
				move[1] = dr;
				strcat(moves, move);
				sprintf(coups, "%d", ((int) (strlen(moves) / 2)));

				// update view améliorer en ne pas reafficher les elts statique
				display_plateau(ren, pl);
				display_enigme(ren, &enigme);
				tmp_Tx = txt2Texture(ren, font, &color, coups);
				displayCoup(ren, tmp_Tx, rectCoup, rectEmpty);

				SDL_RenderPresent(ren);

				selected = -1;
				dr = NONE;
			}
			break;

		case SDL_TEXTINPUT:

			if (focusCoup && phaseEchere) {
				int nb = estEntier(event.text.text);
				if (0 <= nb && nb <= 9) {
					printf("entier ok\n");
				}
			}
			if (focusChat) {
				SDL_ShowSimpleMessageBox(0, "CHAT", "FOCUS", win);
			}

			break;
		case SDL_QUIT:
			quit = TRUE;
			break;

		}
	}

	TTF_CloseFont(font);
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();
	return 0;
}
void ihm() {
	ihm1();
}
