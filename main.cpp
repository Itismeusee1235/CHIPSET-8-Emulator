#include "chip8.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <unistd.h>

const int SCREEN_HEIGHT = 320;
const int SCREEN_WIDTH = 640;

using namespace std;

uint8_t keymap[16] = { SDLK_x, SDLK_1, SDLK_2, SDLK_3, SDLK_q, SDLK_w,
                       SDLK_e, SDLK_a, SDLK_s, SDLK_d, SDLK_z, SDLK_c,
                       SDLK_4, SDLK_r, SDLK_f, SDLK_v };

int main()
{

  CHIP chip;
  chip.loadRom("/home/fenrir/Programming/CHIP-8 Emulator/roms/6-keypad.ch8");

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

  SDL_Rect pixel = { 31, 16, 10, 10 };

  bool quit = false;

  int timer_time = 0;
  int cycle_time = 0;
  int prev_time = SDL_GetTicks();

  while (!quit) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        quit = true;
      } else if (e.type == SDL_KEYDOWN) {
        cout << "Press" << endl;
        for (int i = 0; i < 16; i++) {
          if (e.key.keysym.sym == keymap[i]) {
            chip.set_Key(i, 1);
            break;
          }
        }
      } else if (e.type == SDL_KEYUP) {
        cout << "Unpress" << endl;
        for (int i = 0; i < 16; i++) {
          if (e.key.keysym.sym == keymap[i]) {
            chip.set_Key(i, 0);
            break;
          }
        }
      }
    }

    chip.print();

    int curr_time = SDL_GetTicks();
    int delta_time = curr_time - prev_time;
    // cout << delta_time << endl;
    prev_time = curr_time;
    timer_time += delta_time;
    cycle_time += delta_time;

    if (chip.get_Draw()) {
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
      SDL_RenderClear(renderer);
      SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0xFF);
      chip.set_Draw(false);
      for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 64; j++) {
          if (chip.get_Pixel(j, i)) {
            pixel.x = j * 10;
            pixel.y = i * 10;
            SDL_RenderFillRect(renderer, &pixel);
          }
        }
      }
      SDL_RenderPresent(renderer);
    }

    if (timer_time >= 1000 / 60) {
      chip.update_Timers();
      timer_time = 0;
    }
    if (cycle_time >= 1000 / 200) {
      chip.one_Cycle(false, false);
    }
  }

  return 0;
}
