#ifndef ENTREE_SORTIE_H
#define ENTREE_SORTIE_H

#include <SDL2/SDL.h>
#include <string.h>

typedef struct{
	char key[SDL_NUM_SCANCODES];
	char mouseButtons[4];
	int quit;
	int mousex, mousey;
	int pause;
	int menu;
}ES;

void ES_majEvent(ES* es);
void ES_initEvent(ES* es);

#endif
