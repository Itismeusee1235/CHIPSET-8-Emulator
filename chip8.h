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

  int get_nibble(int, int, int);

  bool draw_display = false;

public:
  CHIP();
  bool loadRom(std::string);
  void print();
  bool one_Cycle();
  ~CHIP();
};
