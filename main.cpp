#include "chip8.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>

const int SCREEN_HEIGHT = 32;
const int SCREEN_WIDTH = 64;

using namespace std;

int main()
{

  CHIP chip;
  chip.loadRom("/home/fenrir/Programming/CHIP-8 Emulator/roms/IBM Logo.ch8");

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
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        quit = true;
      }
    }
    if (chip.get_Draw()) {
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
      SDL_RenderClear(renderer);
      SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0xFF);
      chip.set_Draw(false);
      for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 64; j++) {
          if (chip.get_Pixel(j, i)) {
            pixel.x = j;
            pixel.y = i;
            SDL_RenderFillRect(renderer, &pixel);
          }
        }
      }
      SDL_RenderPresent(renderer);
    }

    chip.one_Cycle(false, true);
  }

  return 0;
}
