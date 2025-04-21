#include "chip8.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <iostream>

const int SCREEN_HEIGHT = 32;
const int SCREEN_WIDTH = 64;

using namespace std;

int main()
{
  SDL_Window* window = NULL;
  SDL_Renderer* renderer = NULL;

  if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    printf("SDL Failed to INIT , %s", SDL_GetError());
    return -1;
  }

  window = SDL_CreateWindow("CHIP 8",
                            SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED,
                            SCREEN_WIDTH,
                            SCREEN_HEIGHT,
                            SDL_WINDOW_SHOWN);

  if (window == NULL) {
    printf("Failed to make window, %s ", SDL_GetError());
    return -1;
  }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == NULL) {
    printf("Failed to make renderer, %s ", SDL_GetError());
    return -1;
  }

  SDL_Rect pixel = { 31, 16, 1, 1 };

  bool quit = false;
  while (!quit) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        quit = true;
      }
    }

    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0xFF);

    for (int i = 0; i < 64; i++) {
      for (int j = 0; j < 32; j++) {
        if ((i + j) % 2 == 1) {
          pixel.x = i;
          pixel.y = j;
          SDL_RenderFillRect(renderer, &pixel);
        }
      }
    }

    SDL_RenderPresent(renderer);
  }

  return 0;
}
