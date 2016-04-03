/*
 * visualiser.c
 *
 *  Created on: 2016-03-24
 *      Author: naaf
 */

#include "../headers/ihm.h"

extern SDL_Renderer *ren;
extern bilan_t bilan;
extern bool_t modeVisualisation;
extern plateau_t pl;
extern enigme_t enigme;
extern plateau_t initPl;
extern enigme_t initEnigme;
static plateau_t savePlateau;
static enigme_t saveEnigme;

static char *movesVisualisation;
static int offset;
void demarrerVisualisation(int rangUser) {
	int i;
	user_t *cur;

	cur = bilan.list_users.first;
	for (i = 0; i < rangUser; i++) {
		cur = cur->next;
	}
	if (!cur->solution) {
		displayMsg("USER n'a pas de solution",FALSE);
		return;
	} else if (strlen(cur->solution) % 2 != 0) {
		fprintf(stderr, "ERREUR formatNotCorret de solution %s\n", cur->solution);
		return;
	}
	if (!modeVisualisation) {
		cpyEnigme(&enigme, &saveEnigme);
		cpyPlateau(pl, savePlateau);
		modeVisualisation = TRUE;
	}

	movesVisualisation = cur->solution;
	onclickReset(initPl, &initEnigme, NULL, NULL, NULL, NULL);
	offset = 0;
}

void pasVisualisation() {

	char r, d;
	if (offset < strlen(movesVisualisation)) {
		r = movesVisualisation[offset++];
		d = movesVisualisation[offset++];
		switch (r) {
		case Rouge:
			printf("%s lus %c %c\n", movesVisualisation, r, d);
			update_pos_robot(pl, &enigme.robots[ROBOT_R], d);
			break;
		case Vert:
			update_pos_robot(pl, &enigme.robots[ROBOT_V], d);
			break;
		case Bleu:
			update_pos_robot(pl, &enigme.robots[ROBOT_B], d);
			break;
		case Jaune:
			update_pos_robot(pl, &enigme.robots[ROBOT_J], d);
			break;
		default:
			break;
		}
		display_plateau(pl);
		display_enigme(&enigme);
		SDL_RenderPresent(ren);
	}

}

void arreterVisualisation() {
	cpyEnigme(&saveEnigme, &enigme);
	cpyPlateau(savePlateau, pl);
	onclickReset(savePlateau, &saveEnigme, NULL, NULL, NULL, NULL);
	modeVisualisation = FALSE;
}
