/*
 * ihm.c
 *
 *  Created on: 13 mars 2016
 *      Author: naaf
 */
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "../headers/SDLS.h"
#include "../headers/ihm.h"

int fenet2() {
	const int WIDTH = 640;
	const int HEIGHT = 480;
	SDL_Window *win = 0;
	SDL_Renderer *ren = 0;

	SDLS_init(WIDTH, HEIGHT, &win, &ren);
	SDLS_affiche_image("assets/TUX Logo.bmp", ren, 0, 0);
	SDL_Delay(9000);
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();
	return 0;
}
int fenet1() {
	SDL_Window *window;                    // Declare a pointer

	SDL_Init(SDL_INIT_VIDEO);              // Initialize SDL2

	// Create an application window with the following settings:
	window = SDL_CreateWindow("An SDL2 window",                  // window title
			SDL_WINDOWPOS_UNDEFINED,           // initial x position
			SDL_WINDOWPOS_UNDEFINED,           // initial y position
			640,                               // width, in pixels
			480,                               // height, in pixels
			SDL_WINDOW_SHOWN                // flags - see below
			);

	// Check that the window was successfully created
	if (window == NULL) {
		// In the case that the window could not be made...
		printf("Could not create window: %s\n", SDL_GetError());
		return 1;
	}

	// The window is open: could enter program loop here (see SDL_PollEvent())
	SDL_Delay(9000);  // Pause execution for 3000 milliseconds, for example

	// Close and destroy the window
	SDL_DestroyWindow(window);

	// Clean up
	SDL_Quit();
	return 0;
}
void aff() {
	while (1) {
		printf(".");
		sleep(1);
		fflush(stdout);
	}
}

void ihm() {
	fenet2();
}
