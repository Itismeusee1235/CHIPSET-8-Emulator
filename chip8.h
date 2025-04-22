#include <ctype.h>
#include <iostream>
#include <stdint.h>
#include <string.h>

class CHIP
{
  uint16_t pc, I;
  uint8_t st, dt;
  uint8_t sp;
  uint8_t V[16];
  uint16_t stack[16];

  uint8_t memory[4096];
  int display[64 * 32];
  int keyboard[16];

  int get_nibble(int, int, int);

  bool draw_display;
  bool halt;
  bool pressed;

public:
  CHIP();
  bool loadRom(std::string);
  void print();
  bool one_Cycle(bool, bool);
  int get_Pixel(int, int);
  bool get_Draw();
  int get_Key(int);
  void set_Key(int, int);
  void set_Draw(bool);
  void update_Timers();
  int get_SoundTimer();
  ~CHIP();
};
