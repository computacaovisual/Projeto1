#ifndef IMAGE_H
#define IMAGE_H

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

/* Estrutura para armazenar os dados do histograma e estatísticas */
typedef struct {
    int valores[256];      // Frequência de cada nível de cinza (0-255)
    int max_frequencia;    // Maior valor encontrado (usado para escalar o gráfico)
    float media;           
    float desvio_padrao;   
} Histograma;


SDL_Surface *image_carregar(const char *caminho);

int image_is_colorida(SDL_Surface *surface);

SDL_Surface *image_converter_cinza(SDL_Surface *surface);

void image_destruir(SDL_Surface *surface);

Histograma image_calcular_histograma(SDL_Surface *surface);

SDL_Surface *image_equalizar(SDL_Surface *surface, Histograma h);

#endif
