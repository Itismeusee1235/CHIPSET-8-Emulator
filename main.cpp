#include "chip8.h"
#include <SDL2/SDL.h>
#include <iostream>

using namespace std;

int main()
{
  SDL_Init(SDL_INIT_EVERYTHING);
  SDL_Window* win =
    SDL_CreateWindow("CHIP-8", 64, 32, 64, 32, SDL_WINDOW_SHOWN);

  if (!win) {
    cout << "Creating window error " << SDL_GetError() << endl;
  }

  SDL_Surface* winSurface = SDL_GetWindowSurface(win);

  while (true) {
    SDL_FillRect(
      winSurface, NULL, SDL_MapRGB(winSurface->format, 255, 90, 120));
    SDL_UpdateWindowSurface(win);
  }

  system("pause");

  SDL_DestroyWindow(win);
  win = NULL;
  winSurface = NULL;
  SDL_Quit();

  // CHIP chip;
  // chip.loadRom("/home/fenrir/Programming/CHIP-8 Emulator/roms/IBM Logo.ch8");
  // chip.print();
  return 0;
}
