#ifndef SDLS_h
#define SDLS_h
// Version 1.1
SDL_Texture* SDLS_loadTexture(char * filename, SDL_Renderer *ren);
void SDLS_affiche_image(char * fname, SDL_Renderer * ren, int x, int y);
int SDLS_init(int w, int h, SDL_Window **win, SDL_Renderer **ren);
int SDLS_putpixels(SDL_Renderer * ren, Uint32* pixels, int w, int h);
int SDLS_getpixels(SDL_Renderer * ren, Uint32* pixels, int w, int h);
void SDLs_dessin(SDL_Texture * tex, SDL_Renderer * ren, int x, int y);
SDL_Texture* txt2Texture(SDL_Renderer *ren, TTF_Font *font, SDL_Color *color,
		char* msg);
#endif
