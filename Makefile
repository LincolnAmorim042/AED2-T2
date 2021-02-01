all: main

exemplo:
	gcc -o main main.c gfx.h gfx.c -lSDL -lSDL_gfx -lSDL_ttf -Wall -Wextra -pedantic

clean:
	rm main *~
