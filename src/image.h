#ifndef IMAGE_H
#define IMAGE_H

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

/* Estrutura para armazenar os dados do histograma e estatísticas (Item 4) */
typedef struct {
    int valores[256];      // Frequência de cada nível de cinza (0-255) [cite: 29]
    int max_frequencia;    // Maior valor encontrado (usado para escalar o gráfico)
    float media;           // Média de intensidade (Brilho) [cite: 31]
    float desvio_padrao;   // Desvio padrão (Contraste) [cite: 32]
} Histograma;

/* --- Funções de Carregamento e Conversão (Item 1 e 2) --- */

/* Carrega imagem do disco e retorna uma surface SDL. Retorna NULL em caso de erro. [cite: 16, 17, 18] */
SDL_Surface *image_carregar(const char *caminho);

/* Verifica se a surface já está em escala de cinza ou se é colorida [cite: 20] */
int image_is_colorida(SDL_Surface *surface);

/* Converte surface colorida para escala de cinza usando a fórmula do enunciado. [cite: 21] */
SDL_Surface *image_converter_cinza(SDL_Surface *surface);

/* Libera a surface da memória [cite: 48] */
void image_destruir(SDL_Surface *surface);


/* --- Funções de Processamento (Item 4 e 5) --- */

/* Calcula o histograma e as estatísticas de média e desvio padrão [cite: 29, 30, 31, 32] */
Histograma image_calcular_histograma(SDL_Surface *surface);

/* Realiza a equalização do histograma para melhorar o contraste (Item 5)  */
SDL_Surface *image_equalizar(SDL_Surface *surface, Histograma h);

#endif