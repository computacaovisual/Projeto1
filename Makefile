CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -I/ucrt64/include/SDL3
LIBS = -L/ucrt64/lib -lSDL3 -lSDL3_image -lSDL3_ttf

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
