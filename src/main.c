/*
 * IDENTIFICAÇÃO DO GRUPO
 * [Bernardo Souza Oliveira] - RA: [10312871]
 * [André Moreira Guimarães] - RA: [10416590]
 * [Henrique Yuji Isogai Yoneoka] - RA: [10418153]
 */

#include <stdio.h>
#include <stdlib.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "image.h"

// Define os limites para classificação (ajuste se necessário)
#define LIMITE_ESCURA 85
#define LIMITE_CLARA 170
#define LIMITE_BAIXO_CONTRASTE 40
#define LIMITE_ALTO_CONTRASTE 80

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s caminho_da_imagem.ext\n", argv[0]);
        return 1;
    }

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "Erro ao inicializar SDL: %s\n", SDL_GetError());
        return 1;
    }
    
    if (!TTF_Init()) {
            fprintf(stderr, "Erro ao inicializar TTF: %s\n", SDL_GetError());
        return 1;
    }

    /* 1. Carrega e converte imagem original */
    SDL_Surface *original = image_carregar(argv[1]);
    if (!original) {
        TTF_Quit();
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

    /* 2. Processamento e Equalização */
    Histograma hist_original = image_calcular_histograma(cinza);
    SDL_Surface *equalizada = image_equalizar(cinza, hist_original);
    Histograma hist_equalizada = image_calcular_histograma(equalizada);

    int is_equalizado = 0; // Estado atual
    SDL_Surface *surf_display = cinza;
    Histograma hist_display = hist_original;

    /* 3. Criação das Janelas e Renderers */
    SDL_Window *janela_main = SDL_CreateWindow("Proj1 - Imagem", cinza->w, cinza->h, 0);
    SDL_Window *janela_sec = SDL_CreateWindow("Proj1 - Controles", 400, 350, 0);

    // Obtém os limites utilizáveis do monitor principal
    SDL_DisplayID display = SDL_GetPrimaryDisplay();
    SDL_Rect bounds;
    SDL_GetDisplayUsableBounds(display, &bounds);

    // Calcula a largura total do "bloco" (janela principal + margem de 10px + janela secundária)
    int largura_total = cinza->w + 10 + 400;

    // Calcula o X inicial para centralizar as DUAS janelas juntas na tela
    int pos_x = bounds.x + (bounds.w - largura_total) / 2;
    int pos_y = bounds.y + (bounds.h - cinza->h) / 2;

    // Evita que a janela principal suma pela esquerda se a imagem for gigantesca
    if (pos_x < bounds.x) pos_x = bounds.x;

    // Posiciona a janela principal
    SDL_SetWindowPosition(janela_main, pos_x, pos_y);

    // Calcula a posição da secundária (logo à direita da principal)
    int sec_x = pos_x + cinza->w + 10;
    
    // TRAVA DE SEGURANÇA: Se a secundária for vazar na direita, ela encosta na borda da tela
    if (sec_x + 400 > bounds.x + bounds.w) {
        sec_x = bounds.x + bounds.w - 400 - 10; // 10px de margem da borda
    }
    
    // Posiciona a janela secundária e define ela como filha
    SDL_SetWindowPosition(janela_sec, sec_x, pos_y);
    SDL_SetWindowParent(janela_sec, janela_main);

    SDL_Renderer *rend_main = SDL_CreateRenderer(janela_main, NULL);
    SDL_Renderer *rend_sec = SDL_CreateRenderer(janela_sec, NULL);

    SDL_Texture *tex_display = SDL_CreateTextureFromSurface(rend_main, surf_display);
    /* 4. Preparação da Fonte */
    TTF_Font *fonte = TTF_OpenFont("font.ttf", 22);
    if (!fonte) {
        printf("AVISO: Ficheiro 'font.ttf' não encontrado na pasta! O texto não será desenhado.\n");
    }

    /* Variáveis do Botão */
    SDL_FRect btn_rect = {100, 280, 200, 40};
    int rodando = 1;
    SDL_Event evento;

   while (rodando) {
        float mx = 0, my = 0;
        Uint32 mouse_state = SDL_GetMouseState(&mx, &my);
        int mouse_hover = (mx >= btn_rect.x && mx <= btn_rect.x + btn_rect.w &&
                           my >= btn_rect.y && my <= btn_rect.y + btn_rect.h);
        int mouse_clicked = mouse_hover && (mouse_state & SDL_BUTTON_LMASK);

        while (SDL_PollEvent(&evento)) {
            // Cmd+Q ou encerramento do sistema
            if (evento.type == SDL_EVENT_QUIT) rodando = 0;
            
            // Clicar no 'X' de qualquer uma das janelas (Específico do SDL3)
            if (evento.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) rodando = 0;
            
            if (evento.type == SDL_EVENT_KEY_DOWN) {
                // Pressionar ESC
                if (evento.key.key == SDLK_ESCAPE) rodando = 0;
                
                // Salvar imagem 
                if (evento.key.key == SDLK_S) {
                    IMG_SavePNG(surf_display, "output_image.png");
                    printf("Imagem salva como output_image.png com sucesso!\n");
                }
            }

            if (evento.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                if (evento.button.button == SDL_BUTTON_LEFT && mouse_hover) {
                    // Alterna estado da imagem
                    is_equalizado = !is_equalizado;
                    surf_display = is_equalizado ? equalizada : cinza;
                    hist_display = is_equalizado ? hist_equalizada : hist_original;
                    
                    SDL_DestroyTexture(tex_display);
                    tex_display = SDL_CreateTextureFromSurface(rend_main, surf_display);
                }
            }
        }

        /* --- DESENHAR JANELA PRINCIPAL --- */
        SDL_SetRenderDrawColor(rend_main, 0, 0, 0, 255);
        SDL_RenderClear(rend_main);
        SDL_RenderTexture(rend_main, tex_display, NULL, NULL);
        SDL_RenderPresent(rend_main);

        /* --- DESENHAR JANELA SECUNDÁRIA --- */
        SDL_SetRenderDrawColor(rend_sec, 240, 240, 240, 255);
        SDL_RenderClear(rend_sec);

        // 1. Desenhar Histograma
        SDL_SetRenderDrawColor(rend_sec, 50, 50, 50, 255);
        int hist_w = 256, hist_h = 150;
        int start_x = (400 - hist_w) / 2, start_y = 20;
        
        SDL_FRect bg_hist = {start_x, start_y, hist_w, hist_h};
        SDL_SetRenderDrawColor(rend_sec, 200, 200, 200, 255);
        SDL_RenderFillRect(rend_sec, &bg_hist);

        SDL_SetRenderDrawColor(rend_sec, 0, 100, 200, 255);
        for (int i = 0; i < 256; i++) {
            if (hist_display.max_frequencia > 0) {
                float altura_barra = ((float)hist_display.valores[i] / hist_display.max_frequencia) * hist_h;
                SDL_FRect barra = {start_x + i, start_y + hist_h - altura_barra, 1, altura_barra};
                SDL_RenderFillRect(rend_sec, &barra);
            }
        }

        // 2. Classificação de Textos
        if (fonte) {
            const char* class_brilho = hist_display.media < LIMITE_ESCURA ? "Escura" : (hist_display.media > LIMITE_CLARA ? "Clara" : "Media");
            const char* class_contraste = hist_display.desvio_padrao < LIMITE_BAIXO_CONTRASTE ? "Baixo" : (hist_display.desvio_padrao > LIMITE_ALTO_CONTRASTE ? "Alto" : "Medio");
            
            char texto_stats[200];
            snprintf(texto_stats, sizeof(texto_stats), "Media: %.1f (%s) | Desvio: %.1f (%s)", 
                     hist_display.media, class_brilho, hist_display.desvio_padrao, class_contraste);

            SDL_Color cor_texto = {0, 0, 0, 255};
            SDL_Surface *surf_texto = TTF_RenderText_Solid(fonte, texto_stats, 0, cor_texto);
            if (surf_texto) {
                SDL_Texture *tex_texto = SDL_CreateTextureFromSurface(rend_sec, surf_texto);
                SDL_FRect text_rect = {20, 190, surf_texto->w, surf_texto->h};
                SDL_RenderTexture(rend_sec, tex_texto, NULL, &text_rect);
                SDL_DestroySurface(surf_texto);
                SDL_DestroyTexture(tex_texto);
            }
        }

        // 3. Desenhar Botão Dinâmico
        if (mouse_clicked) SDL_SetRenderDrawColor(rend_sec, 0, 50, 150, 255);
        else if (mouse_hover) SDL_SetRenderDrawColor(rend_sec, 100, 150, 255, 255);
        else SDL_SetRenderDrawColor(rend_sec, 0, 100, 200, 255);

        SDL_RenderFillRect(rend_sec, &btn_rect);

        // Texto do Botão
        if (fonte) {
            const char *texto_btn = is_equalizado ? "Ver original" : "Equalizar";
            SDL_Color cor_branca = {255, 255, 255, 255};
            SDL_Surface *surf_btn_texto = TTF_RenderText_Solid(fonte, texto_btn, 0, cor_branca);
            if (surf_btn_texto) {
                SDL_Texture *tex_btn_texto = SDL_CreateTextureFromSurface(rend_sec, surf_btn_texto);
                SDL_FRect tr = {
                    btn_rect.x + (btn_rect.w - surf_btn_texto->w) / 2,
                    btn_rect.y + (btn_rect.h - surf_btn_texto->h) / 2,
                    surf_btn_texto->w, surf_btn_texto->h
                };
                SDL_RenderTexture(rend_sec, tex_btn_texto, NULL, &tr);
                SDL_DestroySurface(surf_btn_texto);
                SDL_DestroyTexture(tex_btn_texto);
            }
        }

        SDL_RenderPresent(rend_sec);

        SDL_Delay(16);
   }
}
