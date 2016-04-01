/*
 * ihm.c
 *
 *  Created on: 13 mars 2016
 *      Author: naaf
 */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "../headers/ihm.h"
#define MAP "assets/map.bmp"
#define ACC "assets/accueil.png"

static SDL_Texture *empty_Tx;
extern plateau_t pl;
extern enigme_t enigme;
extern bilan_t bilan;
extern int coupure;
extern char msg_signal[128];
bool_t quit = FALSE;
int currentPhase;
Uint32 timeStart = 0;

SDL_Renderer *ren;
SDL_Window *win;
TTF_Font *font;
SDL_Color colorBlack = { 0, 0, 0, 0 };

void gest_ihm(int signum, siginfo_t * info, void * vide) {
	coupure = 0;
	printf("gest SIG_IHM %d\n", signum);
}

int estEntier(char *s) {
	int nb = atoi(s);
	if (nb > 0 || (nb == 0 && !strcmp(s, "0")))
		return nb;
	return -1;
}

void onclickReset(enigme_t oldEni, plateau_t oldPl, char* coups, char* moves,
		SDL_Rect *rectSrc, SDL_Rect *rectDst) {

	SDL_Texture *tmp_Tx;

	enigme = oldEni;
	cpyPlateau(oldPl, pl);
	memset(moves, 0, (int) sizeof(moves));
	memset(coups, 0, (int) sizeof(moves));
	sprintf(coups, "%d", 0);

	display_plateau(pl);
	display_enigme(&enigme);
	tmp_Tx = txt2Texture(ren, font, &colorBlack, moves);
	displayCoup(tmp_Tx, *rectSrc, rectDst);
	SDL_RenderPresent(ren);
}

bool_t estDirection(enigme_t *e, SDL_MouseMotionEvent* p, int selected, int x,
		int y) {
	return (e->robots[selected].x + x == p->x / CASE)
			&& (e->robots[selected].y + y == p->y / CASE);
}

int awaitLoading() {
	SDL_Event event;
	SDL_Texture *loading_Tx;
	loading_Tx = IMG_LoadTexture(ren, "assets/loading.png");

	SDL_Rect dst_loading = { 12 * CASE, 12 * CASE, 64, 64 };
	SDL_Rect src_loading = { 0, 0, 64, 64 };

	int i = 0, j;
	coupure = 1;
	j = 0;
	while (coupure && !quit) {

		SDL_PollEvent(&event);
		if (event.type == SDL_QUIT) {
			quit = TRUE;
		}

		if (j == 0) {
			src_loading.x = 2 * 64;
			SDL_RenderCopy(ren, loading_Tx, &src_loading, &dst_loading);
			i = i == 0 ? 1 : 0;
			src_loading.x = 64 * i;

			SDL_RenderCopy(ren, loading_Tx, &src_loading, &dst_loading);
			SDL_RenderPresent(ren);
		}
		SDL_Delay(20);
		j = (1 + j) % 5;

	}
	return quit;
}
void displayMsg(char* msg) {
	SDL_Rect rectSrc = { 0, 32, 96, 32 };
	SDL_Rect rectDst = { 0, 576, 224, 32 };
	SDL_Color color = { 255, 0, 0, 0 };
	SDL_Texture *msg_Tx;
	SDL_Texture *emptyInput_Tx = IMG_LoadTexture(ren, "assets/inputField.png");
	TTF_Font *font = TTF_OpenFont("assets/dayrom.TTF", 20);
	msg_Tx = txt2Texture(ren, font, &color, msg);
	SDL_RenderCopy(ren, emptyInput_Tx, &rectSrc, &rectDst);
	rectDst.w -= 64;
	SDL_QueryTexture(msg_Tx, NULL, NULL, &rectDst.w,
						&rectDst.h);
	SDL_RenderCopy(ren, msg_Tx, NULL, &rectDst);
	SDL_RenderPresent(ren);
	TTF_CloseFont(font);
}

int awaitLoadingTexte(char* msg) {
	SDL_Event event;
	SDL_Rect rectSrc = { 0, 32, 96, 32 };
	SDL_Rect rectDst = { 0, 576, 224, 32 };
	SDL_Color color = { 255, 0, 0, 0 };
	SDL_Texture *msg_Tx;
	SDL_Texture *emptyInput_Tx = IMG_LoadTexture(ren, "assets/inputField.png");
	TTF_Font *font = TTF_OpenFont("assets/dayrom.TTF", 20);

	msg_Tx = txt2Texture(ren, font, &color, msg);
	SDL_RenderCopy(ren, emptyInput_Tx, &rectSrc, &rectDst);
	rectDst.w -= 64;
	SDL_RenderCopy(ren, msg_Tx, NULL, &rectDst);
	SDL_RenderPresent(ren);

	SDL_Rect rectDst2 = { 160, 576, 64, 32 };
	SDL_Rect rectSrc2 = { 0, 0, 64, 32 };
	int i = 0;
	int j = 0;
	char *ps;
	coupure = 1;
	while (coupure && !quit) {
		if (j == 0) {
			ps = i == 0 ? "." : i == 1 ? "..." : "....";
			msg_Tx = txt2Texture(ren, font, &color, ps);
			SDL_RenderCopy(ren, emptyInput_Tx, &rectSrc2, &rectDst2);
			SDL_RenderCopy(ren, msg_Tx, NULL, &rectDst2);
			SDL_RenderPresent(ren);
			i = (i + 1) % 3;
		}
		SDL_Delay(20);
		j = (1 + j) % 15;
		SDL_PollEvent(&event);
		if (event.type == SDL_QUIT) {
			quit = TRUE;
		}
	}

	SDL_RenderCopy(ren, emptyInput_Tx, &rectSrc, &rectDst);
	SDL_RenderPresent(ren);
	TTF_CloseFont(font);
	return quit;
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

void display_plateau(plateau_t pl) {

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

void display_enigme(enigme_t *e) {
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
void display_bilan(bilan_t *b) {
	user_t *cur;
	list_user_t* l;
	int max, i;
	SDL_Texture *nom_Tx, *score_Tx, *points_Tx, *tour_Tx;
	SDL_Rect rectNom = { NB_CASE * CASE, CASE, 96, CASE };
	SDL_Rect rectScore = { (NB_CASE + 4) * CASE, CASE, CASE, CASE };
	SDL_Rect rectCoups = { (NB_CASE + 6) * CASE, CASE, CASE, CASE };
	SDL_Rect rectTour = { 3 * CASE, 16 * CASE, CASE, CASE };
	SDL_Rect rectEmpty = { 0, 0, CASE * 2, CASE };

	TTF_Font *font = TTF_OpenFont("assets/dayrom.TTF", 18);
	SDL_Color color = { 0, 0, 0, 0 };

	l = &(b->list_users);
	cur = l->first;
	max = l->nb < 15 ? l->nb : 15;
	printf(" nb_client %d \n", l->nb);
	char tmp[5];

	for (i = 0; i < max && cur != NULL; i++) {
		nom_Tx = txt2Texture(ren, font, &color, cur->name);
		memset(tmp, 0, 5);
		sprintf(tmp, "%d", cur->score);
		score_Tx = txt2Texture(ren, font, &color, tmp);
		memset(tmp, 0, 5);
		sprintf(tmp, "%d", cur->nb_coups);
		points_Tx = txt2Texture(ren, font, &color, tmp);
		SDL_QueryTexture(nom_Tx, NULL, NULL, &rectNom.w, &rectNom.h);
		SDL_RenderCopy(ren, nom_Tx, NULL, &rectNom);
		SDL_QueryTexture(score_Tx, NULL, NULL, &rectScore.w, &rectScore.h);
		SDL_RenderCopy(ren, score_Tx, NULL, &rectScore);
		SDL_QueryTexture(points_Tx, NULL, NULL, &rectCoups.w, &rectCoups.h);
		SDL_RenderCopy(ren, points_Tx, NULL, &rectCoups);
		rectNom.y = rectScore.y = rectCoups.y = CASE + rectNom.y;
		cur = cur->next;
	}
	memset(tmp, 0, 5);
	sprintf(tmp, "%d", b->current_tour);
	SDLS_affiche_image("assets/emptyTT.png", ren, rectTour.x, rectTour.y);
	tour_Tx = txt2Texture(ren, font, &colorBlack, tmp);
	SDL_QueryTexture(tour_Tx, NULL, NULL, &rectTour.w, &rectTour.h);
	SDL_RenderCopy(ren, tour_Tx, &rectEmpty, &rectTour);
	printf(" i %d max %d\n", i, max);
	SDL_RenderPresent(ren);
	TTF_CloseFont(font);
}

void displayAccueil() {
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

	while (!quitAccueil && !quit) {
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
//					TODO send REquete
					//send_request(sc,2,CONNEXION,name);
					awaitLoading();
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

void formatTime(int time, char* timeChaine) {
	int s = time / 1000;
	int m = s / 60;
	s = (s % 60);
	sprintf(timeChaine, "%d : %d", m, s);
}

void displayCoup(SDL_Texture *tmp_Tx, SDL_Rect srcR, SDL_Rect *emptyR) {
	srcR.x = 13 * CASE;
	srcR.y = 18 * CASE - 8;
	srcR.w = 64;

	SDL_RenderCopy(ren, empty_Tx, emptyR, &srcR);
	SDL_QueryTexture(tmp_Tx, NULL, NULL, &srcR.w, &srcR.h);
	SDL_RenderCopy(ren, tmp_Tx, NULL, &srcR);
}

int ihm1() {
	SDL_Texture *tmp_Tx;
	SDL_Event event;

	SDL_Rect rectLabelCoup = { 10 * CASE, 16 * CASE + 16, 0, 0 };
	SDL_Rect rectSendMoves = { 7 * CASE, 17 * CASE, 64, 64 };
	SDL_Rect rectReset = { 10 * CASE, 17 * CASE, 64, 64 };
	SDL_Rect rectCoup = { 12 * CASE, 17 * CASE, CASE * 4, 64 };
	SDL_Rect rectEmpty = { 0, 0, 64, 32 };
	SDL_Rect rectUser = { 512, 32, 256, 32 };
	SDL_Rect rectTime = { 3 * CASE, 17 * CASE, 2 * CASE, 32 };
	SDL_Rect rectVoir = { 19 * CASE, 17 * CASE, 64, 64 };
	SDL_Rect rectArret = { 16 * CASE, 17 * CASE, 64, 64 };
	SDL_Rect rectSuivant = { 22 * CASE, 17 * CASE, 64, 64 };

	plateau_t oldPl;
	enigme_t oldEni;
	Direction dr;
	int selected;
	bool_t focusCoup = FALSE;
	bool_t focusUser = FALSE;
	int userSelected = -1;

	char message[256];
	char moves[512];
	char move[3];
	char coups[5];
	char timeChaine[32];
	int lenCoups;
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
	init_plateau(pl); // TODO Sup
	parse_plateau("(3,4,H)(3,4,G)(12,6,H)(1,4,H)(9,4,G)(15,6,H)(15,6,T)", pl); // TODO sup
	if (awaitLoadingTexte("attente de session ")) {
		goto fin;
	}
	display_plateau(pl);

	if (awaitLoadingTexte("attente d'enigme ")) {
		goto fin;
	}
	parse_enigme("13r,5r,2b,2b,12j,2j,3v,13v,4c,4c,R)", &enigme); //TODO SUP
	parse_bilan("6(saucisse,223)(brouette,0)(zak,1234)(ashraf,512)", &bilan); //TODO SUP
	bind_enigme_plateau(pl, &enigme);
	display_enigme(&enigme);
	display_bilan(&bilan);

	displayMsg("PHASE REFLEXION"); //TODO
	SDL_Delay(1000);
	displayMsg("PHASE ENCHERE");
	SDL_Delay(1000);
	displayMsg("PHASE SOLUTION");
	SDL_Delay(1000);
	//TODO
	oldEni = enigme;
	cpyPlateau(pl, oldPl);
	selected = -1;
	dr = NONE;

	tmp_Tx = txt2Texture(ren, font, &colorBlack, labelCoup);
	SDL_QueryTexture(tmp_Tx, NULL, NULL, &rectLabelCoup.w, &rectLabelCoup.h);
	SDL_RenderCopy(ren, tmp_Tx, NULL, &rectLabelCoup);
	SDL_RenderPresent(ren);
	TTF_CloseFont(font);
	font = TTF_OpenFont("assets/dayrom.TTF", 20);

	SDL_StartTextInput();
	int k, save_yUser;
	timeStart = SDL_GetTicks();
	currentPhase = PHASE_ENCHERE; //TODO SUP
	while (!quit) {
		SDL_WaitEvent(&event);

		switch (event.type) {
		case SDL_MOUSEBUTTONDOWN:
//			handle send moves or coups numbers of moves
			if (estContenu(&rectSendMoves, &event.motion)) {
				SDL_ShowSimpleMessageBox(0, "SENDMOVES", moves, win);
//				SEND REQUETE TODO
			}
//			handle btn reset
			if (estContenu(&rectReset, &event.motion)
					|| estContenu(&rectCoup, &event.motion)) {
				SDL_ShowSimpleMessageBox(0, "DEBUG", "RESET", win);
				if (strlen(moves) > 0) {
					onclickReset(oldEni, oldPl, coups, moves, &rectCoup,
							&rectEmpty);
				}
			}
//			handle btn arret simulation
			if (estContenu(&rectArret, &event.motion)) {
				SDL_ShowSimpleMessageBox(0, "DEBUG", "ARRET", win);
			}
//			handle btn voir simulation
			if (estContenu(&rectVoir, &event.motion)) {
				SDL_ShowSimpleMessageBox(0, "DEBUG", "Voir", win);
			}
//			handle btn suivant (movement suivant de la simulation
			if (estContenu(&rectSuivant, &event.motion)) {
				SDL_ShowSimpleMessageBox(0, "DEBUG", "Suivant", win);
			}

//			handle focus coups
			focusCoup = FALSE;
			if (estContenu(&rectCoup, &event.motion)) {
				SDL_ShowSimpleMessageBox(0, "DEBUG", "COUPS", win);
				SDLS_affiche_image("assets/focusCoups.png", ren, rectCoup.x,
						rectCoup.y + CASE);
				memset(coups, 0, sizeof(coups));
				lenCoups = 0;
				focusCoup = TRUE;
			}
//			handle select user
			save_yUser = rectUser.y;
			focusUser = FALSE;
			for (k = 0; k < bilan.list_users.nb; k++) {
				if (estContenu(&rectUser, &event.motion)) {
					SDL_ShowSimpleMessageBox(0, "DEBUG", "USER", win);
					SDLS_affiche_image("assets/focusUser.png", ren, rectUser.x,
							rectUser.y);
					focusUser = TRUE;
					userSelected = k;

				}
				rectUser.y = rectUser.y + CASE;
			}
			rectUser.y = save_yUser;

			//handle move robot
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
				/* ajout de deplacement robot */
				move[0] = enigme.robots[selected].c;
				move[1] = dr;
				strcat(moves, move);
				sprintf(coups, "%d", ((int) (strlen(moves) / 2)));

				//update view  TODO a ameliorer
				display_plateau(pl);
				display_enigme(&enigme);
				tmp_Tx = txt2Texture(ren, font, &colorBlack, coups);
				displayCoup(tmp_Tx, rectCoup, &rectEmpty);

				SDL_RenderPresent(ren);

				selected = -1;
				dr = NONE;
			}
			// handle unfocus
			if (!focusCoup) {
				printf("focuCoups NON\n");
				SDLS_affiche_image("assets/unfocusCoups.png", ren, rectCoup.x,
						rectCoup.y + CASE);
			}
			if (!focusUser) {
				printf("focuUser NON\n");
				rectUser.y = rectUser.y + CASE * userSelected;
				SDLS_affiche_image("assets/unfocusUser.png", ren, rectUser.x,
						rectUser.y);
				rectUser.y = save_yUser;
			}

			break;
			// handle entrer en dur le nb coups
		case SDL_TEXTINPUT:
			if (focusCoup
					&& ((currentPhase & PHASE_REFLEX)
							|| (currentPhase & PHASE_ENCHERE))) {
				int nb = estEntier(event.text.text);
				if (0 <= nb && nb <= 9) {
					if (lenCoups < 4) {
						coups[lenCoups++] = event.text.text[0];
						tmp_Tx = txt2Texture(ren, font, &colorBlack, coups);
						displayCoup(tmp_Tx, rectCoup, &rectEmpty);
						SDL_RenderPresent(ren);
						printf("entier ok %s\n", coups);
					}
				}
			}
			break;
		case SDL_QUIT:
			quit = TRUE;
			break;

		}
		// UPDATE Time
		memset(timeChaine, 0, 32);
		formatTime(SDL_GetTicks() - timeStart, timeChaine);

		SDLS_affiche_image("assets/emptyTT.png", ren, rectTime.x, rectTime.y);
		tmp_Tx = txt2Texture(ren, font, &colorBlack, timeChaine);
		SDL_QueryTexture(tmp_Tx, NULL, NULL, &rectTime.w, &rectTime.h);
		SDL_RenderCopy(ren, tmp_Tx, &rectEmpty, &rectTime);
		SDL_RenderPresent(ren);
	}
	fin: TTF_CloseFont(font);
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();
	return 0;
}
void ihm() {
	ihm1();
}
