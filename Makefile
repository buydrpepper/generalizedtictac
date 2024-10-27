all:
	gcc -I  src/include -L src/lib -o main main.c brd_spring.c brd_spring.h font8x8_basic.h -lmingw32 -lSDL2main -lSDL2 -fdiagnostics-color=always -g
