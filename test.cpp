#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <iostream>
using namespace std;

float phase = 0.0f;
float frequency = 440.0f; // A4 tone
float amplitude = 0.25f;
bool play_beep = true;

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

int main()
{
  SDL_Init(SDL_INIT_EVERYTHING);

  SDL_AudioSpec wanted, have;
  SDL_AudioDeviceID device;
  SDL_memset(&wanted, 0, sizeof(wanted));

  SDL_Window* window;
  window = SDL_CreateWindow("Audio", 100, 100, 100, 100, SDL_WINDOW_SHOWN);

  wanted.freq = 44100;
  wanted.format = AUDIO_F32SYS;
  wanted.channels = 1;
  wanted.samples = 1024;
  wanted.callback = audio_callback;

  device =
    SDL_OpenAudioDevice(NULL, 0, &wanted, &have, SDL_AUDIO_ALLOW_ANY_CHANGE);

  SDL_PauseAudioDevice(device, 0); // unpause device
  bool quit = false;
  while (!quit) {
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
      if (ev.type == SDL_QUIT) {
        quit = true;
      } else if (ev.type == SDL_KEYDOWN) {
        cout << "Key";
        if (ev.key.keysym.sym == SDLK_m) {
          cout << play_beep;
          play_beep = !play_beep;
        }
      }
    }
  }
  return 0;
}
