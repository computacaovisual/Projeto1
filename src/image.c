#include "image.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* 1. Carregamento de imagem [cite: 16, 17] */
SDL_Surface *image_carregar(const char *caminho) {
    SDL_Surface *surface = IMG_Load(caminho);
    if (!surface) {
        fprintf(stderr, "Erro ao carregar imagem '%s': %s\n", caminho, SDL_GetError());
        return NULL;
    }
    return surface;
}

/* 2. Verificação de cor [cite: 20] */
int image_is_colorida(SDL_Surface *surface) {
    if (!surface) return 0;
    
    // Converte temporariamente para RGB24 para analisar os canais
    SDL_Surface *conv = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGB24);
    if (!conv) return 1;

    Uint8 *pixels = (Uint8 *)conv->pixels;
    int total = conv->w * conv->h;
    
    // Amostragem de pixels para verificar se R == G == B
    for (int i = 0; i < total; i++) {
        if (pixels[i * 3] != pixels[i * 3 + 1] || pixels[i * 3] != pixels[i * 3 + 2]) {
            SDL_DestroySurface(conv);
            return 1; // É colorida
        }
    }
    SDL_DestroySurface(conv);
    return 0; // Já está em cinza
}

/* 2. Conversão para escala de cinza [cite: 21, 22] */
SDL_Surface *image_converter_cinza(SDL_Surface *surface) {
    if (!surface) return NULL;

    SDL_Surface *rgb = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGB24);
    SDL_Surface *cinza = SDL_CreateSurface(rgb->w, rgb->h, SDL_PIXELFORMAT_RGB24);
    
    Uint8 *src = (Uint8 *)rgb->pixels;
    Uint8 *dst = (Uint8 *)cinza->pixels;

    for (int y = 0; y < rgb->h; y++) {
        for (int x = 0; x < rgb->w; x++) {
            int i = y * rgb->pitch + x * 3;
            // Fórmula do enunciado: Y = 0.2125*R + 0.7154*G + 0.0721*B 
            Uint8 Y = (Uint8)(0.2125 * src[i] + 0.7154 * src[i+1] + 0.0721 * src[i+2]);
            dst[i] = dst[i+1] = dst[i+2] = Y;
        }
    }
    SDL_DestroySurface(rgb);
    return cinza;
}

/* 4. Análise e Cálculo do Histograma [cite: 28, 29, 30] */
Histograma image_calcular_histograma(SDL_Surface *surface) {
    Histograma h = {0};
    if (!surface) return h;

    Uint8 *pixels = (Uint8 *)surface->pixels;
    
    // Contagem de frequências e valor máximo 
    for (int y = 0; y < surface->h; y++) {
        for (int x = 0; x < surface->w; x++) {
            Uint8 tom = pixels[y * surface->pitch + x * 3];
            h.valores[tom]++;
            if (h.valores[tom] > h.max_frequencia) h.max_frequencia = h.valores[tom];
        }
    }

    // Média de intensidade (Brilho) [cite: 31]
    int total = surface->w * surface->h;
    double soma = 0;
    for (int i = 0; i < 256; i++) soma += (double)i * h.valores[i];
    h.media = (float)(soma / total);

    // Desvio Padrão (Contraste) [cite: 32]
    double soma_var = 0;
    for (int i = 0; i < 256; i++) {
        soma_var += h.valores[i] * pow(i - h.media, 2);
    }
    h.desvio_padrao = (float)sqrt(soma_var / total);

    return h;
}

/* 5. Equalização do histograma  */
SDL_Surface *image_equalizar(SDL_Surface *surface, Histograma h) {
    if (!surface) return NULL;

    SDL_Surface *out = SDL_CreateSurface(surface->w, surface->h, SDL_PIXELFORMAT_RGB24);
    int total = surface->w * surface->h;
    
    // Cálculo da CDF (Função de Distribuição Acumulada) [cite: 36]
    float cdf[256];
    cdf[0] = (float)h.valores[0] / total;
    for (int i = 1; i < 256; i++) {
        cdf[i] = cdf[i-1] + ((float)h.valores[i] / total);
    }

    Uint8 *src = (Uint8 *)surface->pixels;
    Uint8 *dst = (Uint8 *)out->pixels;

    for (int y = 0; y < surface->h; y++) {
        for (int x = 0; x < surface->w; x++) {
            int i = y * surface->pitch + x * 3;
            Uint8 novo_tom = (Uint8)(cdf[src[i]] * 255);
            dst[i] = dst[i+1] = dst[i+2] = novo_tom;
        }
    }
    return out;
}

/* Liberação de memória  */
void image_destruir(SDL_Surface *surface) {
    if (surface) SDL_DestroySurface(surface);
}