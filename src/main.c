#include <stdio.h>
#include <stdlib.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "image.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s caminho_da_imagem.ext\n", argv[0]);
        return 1;
    }

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "Erro ao inicializar SDL: %s\n", SDL_GetError());
        return 1;
    }

    /* Carrega e converte imagem */
    SDL_Surface *original = image_carregar(argv[1]);
    if (!original) {
        SDL_Quit();
        return 1;
    }

    SDL_Surface *cinza = NULL;
    if (image_is_colorida(original)) {
        printf("Imagem colorida detectada. Convertendo para cinza...\n");
        cinza = image_converter_cinza(original);
    } else {
        printf("Imagem já está em escala de cinza.\n");
        cinza = SDL_ConvertSurface(original, SDL_PIXELFORMAT_RGB24);
    }
    image_destruir(original);

    if (!cinza) {
        SDL_Quit();
        return 1;
    }

    /* Cria janela do tamanho da imagem, centralizada */
    SDL_Window *janela = SDL_CreateWindow("Proj1", cinza->w, cinza->h,
                                          SDL_WINDOW_RESIZABLE);
    if (!janela) {
        fprintf(stderr, "Erro ao criar janela: %s\n", SDL_GetError());
        image_destruir(cinza);
        SDL_Quit();
        return 1;
    }
    SDL_SetWindowPosition(janela, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    SDL_Renderer *renderer = SDL_CreateRenderer(janela, NULL);
    if (!renderer) {
        fprintf(stderr, "Erro ao criar renderer: %s\n", SDL_GetError());
        image_destruir(cinza);
        SDL_DestroyWindow(janela);
        SDL_Quit();
        return 1;
    }

    /* Cria texture a partir da surface cinza */
    SDL_Texture *textura = SDL_CreateTextureFromSurface(renderer, cinza);
    if (!textura) {
        fprintf(stderr, "Erro ao criar textura: %s\n", SDL_GetError());
        image_destruir(cinza);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(janela);
        SDL_Quit();
        return 1;
    }

    /* Loop principal */
    SDL_Event evento;
    int rodando = 1;

    while (rodando) {
        while (SDL_PollEvent(&evento)) {
            if (evento.type == SDL_EVENT_QUIT) rodando = 0;
            if (evento.type == SDL_EVENT_KEY_DOWN &&
                evento.key.key == SDLK_ESCAPE) rodando = 0;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, textura, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    /* Libera recursos */
    SDL_DestroyTexture(textura);
    image_destruir(cinza);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(janela);
    SDL_Quit();

    return 0;
}
