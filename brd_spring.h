#ifndef BRD_SPRING
#define BRD_SPRING
#include "SDL2/SDL.h"

// tic tac toe

typedef struct GameWindow GameWindow;
typedef struct GameButton GameButton;
typedef struct GameBoard GameBoard;
typedef struct Piece Piece;

enum State {BRD_MENU, BRD_INGAME, BRD_WINNERX, BRD_WINNERO, BRD_TIE};

struct GameWindow {
    size_t w;
    size_t h;
    GameBoard *board;
    size_t num_buttons;
    size_t num_buttons_allocated;
    GameButton *buttons;
    enum State state;
};

struct Piece {
    size_t x;
    size_t y;
    int isX; //true if x, false otherwise
};

struct GameBoard {
    int x;
    int y;
    int playerX;
    size_t gridsize;
    size_t winning_len;
    size_t piecesize;
    size_t num_placed;
    size_t num_allocated;
    int *entire_board; //this must be updated alongside pieces
    Piece *pieces; 
};

struct GameButton {
    int x;
    int y;
    size_t w;
    size_t h;
    size_t text_scale; //1 is defualt
    int hovered;
    void (*onClick)(GameWindow*);
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
    char *text;
};



void BRD_MakeButton(GameWindow *window, void (*action)(GameWindow*), size_t x, size_t y, size_t w, size_t h, size_t text_scale, unsigned char r, unsigned char g, unsigned char b, unsigned char a, char *text) ;

void BRD_Update(GameWindow *window, SDL_Renderer *renderer, SDL_Event *event, int const mx, int const my) ;

GameWindow* BRD_Init(size_t w, size_t h, size_t size);









#endif