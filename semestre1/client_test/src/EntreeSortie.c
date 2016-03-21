#include "../headers/EntreeSortie.h"

void ES_initEvent(ES* es){
	memset(es,0,sizeof(ES));
}
void ES_majEvent(ES* es){
	SDL_Event event;
	while(SDL_PollEvent(&event)){
		switch(event.type){
			case SDL_KEYDOWN:
				es->key[event.key.keysym.sym]=1;
			break;
			case SDL_KEYUP:
				es->key[event.key.keysym.sym]=0;
			break;
			case SDL_MOUSEMOTION:
				es->mousex = event.motion.x;
				es->mousey = event.motion.y;
			break;
			case SDL_MOUSEBUTTONDOWN:
				es->mouseButtons[event.button.button] = 1;
			break;
			case SDL_MOUSEBUTTONUP:
				es->mouseButtons[event.button.button] = 0;
			break;
			case SDL_QUIT:
				es->quit = 1;
			break;
			default:
			break;
		}
	}
}
//	ES es;
//	ES_initEvent(&es);
//	while (!es.key[SDLK_ESCAPE] && !es.quit) {
//		if (es.mouseButtons[SDL_BUTTON_LEFT]) {
//			x = es.mousex / CASE;
//			y = es.mousey / CASE;
//
//			for (i = 0; i < NB_ROBOT; ++i) {
//				if (eni.robots[i].x == x && eni.robots[i].y == y)
//					selected = i;
//			}
//
//			if (selected > -1) {
//				dr = estDirection(&eni, &es, selected, -1, 0) == TRUE ?
//						Gauche : dr;
//				dr = estDirection(&eni, &es, selected, 1, 0) == TRUE ?
//						Droit : dr;
//				dr = estDirection(&eni, &es, selected, 0, -1) == TRUE ?
//						Haut : dr;
//				dr = estDirection(&eni, &es, selected, 0, 1) == TRUE ? Bas : dr;
//			}
//
//			if (dr != NONE) {
//				update_pos_robot(pl, &eni.robots[selected], dr);
//				/* update move */
//				move[0] = eni.robots[selected].c;
//				move[1] = dr;
//				strcat(moves, move);
//				printf("%s\n", moves);
//
//				// update view améliorer en ne pas reafficher les elts statique
//				SDL_RenderClear(ren);
////				display_plateau(ren, pl);
//				display_enigme(ren, &eni);
//
//				selected = -1;
//			}
//
//		}
//		if (es.mouseButtons[SDL_BUTTON_RIGHT]) {
//			sprintf(message, "Right button was pressed! %d %d", es.mousex,
//					es.mousey);
//			selected = -1;
//			dr = NONE;
//			SDL_ShowSimpleMessageBox(0, "Mouse", message, win);
//		}
//		dr = NONE;
//		ES_majEvent(&es);
//	}
