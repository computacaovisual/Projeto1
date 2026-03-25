CC = gcc
# Flags de compilação com otimização O2 para processamento de pixels [cite: 45]
CFLAGS = -std=c99 -Wall -Wextra -I/ucrt64/include/SDL3 -O2
# Inclusão obrigatória de -lm para a biblioteca math.h 
LIBS = -L/ucrt64/lib -lSDL3 -lSDL3_image -lSDL3_ttf -lm

SRC = src/main.c src/image.c
OBJ = $(SRC:.c=.o)
TARGET = programa

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LIBS)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGET).exe $(TARGET)