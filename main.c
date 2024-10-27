
#include <stdio.h>
#include <stdlib.h>
#include "SDL2/SDL.h"
#include "brd_spring.h"
#include <stdio.h>
#include <math.h>
int main(int argc, char*argv[]) {

    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *window = SDL_CreateWindow("test",SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,1300,900, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if(!window) printf("could not make window: %s" "\n", SDL_GetError());
    if(!renderer) printf("could not make renderer: %s" "\n", SDL_GetError());


    SDL_Event event;
    Uint64 NOW = SDL_GetPerformanceCounter();
    Uint64 LAST = 0;
    double deltaTime = 0;
    //test vars
    GameWindow *gamewindow = BRD_Init(1200, 800, 3);
    //main loop
    int curx = 0;
    int cury = 0;
    double mousevx = 0;
    double mousevy = 0;
    while(1) {
        if(!SDL_PollEvent(&event)) continue;
        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();
        deltaTime = (double) ((NOW - LAST) *1000/ (double)SDL_GetPerformanceFrequency());
        int lastx = curx;
        int lasty = cury;
        SDL_GetMouseState(&curx, &cury);
        mousevx = ((double)(curx - lastx))/deltaTime;
        mousevy = ((double)(cury - lasty))/deltaTime;
        SDL_SetRenderDrawColor(renderer, 0,0,0, 255);
        SDL_RenderClear(renderer);
        switch(event.type) {
            case SDL_QUIT:
                goto END;
            case SDL_KEYDOWN:
                break;
            default:
                break;
        }
        
        BRD_Update(gamewindow, renderer, &event, curx, cury);
        SDL_RenderPresent(renderer);
    }
    END:
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    return EXIT_SUCCESS;
}