/*
 * ihm.c
 *
 *  Created on: 13 mars 2016
 *      Author: naaf
 */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include "../headers/ihm.h"
#define MAP "assets/map.bmp"
#define ACC "assets/accueil.png"

extern pthread_t thread_com, thread_chat;
extern bilan_t bilan;
extern int attenteTraitement;
extern char msg_signal[128];
extern int sc;
extern bool_t moiJoue;
extern char myName[256];
extern int valideCoups;
extern bool_t quit;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

plateau_t pl;
enigme_t enigme;
plateau_t initPl;
enigme_t initEnigme;
bool_t modeVisualisation;
int currentPhase;
Uint32 timeStart = 0;
char moves[512];
char coups[5];

static SDL_Texture *empty_Tx;
SDL_Renderer *ren;
SDL_Window *win;
TTF_Font *font;
SDL_Color colorBlack = { 0, 0, 0, 0 };

void gest_ihm(int signum, siginfo_t * info, void * vide) {
	SigMsg *sigMsg = (SigMsg*) info->si_value.sival_ptr;
	fprintf(stderr, "gest_ihm %d \n", sigMsg->val);

	attenteTraitement |= sigMsg->val;

	if ( PHASE_SESSION & sigMsg->val) {
		printf("pl %s \n", sigMsg->data);
		init_plateau(initPl);
		parse_plateau(sigMsg->data, initPl);
		cpyPlateau(initPl, pl);
		display_plateau(pl);
	}

	if ( PHASE_REFLEX & sigMsg->val) {

		printf("phase reflexion %s , %s\n", sigMsg->data, sigMsg->data2);

		parse_enigme(sigMsg->data, &initEnigme);
		parse_bilan(sigMsg->data2, &bilan);
		cpyEnigme(&initEnigme, &enigme);
		cpyPlateau(initPl, pl);
		bind_enigme_plateau(pl, &enigme);

		currentPhase = PHASE_REFLEX;

		onclickReset(pl, &enigme, coups, moves);
		display_bilan(&bilan);
		displayMsg("REFLEXION", TRUE);
		timeStart = SDL_GetTicks();
		free(sigMsg->data);
		free(sigMsg->data2);

	}
	if ( PHASE_ENCHERE & sigMsg->val) {
		currentPhase = PHASE_ENCHERE;
		displayMsg("ENCHERE", TRUE);
		timeStart = SDL_GetTicks();
	}
	if ( PHASE_RESO & sigMsg->val) {
		currentPhase = PHASE_RESO;
		displayMsg("SOLUTION", TRUE);
		timeStart = SDL_GetTicks();
	}

	if ( UPDATE_L & sigMsg->val) {
		if (sigMsg->data != NULL && sigMsg->u == NULL) {
			adduser(sigMsg->data, 0, &bilan.list_users);
			free(sigMsg->data);
		}
		if (sigMsg->data2 != NULL && sigMsg->u == NULL) {
			removeuser(sigMsg->data2, &bilan.list_users);
			free(sigMsg->data2);
		}
		display_bilan(&bilan);
	}
	if ( UPDATE_U & sigMsg->val) {
		if (sigMsg->data != NULL && sigMsg->data2 != NULL) {
			user_t *u = getuser(sigMsg->data, &bilan.list_users);
			if (u == NULL) {
				printf(stderr, "erreur user not exist %s", sigMsg->data);
			} else {
				u->nb_coups = atoi(sigMsg->data2);
			}
		}
		display_bilan(&bilan);
	}
	if ( SIGALEMENT & sigMsg->val) {
		displayMsg(msg_signal, FALSE);
	}
	free(sigMsg);
}

int estEntier(char *s) {
	int nb = atoi(s);
	if (nb > 0 || (nb == 0 && !strcmp(s, "0")))
		return nb;
	return -1;
}

void updateView(SDL_Rect* rectCoup, SDL_Rect* rectEmpty) {
	SDL_Texture *tmp_Tx;

	display_plateau(pl);
	display_enigme(&enigme);

	tmp_Tx = txt2Texture(ren, font, &colorBlack, coups);
	displayCoup(tmp_Tx, *rectCoup, rectEmpty);

	SDL_RenderPresent(ren);
}

void onclickReset(plateau_t srcPl, enigme_t *srcE, char* coups, char* moves) {

	SDL_Rect rectSrc = { 12 * CASE, 17 * CASE, CASE * 4, 64 };
	SDL_Rect rectDst = { 0, 0, 64, 32 };
	SDL_Texture *tmp_Tx;

	cpyEnigme(srcE, &enigme);
	cpyPlateau(srcPl, pl);
	if (coups != NULL) {
		memset(moves, 0, (int) sizeof(moves));
		memset(coups, 0, (int) sizeof(moves));

		sprintf(coups, "%d", 0);
		tmp_Tx = txt2Texture(ren, font, &colorBlack, coups);
		displayCoup(tmp_Tx, rectSrc, &rectDst);
	}

	display_plateau(pl);
	display_enigme(&enigme);
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
	j = 0;

	while (!(attenteTraitement & FIN_CONNEXION) && !quit) {

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
	attenteTraitement &= ~FIN_CONNEXION;
	return quit;
}
void erreur(char *msg, bool_t fin) {
	SDL_ShowSimpleMessageBox(0, "ERREUR", msg, win);
	if (fin)
		quit = TRUE;
}
void displayMsg(char* msg, bool_t phase) {
	SDL_Color color = { 255, 0, 0, 0 };
	SDL_Texture *msg_Tx;
	SDL_Texture *emptyInput_Tx;
	TTF_Font *font;
	SDL_Rect rectSrc = { 0, 0, 96, 32 };
	SDL_Rect rectDst = { 0, 576, 224, 32 };
	if (phase) {
		rectSrc.w = 4 * CASE;
		rectDst.x = 7 * CASE;
		rectDst.y = 16 * CASE;
		rectDst.w = 4 * CASE;
		emptyInput_Tx = IMG_LoadTexture(ren, "assets/phaseEmpty.png");
	} else {
		emptyInput_Tx = IMG_LoadTexture(ren, "assets/inputField.png");
	}
	if (phase)
		font = TTF_OpenFont("assets/dayrom.TTF", 20);
	else
		font = TTF_OpenFont("assets/dayrom.TTF", 16);
	msg_Tx = txt2Texture(ren, font, &color, msg);
	SDL_RenderCopy(ren, emptyInput_Tx, &rectSrc, &rectDst);
	if (!phase)
		rectDst.w -= 64;
	SDL_QueryTexture(msg_Tx, NULL, NULL, &rectDst.w, &rectDst.h);
	SDL_RenderCopy(ren, msg_Tx, NULL, &rectDst);
	SDL_RenderPresent(ren);
	TTF_CloseFont(font);
}

int awaitLoadingTexte(char* msg, int attente) {
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
	while (!(attenteTraitement & attente) && !quit) {
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
	attenteTraitement &= ~attente;
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

	char tmp[5];
	SDLS_affiche_image("assets/emptyListUser.png", ren, rectNom.x, rectNom.y);
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

	memset(myName, 0, 256);
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
				myName[len] = 0;
				updateInput = TRUE;
				break;
			case SDLK_BACKSPACE:
				len = (len == 0) ? len : len - 1;
				myName[len] = 0;
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
					myName[len++] = event.key.keysym.sym
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
					displayMsg("nom vide", FALSE);
				} else {
					send_request(sc, 2, CONNEXION, myName);
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
			inputField_Tx = txt2Texture(ren, font, &color, myName);
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

void phaseReflexion() {

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

	Direction direction;
	int robotSelected;
	bool_t focusCoup, preFocusCoup;
	bool_t focusUser, preFocusUser;
	int userSelected;

	char message[256];
	char move[3];
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

	empty_Tx = IMG_LoadTexture(ren, "assets/inputField.png");
	font = TTF_OpenFont("assets/dayrom.TTF", 12);

	/*** Show **/

	displayAccueil(win, ren);
	SDLS_affiche_image("assets/pl.png", ren, 0, 0);

	awaitLoadingTexte("attente de session ", PHASE_SESSION);

	focusCoup = preFocusCoup = FALSE;
	focusUser = preFocusUser = FALSE;
	userSelected = robotSelected = -1;
	direction = NONE;
	lenCoups = 0;
	memset(message, 0, sizeof(message));
	memset(moves, 0, sizeof(moves));
	memset(move, 0, sizeof(move));
	memset(coups, 0, sizeof(coups));
	if (!quit) {
		awaitLoadingTexte("attente d'enigme ", PHASE_REFLEX);

		// display labelCoup
		tmp_Tx = txt2Texture(ren, font, &colorBlack, labelCoup);
		SDL_QueryTexture(tmp_Tx, NULL, NULL, &rectLabelCoup.w,
				&rectLabelCoup.h);
		SDL_RenderCopy(ren, tmp_Tx, NULL, &rectLabelCoup);
		SDL_RenderPresent(ren);
		TTF_CloseFont(font);
		font = TTF_OpenFont("assets/dayrom.TTF", 20);
	}
	SDL_StartTextInput();
	int k, save_yUser;

	while (!quit) {
		SDL_WaitEvent(&event);
		switch (event.type) {
		case SDL_MOUSEBUTTONDOWN:
//			handle send moves or coups numbers of moves
			if (estContenu(&rectSendMoves, &event.motion)) {
				if (currentPhase == PHASE_REFLEX && strlen(coups) > 0) {
					send_request(sc, 3, SOLUTION, myName, coups);
				} else if (currentPhase == PHASE_ENCHERE && strlen(coups) > 0) {
					valideCoups =
							(valideCoups == -1) ? atoi(coups) : valideCoups;
					send_request(sc, 3, ENCHERE, myName, coups);
				} else if (currentPhase == PHASE_RESO && strlen(moves) > 0
						&& moiJoue) {
					send_request(sc, 3, SOLUTION, myName, moves);
				} else {
					if (!moiJoue && currentPhase == PHASE_RESO) {
						displayMsg("Ce n'est pas votre tour", FALSE);
					} else {
						displayMsg("coup ou deplacement est vide ", FALSE);
					}
				}
			}
//			handle btn reset /*|| estContenu(&rectCoup, &event.motion)*/
			if (estContenu(&rectReset, &event.motion)
					|| estContenu(&rectCoup, &event.motion)) {
				if (strlen(moves) > 0 || strlen(coups) > 0) {
					onclickReset(initPl, &initEnigme, coups, moves);
				}
			}
//			handle btn arret simulation
			if (estContenu(&rectArret, &event.motion)) {
				if (modeVisualisation) {
					arreterVisualisation();
					displayMsg("arret de visualisation", FALSE);
				}
			}
//			handle btn voir simulation
			if (estContenu(&rectVoir, &event.motion)) {
				if (userSelected >= 0) {
					displayMsg("visalisation", FALSE);
					demarrerVisualisation(userSelected);
				}
			}
//			handle btn suivant (movement suivant de la simulation
			if (estContenu(&rectSuivant, &event.motion)) {
				if (modeVisualisation) {
					pasVisualisation();
					displayMsg("pas suivant", FALSE);
				}
			}

//			handle focus coups
			focusCoup = FALSE;
			if (estContenu(&rectCoup, &event.motion)) {
				SDLS_affiche_image("assets/focusCoups.png", ren, rectCoup.x,
						rectCoup.y + CASE);
				memset(coups, 0, sizeof(coups));
				lenCoups = 0;
				preFocusCoup = focusCoup = TRUE;

			}
//			handle select user
			save_yUser = rectUser.y;
			focusUser = FALSE;
			for (k = 0; k < bilan.list_users.nb; k++) {
				if (estContenu(&rectUser, &event.motion)) {
					if (userSelected > -1) {
						int y = rectUser.y;
						rectUser.y = save_yUser + CASE * userSelected;
						SDLS_affiche_image("assets/unfocusUser.png", ren,
								rectUser.x, rectUser.y);
						rectUser.y = y;
					}
					SDLS_affiche_image("assets/focusUser.png", ren, rectUser.x,
							rectUser.y);
					preFocusUser = focusUser = TRUE;
					userSelected = k;

				}
				rectUser.y = rectUser.y + CASE;
			}
			rectUser.y = save_yUser;

			//handle move robot
			if (!modeVisualisation) {
				x = event.motion.x / CASE;
				y = event.motion.y / CASE;
				for (i = 0; i < NB_ROBOT; ++i) {
					if (enigme.robots[i].x == x && enigme.robots[i].y == y)
						robotSelected = i;
				}
				if (robotSelected > -1) {
					direction =
							estDirection(&enigme, &event.motion, robotSelected,
									-1, 0) == TRUE ? Gauche : direction;
					direction =
							estDirection(&enigme, &event.motion, robotSelected,
									1, 0) == TRUE ? Droit : direction;
					direction =
							estDirection(&enigme, &event.motion, robotSelected,
									0, -1) == TRUE ? Haut : direction;
					direction =
							estDirection(&enigme, &event.motion, robotSelected,
									0, 1) == TRUE ? Bas : direction;
				}

				if (direction != NONE) {
					update_pos_robot(pl, &enigme.robots[robotSelected],
							direction);
					/* update dans moves le nouveau deplacement de robot et le coups*/
					move[0] = enigme.robots[robotSelected].c;
					move[1] = direction;
					strcat(moves, move);
					sprintf(coups, "%d", ((int) (strlen(moves) / 2)));

					//update view
					updateView(&rectCoup, &rectEmpty);

					robotSelected = -1;
					direction = NONE;
				}

				// handle unfocus
				if (preFocusCoup && !focusCoup) {
					SDLS_affiche_image("assets/unfocusCoups.png", ren,
							rectCoup.x, rectCoup.y + CASE);
					preFocusCoup = focusCoup;
				}
				if (preFocusUser && !focusUser) {
					rectUser.y = save_yUser + CASE * userSelected;
					SDLS_affiche_image("assets/unfocusUser.png", ren,
							rectUser.x, rectUser.y);
					rectUser.y = save_yUser;
					userSelected = -1;
					preFocusUser = focusUser;
				}
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
	TTF_CloseFont(font);
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	send_request(sc, 2, SORT, myName);
	pthread_cancel(thread_com);
	pthread_cancel(thread_chat);
	SDL_Quit();
	return 0;
}
void ihm() {
	ihm1();
}
