#include "brd_spring.h"
#include "font8x8_basic.h"
#include <math.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BRD_NUM_LINES 4
int const BRD_WIN_LINES[BRD_NUM_LINES][2] = {
    {1, 0},
    {0, 1},
    {1, 1},
    {1, -1},
};

void BRD_ClearButtons(GameWindow *window) {
    size_t sz = window->num_buttons;
    window->num_buttons = 0;
}

void BRD_FreeBoard(GameWindow *window) {
    if(!window->board) return;
    if(window->board->pieces) free(window->board->pieces);
    free(window->board->entire_board);
    free(window->board);
    window->board = 0;
}

void BTN_ActionNone(GameWindow *window) {
    return;
}
void BRD_ChangedState(GameWindow *window);
void BTN_ActionPlay(GameWindow *window) {
    window->state = BRD_INGAME;
    BRD_ChangedState(window);
}
void BTN_ActionMenu(GameWindow *window) {
    window->state = BRD_MENU;
    BRD_ChangedState(window);
}
void BTN_XWinner(GameWindow *window) {
    window->state = BRD_WINNERX;
    BRD_ChangedState(window);
}
void BRD_MakeButton(GameWindow *window, void (*action)(GameWindow*), size_t x, size_t y, size_t w, size_t h, size_t text_scale, unsigned char r, unsigned char g, unsigned char b, unsigned char a, char *text) {
    size_t const sz = window->num_buttons;
    if(sz == window->num_buttons_allocated) 
        window->buttons = realloc(window->buttons, (window->num_buttons_allocated+ 1) * sizeof(GameButton));
    
    ++window->num_buttons_allocated;
    window->buttons[sz] = (GameButton){.x = x, .y = y, .w = w, .h = h, .text_scale = text_scale, .r = r, .g = g, .b = b, .a = a, .text = text, .onClick = action};
    ++window->num_buttons;
}

void BRD_MakeBoard(GameWindow *window, int const x, int const y, size_t const psize, size_t const sz, size_t winninglen) {
    window->board = malloc(sizeof(GameBoard));
    *window->board = (GameBoard){.x = x, .y = y, .playerX = 1, .gridsize = sz, .piecesize = psize, .winning_len = winninglen,
    .entire_board = calloc(sz * sz * BRD_NUM_LINES, sizeof(int))};
}

void BRD_ChangedState(GameWindow *window) { //helper function
    switch(window->state) {
        case BRD_MENU:
            BRD_ClearButtons(window);
            BRD_FreeBoard(window);
            BRD_MakeButton(window, BTN_ActionPlay, 40, 300, 800, 80, 4, 155, 255, 155, 255, "Click to play GOMOKU");
        break;
        case BRD_INGAME:
            BRD_ClearButtons(window);
            BRD_FreeBoard(window);

            size_t width = 80;
            size_t height = 20;
            int posx = window->w - width - 5;
            int posy = 5;
            BRD_MakeButton(window, BTN_ActionMenu, posx, posy, width, height, 2, 255, 25, 25, 255, "Quit");
            BRD_MakeBoard(window, 10, 10, 48, 15, 5);
        break;
        case BRD_WINNERO:
            BRD_ClearButtons(window);
            BRD_FreeBoard(window);
            BRD_MakeButton(window, BTN_ActionMenu, 300, 300, 450, 100, 4, 255, 255, 255, 255, "Player O Wins!");
        break;
        case BRD_WINNERX:
            BRD_ClearButtons(window);
            BRD_FreeBoard(window);
            BRD_MakeButton(window, BTN_ActionMenu, 300, 300, 450, 100, 4, 255, 255, 255, 255, "Player X Wins!");
        break;
        case BRD_TIE:
            BRD_ClearButtons(window);
            BRD_FreeBoard(window);
            BRD_MakeButton(window, BTN_ActionMenu, 300, 300, 450, 100, 4, 255, 255, 255, 255, "Tie!");
        break;

        default: break;

    }
}



//out of bounds is checked
//updates the player only when no significant event
int* BRD_ReqPlacePiece(GameBoard * const board, size_t const mx, size_t const my) {
    if(!board) return 0;
    size_t const gsz = board->gridsize;
    size_t const x = (mx - board->x) / (board->piecesize);
    size_t const y = (my - board->y) / (board->piecesize);
    if(y >= gsz || x >= gsz) return 0;
    for(size_t i = 0; i < BRD_NUM_LINES; ++i) {
        if(board->entire_board[i * gsz * gsz+ x*gsz + y]) return 0;
    }
        
    //allocate if neccesary (about double each time)
    if(board->num_allocated == board->num_placed) {
        board->pieces = realloc(board->pieces, 2 * sizeof(Piece)*(board->num_allocated + 1));
        board->num_allocated = 2 * (board->num_allocated + 1);
    }

    //update list of pieces
    board->pieces[board->num_placed] = (Piece){.isX = board->playerX, .x = x, .y = y};
    ++board->num_placed;

    //update entire board array
    //this code is ass
    for (size_t curline = 0; curline < BRD_NUM_LINES; ++curline) {
        size_t start_ind = curline * gsz * gsz;
        int const dx1 = BRD_WIN_LINES[curline][0];
        int const dy1 = BRD_WIN_LINES[curline][1];
        int const dx2 = -dx1;
        int const dy2 = -dy1;
        int len1 = 0;
        int len2 = 0;
        int const fac = board->playerX? 1 : -1;
        if(x + dx1 >= 0 && x + dx1 < gsz && y + dy1 >= 0 && y + dy1 < gsz) {
            len1 = fac * board->entire_board[start_ind + (x + dx1) * gsz + y + dy1];
            if(len1 < 0) len1 = 0;
        }
        if(x + dx2 >= 0 && x + dx2 < gsz && y + dy2 >= 0 && y + dy2 < gsz) {
            len2 = fac * board->entire_board[start_ind + (x + dx2) * gsz + y + dy2];
            if(len2 < 0) len2 = 0;
        }
        int const newlen = (len1 + len2 + 1);
        if(newlen == board->winning_len) return (int*) 1;
        
        board->entire_board[start_ind + (x + dx1 * len1) * gsz + y + dy1 * len1] = fac*newlen;
        board->entire_board[start_ind + (x + dx2 * len2) * gsz + y + dy2 * len2] = fac*newlen;
    }
    if(board->num_placed == gsz * gsz) return (int*) 2;
    board->playerX = !board->playerX;
    return 0;

}

void BRD_DrawBoard(GameBoard * const board, SDL_Renderer *renderer) {
    if(!board) return;
    size_t const sz = board->num_placed;
    size_t const psz = board->piecesize;
    for(size_t i = 0; i < sz; ++i) {
        size_t const px = board->pieces[i].x;
        size_t const py = board->pieces[i].y;
        SDL_Rect const rect = {.h = psz, .w = psz, .x = px * psz +  board->x, .y = py * psz + board->y};
        if(board->pieces[i].isX) {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        } else SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        SDL_RenderFillRect(renderer, &rect);
    }
    SDL_SetRenderDrawColor(renderer, 0, 100, 0, 255);

    size_t const gsz =board->gridsize;
    for(size_t i = 1; i < gsz; ++i) {
        SDL_RenderDrawLine(renderer, psz * i + board->x, board->y, psz * i + board->x, board->y + psz * gsz);
        SDL_RenderDrawLine(renderer, board->x, psz * i + board->y, board->x + psz * gsz, psz * i + board->y);
    }
}

void BRD_DrawButton(SDL_Renderer *renderer, GameButton const * const button) {
    SDL_Rect mainrect = {.h = button->h, .w = button->w, .x = button->x, .y = button->y};
    SDL_SetRenderDrawColor(renderer, 
    button->r - button->hovered * 50, 
    button->g - button->hovered * 50, 
    button->b - button->hovered * 50, button->a);
    SDL_RenderFillRect(renderer, &mainrect);
    SDL_SetRenderDrawColor(renderer, 0,0,0, 255);
    size_t const fontsz = button->text_scale;
    size_t const padding = 4;
    for(size_t i = 0; ; ++i) {
        char const cur = button->text[i];
        if(!cur) break;
        char const * const curarr = font8x8_basic[cur]; //size 8
        for(size_t j = 0; j < 8; ++j) { // depth
            char curbits = curarr[j];
            for(size_t k = 0; k < 8; ++k) { //semi width
                if(curbits & 1) {
                    SDL_Rect rect = {.h = fontsz, .w = fontsz, 
                    .x = padding + mainrect.x + fontsz * (8 * i + k),
                    .y = padding + mainrect.y + fontsz * j};
                    SDL_RenderFillRect(renderer, &rect);
                }
                curbits >>= 1;
            }
        }
    }
}

void BRD_Update(GameWindow *window, SDL_Renderer *renderer, SDL_Event *event, int const mx, int const my) {
    //updating buttons
    size_t const num_buttons = window->num_buttons;
    int mouse_pressed = event->type == SDL_MOUSEBUTTONDOWN; 
    for(size_t i = 0; i < num_buttons; ++i) {
        GameButton * const cur = &window->buttons[i];
        if(mx >= cur->x && mx <= cur->x + cur->w && my >= cur->y && my <= cur->y + cur->h)
            cur->hovered = 1;
        else cur->hovered = 0;
        if(cur->hovered && mouse_pressed) {
            //this can be a VERY BAD memory bug if not careful. the only reason it
            //doesn't break is that the buttons array is set to null when buttons are 
            //deleted, and it doesn't unallocate any already allocated memory
            //TODO: FIX THIS!!!
            cur->onClick(window); 
            mouse_pressed = 0;
        };
        BRD_DrawButton(renderer, &window->buttons[i]);
    }
    //updating board
    if(mouse_pressed) {
        int *winner= BRD_ReqPlacePiece(window->board, mx, my);
        if(winner == 1) { //hack for now
            window->state = window->board->playerX? BRD_WINNERX : BRD_WINNERO;
            BRD_ChangedState(window);
        }
        else if (winner == 2) {
            window->state = BRD_TIE;
            BRD_ChangedState(window);
        }
    }
    //this is less bad b/c we assume a window has only one board, and it has to be null if there is no board
    //still this is bad
    //TODO: fix this
    BRD_DrawBoard(window->board, renderer);
    
}

GameWindow* BRD_Init(size_t w, size_t h, size_t size) {

    GameBoard *board = malloc(sizeof(*board));
    *board = (GameBoard){.gridsize = size};
    GameWindow *ret = malloc(sizeof(*ret));
    *ret = (GameWindow){.w = h, .h = h, .state=BRD_MENU, .board = board};
    BRD_ChangedState(ret);
    //state is menu now
    
} 
