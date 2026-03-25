# Projeto 1 - Computação Visual (Processamento de Imagens)

Projeto desenvolvido para a disciplina de Computação Visual (7º semestre). Este software realiza a conversão de imagens coloridas para escala de cinza, calcula e exibe o seu histograma e permite equalizar a imagem para melhorar o seu contraste, utilizando a biblioteca gráfica SDL3.

## Identificação do Grupo
 * [Bernardo Souza Oliveira] - RA: [10312871]
 * [André Moreira Guimarães] - RA: [10416590]
 * [Henrique Yuji Isogai Yoneoka] - RA: [10418153]

*(As contribuições de cada um foram distribuídas entre o processamento matemático em `image.c`, desenvolvimento da GUI em `main.c` e criação da documentação.)*

## Funcionalidades
1. Carregamento de imagens (PNG, JPG, BMP) com tratamento de erros.
2. Conversão automática para tons de cinza.
3. Exibição de duas janelas simultâneas (Imagem em foco e Dashboard de controlo).
4. Cálculo do histograma, Média de Intensidade e Desvio Padrão.
5. Equalização de imagem interativa via botão.
6. Exportação da imagem renderizada ao premir a tecla `S`.

## Dependências
- Linguagem C (C99)
- SDL3, SDL3_image e SDL3_ttf
- Compilador `gcc`
- Ficheiro de fonte `.ttf`.

## Como Compilar e Executar
1. **Compilação (Linux / Windows com MSYS2):** ```bash
   gcc src/main.c src/image.c -o programa -lSDL3 -lSDL3_image -lSDL3_ttf -lm

2. **Compilação MacOS
   gcc src/main.c src/image.c -o programa -I/opt/homebrew/include -L/opt/homebrew/lib -lSDL3 -lSDL3_image -lSDL3_ttf -lm

   ./programa macaco.webp