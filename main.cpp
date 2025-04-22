#include "chip8.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_render.h>
#include <unistd.h>

const int SCREEN_HEIGHT = 320;
const int SCREEN_WIDTH = 640;
const int timerFreq = 60;
const int instPerFrame = 11;

using namespace std;

uint8_t keymap[16] = { SDLK_x, SDLK_1, SDLK_2, SDLK_3, SDLK_q, SDLK_w,
                       SDLK_e, SDLK_a, SDLK_s, SDLK_d, SDLK_z, SDLK_c,
                       SDLK_4, SDLK_r, SDLK_f, SDLK_v };
float phase = 0.0f;
float frequency = 440.0f; // A4 tone
float amplitude = 0.25f;
bool play_beep = false;

void audio_callback(void* userdata, Uint8* stream, int len)
{
  float* fstream = (float*)stream;
  int samples = len / sizeof(float);

  for (int i = 0; i < samples; ++i) {
    if (play_beep) {
      fstream[i] = amplitude * sinf(phase);
      phase += 2.0f * M_PI * frequency / 44100.0f;
      if (phase >= 2.0f * M_PI)
        phase -= 2.0f * M_PI;
    } else {
      fstream[i] = 0.0f;
    }
  }
}

int main(int argc, char* argv[])
{

  CHIP chip;
  if (!chip.loadRom(argv[1])) {
    cout << "Failed to load rom" << endl;
    return -1;
  }

  SDL_Window* window = NULL;
  SDL_Renderer* renderer = NULL;
  SDL_AudioSpec wanted, have;
  SDL_AudioDeviceID device = NULL;
  SDL_memset(&wanted, 0, sizeof(wanted));

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

  wanted.freq = 44100;
  wanted.format = AUDIO_F32SYS;
  wanted.channels = 1;
  wanted.samples = 1024;
  wanted.callback = audio_callback;

  device =
    SDL_OpenAudioDevice(NULL, 0, &wanted, &have, SDL_AUDIO_ALLOW_ANY_CHANGE);

  if (device == NULL) {
    printf("Failed to get audio device, %s ", SDL_GetError());
  }

  SDL_PauseAudioDevice(device, 0); // unpause device

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
        if (e.key.keysym.sym == SDLK_l) {
          chip.reset();
        } else if (e.key.keysym.sym == SDLK_p) {
          quit = true;
        }
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

    if (timer_time >= 1000 / timerFreq) {
      chip.update_Timers();
      timer_time = 0;
    }

    int num_cycles = delta_time * instPerFrame * timerFreq / 1000;
    // cout << num_cycles << " " << delta_time << endl;
    for (int i = 0; i < num_cycles; i++) {
      chip.one_Cycle(false, false);
    }

    if (chip.get_SoundTimer()) {
      play_beep = true;
    } else {
      play_beep = false;
    }

    if (delta_time < (1000 / 60)) {
      SDL_Delay((1000 / 60) - delta_time);
    }
  }

  return 0;
}
