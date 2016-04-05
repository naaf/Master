/*
 * main.c
 *
 *  Created on: 2016-04-04
 *      Author: naaf
 */

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

int main(void) {
	SDL_Renderer *ren;
	SDL_Window *win;
	SDLS_init(768, 608, &win, &ren);
	if (TTF_Init() == -1) {
		fprintf(stderr, "Erreur d'initialisation de TTF_Init : %s\n",
		TTF_GetError());
	}
	SDLS_affiche_image("assets/pl.png", ren, 0, 0);
	TTF_Font* Sans = TTF_OpenFont("assets/dayrom.TTF", 24); //this opens a font style and sets a size

	SDL_Color White = { 0, 0, 0 }; // this is the color in rgb format, maxing out all would give you the color white, and it will be your text's color

	SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Sans,
			"put your text here", White); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first

	SDL_Texture* Message = SDL_CreateTextureFromSurface(ren, surfaceMessage); //now you can convert it into a texture

	SDL_Rect Message_rect; //create a rect
	Message_rect.x = 0;  //controls the rect's x coordinate
	Message_rect.y = 0; // controls the rect's y coordinte
	Message_rect.w = 100; // controls the width of the rect
	Message_rect.h = 100; // controls the height of the rect

	//Mind you that (0,0) is on the top left of the window/screen, think a rect as the text's box, that way it would be very simple to understance
	SDL_QueryTexture(Message, NULL, NULL, &Message_rect.w, &Message_rect.h);
	//Now since it's a texture, you have to put RenderCopy in your game loop area, the area where the whole code executes

	SDL_RenderCopy(ren, Message, NULL, &Message_rect); //you put the renderer's name first, the Message, the crop size(you can ignore this if you don't want to dabble with cropping), and the rect which is the size and coordinate of your texture

	SDL_RenderPresent(ren);
	SDL_Delay(2000);
	return EXIT_SUCCESS;
}
