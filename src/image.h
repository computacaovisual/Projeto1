#ifndef IMAGE_H
#define IMAGE_H

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

/* Carrega imagem do disco e retorna uma surface SDL.
   Retorna NULL em caso de erro. */
SDL_Surface *image_carregar(const char *caminho);

/* Verifica se a surface é colorida (retorna 1) ou já está em cinza (retorna 0) */
int image_is_colorida(SDL_Surface *surface);

/* Converte surface colorida para escala de cinza.
   Retorna nova surface em cinza. O chamador deve destruir com SDL_DestroySurface(). */
SDL_Surface *image_converter_cinza(SDL_Surface *surface);

/* Libera a surface da memória */
void image_destruir(SDL_Surface *surface);

#endif
