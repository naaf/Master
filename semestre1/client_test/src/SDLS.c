#include<SDL2/SDL.h>
#include<stdio.h>

// Version 1.1
SDL_Texture* SDLS_loadTexture(char * filename, SDL_Renderer *ren){
        SDL_Texture *texture = 0;
        SDL_Surface *loadedImage = 0;
        loadedImage = SDL_LoadBMP(filename);
        if (!loadedImage)
          {
          fprintf(stderr,"Erreur chargement image : %s\n",SDL_GetError());
          return 0;
          }
 
        texture = SDL_CreateTextureFromSurface(ren, loadedImage);
        SDL_FreeSurface(loadedImage);
        if (texture == 0)
          {
          fprintf(stderr,"Erreur creation texture : %s\n",SDL_GetError());
          return 0;
          }
        return texture;
}
Uint32 SDLS_couleur(int r, int g, int b)
{ return (((r<<8)+ g)<<8 )+ b; }
 
int SDLS_getpixels(SDL_Renderer * ren, Uint32* pixels, int w, int h)
{
    int res;
    int width=w;//, height=h;
    //SDL_RenderGetLogicalSize(ren,&width,&height);
    //printf("%d %d\n",width,height);
    res = SDL_RenderReadPixels(ren,NULL,SDL_PIXELFORMAT_ARGB8888,pixels,width*sizeof(Uint32));
    if (res<0) 
    {
        fprintf(stderr,"Erreur getpixels : %s\n",SDL_GetError());
        return -1;
    }
    return 0;
}
int  SDLS_putpixels(SDL_Renderer * ren, Uint32* pixels, int w, int h) 
{
  SDL_Texture * tex;
  int width=w, height=h;
  //SDL_RenderGetLogicalSize(ren,&width,&height);
  tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_ARGB8888,
  SDL_TEXTUREACCESS_STREAMING, width, height);
  SDL_UpdateTexture(tex, NULL, pixels, width * sizeof (Uint32));
  SDL_RenderCopy(ren,tex, NULL, NULL);
  SDL_RenderPresent(ren);
  SDL_DestroyTexture(tex);
  return 0;
}

void SDLS_affiche_image(char * fname,SDL_Renderer * ren, int x, int y) 
{
  SDL_Texture * tex = 0;
  SDL_Rect dst;
 
  tex = SDLS_loadTexture(fname,ren);
  if (tex == 0) return;
 
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
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
	fprintf(stderr,"Erreur initialisation\n");
	return -1;
    }
    /* Création de la fenêtre et du renderer */
    SDL_CreateWindowAndRenderer(w,h, 0, win, ren); // SDL_WINDOW_SHOWN|SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC,&win,&ren);
    if (!*win || !*ren) 
	 {
	fprintf(stderr,"Erreur à la création des fenêtres\n");
	SDL_Quit();
	return -1;
    }
    /* Affichage du fond noir */
    SDL_SetRenderDrawColor(*ren, 0, 0, 0, 255);
    SDL_RenderClear(*ren);
  
    SDL_RenderPresent(*ren);
  
    return 0;
}
