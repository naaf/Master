#include<stdio.h>
#include "../headers/ihm.h"

SDL_Texture* SDLS_loadTexture(char * filename, SDL_Renderer *ren) {
	SDL_Texture *texture = 0;
	SDL_Surface *loadedImage = 0;
	loadedImage = SDL_LoadBMP(filename);
	if (!loadedImage) {
		fprintf(stderr, "Erreur chargement image : %s\n", SDL_GetError());
		return 0;
	}

	texture = SDL_CreateTextureFromSurface(ren, loadedImage);
	SDL_FreeSurface(loadedImage);
	if (texture == 0) {
		fprintf(stderr, "Erreur creation texture : %s\n", SDL_GetError());
		return 0;
	}
	return texture;
}
Uint32 SDLS_couleur(int r, int g, int b) {
	return (((r << 8) + g) << 8) + b;
}

void SDLS_affiche_image(char * fname, SDL_Renderer * ren, int x, int y) {
	SDL_Texture * tex = 0;
	SDL_Rect dst;

	tex = IMG_LoadTexture(ren, fname);
	if (tex == 0)
		return;

	// Récupération de la taille de la texture
	SDL_QueryTexture(tex, NULL, NULL, &dst.w, &dst.h);

	// Centrage
	dst.x = x; //(WIDTH - dst.w)/2;
	dst.y = y; //(HEIGHT- dst.h)/2;

	// Affichage
	SDL_RenderCopy(ren, tex, NULL, &dst);
	SDL_RenderPresent(ren);
}
int SDLS_init(int w, int h, SDL_Window **win, SDL_Renderer **ren) {
	/* Initialisation de la SDL. Si ça se passe mal, on quitte */
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		fprintf(stderr, "Erreur initialisation\n");
		return -1;
	}
	/* Création de la fenêtre et du renderer */
	SDL_CreateWindowAndRenderer(w, h, 0, win, ren); // SDL_WINDOW_SHOWN|SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC,&win,&ren);
	if (!*win || !*ren) {
		fprintf(stderr, "Erreur à la création des fenêtres\n");
		SDL_Quit();
		return -1;
	}
	/* Affichage du fond noir */
	SDL_SetRenderDrawColor(*ren, 255, 255, 255, 255);
	SDL_RenderClear(*ren);

	SDL_RenderPresent(*ren);

	return 0;
}

void SDLs_dessin(SDL_Texture * tex, SDL_Renderer * ren, int x, int y) {
	SDL_Rect dst;
	if (tex == 0)
		return;

	// Récupération de la taille de la texture
	SDL_QueryTexture(tex, NULL, NULL, &dst.w, &dst.h);

	dst.x = x;
	dst.y = y;

	// Affichage
	SDL_RenderCopy(ren, tex, NULL, &dst);
//	SDL_RenderPresent(ren);
}
SDL_Texture* txt2Texture(SDL_Renderer *ren, TTF_Font *font, SDL_Color *color,
		char* msg) {
	SDL_Surface *surf;
	SDL_Texture *tex;
	surf = TTF_RenderText_Blended(font, msg, *color);
	tex = SDL_CreateTextureFromSurface(ren, surf);

	SDL_FreeSurface(surf);
	return tex;
}

bool_t saveScreenshotBMP(char* filepath, SDL_Window* SDLWindow,
		SDL_Renderer* SDLRenderer) {
	SDL_Surface* saveSurface = NULL;
	SDL_Surface* infoSurface = NULL;
	infoSurface = SDL_GetWindowSurface(SDLWindow);
	if (infoSurface == NULL) {
		printf(
				"Failed to create info surface from window in saveScreenshotBMP(string), SDL_GetError() - ");
	} else {
		unsigned char pixels[infoSurface->w * infoSurface->h
				* infoSurface->format->BytesPerPixel];
		if (pixels == 0) {
			printf(
					"Unable to allocate memory for screenshot pixel data buffer!\n");
			return FALSE;
		} else {
			if (SDL_RenderReadPixels(SDLRenderer, &infoSurface->clip_rect,
					infoSurface->format->format, pixels,
					infoSurface->w * infoSurface->format->BytesPerPixel) != 0) {
				printf(
						"Failed to read pixel data from SDL_Renderer object. SDL_GetError()%s - ",
						SDL_GetError());

				return FALSE;
			} else {
				saveSurface = SDL_CreateRGBSurfaceFrom(pixels, infoSurface->w,
						infoSurface->h, infoSurface->format->BitsPerPixel,
						infoSurface->w * infoSurface->format->BytesPerPixel,
						infoSurface->format->Rmask, infoSurface->format->Gmask,
						infoSurface->format->Bmask, infoSurface->format->Amask);
				if (saveSurface == NULL) {
					printf(
							"Couldn't create SDL_Surface from renderer pixel data. SDL_GetError() - ");
					return FALSE;
				}
				SDL_SaveBMP(saveSurface, filepath);
				SDL_FreeSurface(saveSurface);
				saveSurface = NULL;
			}

		}
		SDL_FreeSurface(infoSurface);
		infoSurface = NULL;
	}
	return TRUE;
}
