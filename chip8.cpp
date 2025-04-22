#include "./chip8.h"
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <stdint.h>
#include <string.h>

using namespace std;

CHIP::CHIP()
{
  draw_display = true;
  halt = false;
  pressed = false;
  srand(time(0));
  st = 0;
  dt = 0;
  sp = -1;
  I = 0;

  unsigned char font_set[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
  };

  memset(memory, 0, sizeof(memory));
  memset(V, 0, sizeof(V));
  memset(stack, 0, sizeof(stack));

  for (int i = 0; i < 80; i++) {
    memory[i] = font_set[i];
  }

  pc = 0x200;

  memset(display, 0, sizeof(display));
  memset(keyboard, 0, sizeof(keyboard));
}

void CHIP::reset()
{
  draw_display = true;
  halt = false;
  pressed = false;
  srand(time(0));
  st = 0;
  dt = 0;
  sp = -1;
  I = 0;
  memset(V, 0, sizeof(V));
  memset(stack, 0, sizeof(stack));
  pc = 0x200;

  memset(display, 0, sizeof(display));
  memset(keyboard, 0, sizeof(keyboard));
}
bool CHIP::loadRom(std::string file_name)
{
  std::ifstream file;
  size_t size = 0;
  file.open(file_name, ios::in | ios::binary);
  if (file.is_open()) {
    file.seekg(0, ios::end);
    size = file.tellg();
    if (size > 4096 - 512) {
      cout << "ROM too big to load" << endl;
      return false;
    }
    file.seekg(0, ios::beg);
    char c;
    for (int i = 0x200; file.get(c); i++) {
      memory[i] = (uint8_t)c;
    }

  } else {
    return false;
  }
  return true;
}

bool CHIP::one_Cycle(bool trace_mode, bool sound_on)
{

  uint16_t opcode = (memory[pc] << 8) | memory[pc + 1];
  int type_nibble = get_nibble(opcode, 12, 0xF000);

  if (trace_mode) {
    cout << hex << ((opcode & 0xF000) >> 12) << ((opcode & 0x0F00) >> 8)
         << ((opcode & 0x00F0) >> 4) << (opcode & 0x000F) << " ";
    cout << pc << " " << sp << endl;

    for (int i = 0; i < 0xF; i++) {
      cout << V[i] << " ";
    }
    cout << endl;
    cout << st << " " << dt << endl;
  }

  pc += 2;

  int n1, n2, n3;
  switch (type_nibble) {
    case 0: {
      switch (opcode) {
        case 0x00E0:
          memset(display, 0, sizeof(display));
          draw_display = true;
          // cout << "Cleared display, time to draw" << endl;
          break;
        case 0x00EE:
          pc = stack[sp];
          sp -= 1;
          sp = sp < -1 ? -1 : sp;
          break;
      }
      break;
    }
    case 1: {
      pc = (uint16_t)get_nibble(opcode, 0, 0x0FFF);
      break;
    }
    case 2: {
      sp += 1;
      stack[sp] = pc;
      pc = (uint16_t)get_nibble(opcode, 0, 0x0FFF);
      break;
    }
    case 3: {
      n1 = get_nibble(opcode, 8, 0x0F00);
      n2 = get_nibble(opcode, 0, 0x00FF);
      if (n2 == V[n1]) {
        pc += 2;
      }
      break;
    }
    case 4: {
      n1 = get_nibble(opcode, 8, 0x0F00);
      n2 = get_nibble(opcode, 0, 0x00FF);
      if (n2 != V[n1]) {
        pc += 2;
      }
      break;
    }
    case 5: {
      n1 = get_nibble(opcode, 8, 0x0F00);
      n2 = get_nibble(opcode, 4, 0x00F0);
      if (V[n1] == V[n2]) {
        pc += 2;
      }
      break;
    }
    case 6: {
      n1 = get_nibble(opcode, 8, 0x0F00);
      n2 = get_nibble(opcode, 0, 0x00FF);
      V[n1] = n2;
      break;
    }
    case 7: {
      n1 = get_nibble(opcode, 8, 0x0F00);
      n2 = get_nibble(opcode, 0, 0x00FF);
      V[n1] += n2;
      break;
    }
    case 8: {
      n1 = get_nibble(opcode, 8, 0x0F00);
      n2 = get_nibble(opcode, 4, 0x00F0);
      n3 = get_nibble(opcode, 0, 0x000F);
      switch (n3) {
        case 0:
          V[n1] = V[n2];
          break;
        case 1:
          V[n1] = V[n1] | V[n2];
          break;
        case 2:
          V[n1] = V[n1] & V[n2];
          break;
        case 3:
          V[n1] = V[n1] ^ V[n2];
          break;
        case 4: {
          uint16_t sum = V[n1] + V[n2];
          V[n1] = (uint8_t)(sum);
          V[0xF] = sum > 0xFF ? 1 : 0;
          break;
        }
        case 5: {
          int car;
          if (V[n1] >= V[n2]) {
            car = 1;
          } else {
            car = 0;
          }
          V[n1] = V[n1] - V[n2];
          V[0xF] = car;
          break;
        }
        case 6: {
          int car = V[n1] & 0x1;
          V[n1] = (uint8_t)(V[n1] >> 1);
          V[0xF] = car;
          break;
        }
        case 7: {
          int car;
          if (V[n2] >= V[n1]) {
            car = 1;
          } else {
            car = 0;
          }
          V[n1] = V[n2] - V[n1];
          V[0xF] = car;
          break;
        }
        case 0xE: {
          int car = (uint8_t)(V[n1] >> 7);
          V[n1] = (uint8_t)(V[n1] << 1);
          V[0xF] = car;
          break;
        }
      }
      break;
    }
    case 9: {
      n1 = get_nibble(opcode, 8, 0x0F00);
      n2 = get_nibble(opcode, 4, 0x00F0);
      if (V[n1] != V[n2]) {
        pc += 2;
      }
      break;
    }
    case 0xA: {
      n1 = get_nibble(opcode, 0, 0x0FFF);
      I = (uint16_t)n1;
      break;
    }
    case 0xB: {
      n1 = get_nibble(opcode, 0, 0x0FFF);
      pc = n1 + V[0];
      break;
    }
    case 0xC: {
      n1 = get_nibble(opcode, 8, 0x0F00);
      n2 = get_nibble(opcode, 0, 0x00FF);
      V[n1] = (uint8_t)((rand() % 256) & n2);
      break;
    }
    case 0xD: {

      V[0xF] = 0;

      n1 = get_nibble(opcode, 8, 0x0F00);
      n2 = get_nibble(opcode, 4, 0x00f0);
      n3 = get_nibble(opcode, 0, 0x000f);

      int x = V[n1];
      int y = V[n2];

      for (int i = 0; i < n3; i++) {
        uint8_t byte = memory[I + i];
        for (int j = 0; j < 8; j++) {
          int bit = (byte >> (7 - j)) & 1;
          int idx = (x + j) % 64 + ((y + i) % 32) * 64;

          if (display[idx] == 1 && bit == 1) {
            V[0xF] = 1;
          }
          display[idx] = display[idx] ^ bit;
        }
      }

      draw_display = true;
      // cout << "Wrote to display, time to draw" << endl;
      break;
    }
    case 0xE: {
      n1 = get_nibble(opcode, 8, 0x0F00);
      n2 = get_nibble(opcode, 0, 0x00FF);
      switch (n2) {
        case 0x9E: {
          if (keyboard[V[n1]] == 1) {
            pc += 2;
          }
          break;
        }
        case 0xA1: {
          if (keyboard[V[n1]] == 0) {
            pc += 2;
          }
          break;
        }
      }
      break;
    }
    case 0xF: {
      n1 = get_nibble(opcode, 8, 0x0F00);
      n2 = get_nibble(opcode, 0, 0x00FF);
      switch (n2) {
        case 0x07: {
          V[n1] = dt;
          break;
        }
        case 0x0A: {
          if (!halt) {
            halt = true;
          } else {
            if (pressed) {
              if (keyboard[V[n1]] == 0) {
                halt = false;
                pressed = false;
                break;
              }
            } else {
              for (int i = 0; i < 16; i++) {
                if (keyboard[i] != 0) {
                  pressed = true;
                  V[n1] = (uint8_t)i;
                  break;
                }
              }
            }
          }
          pc -= 2;
          break;
        }
        case 0x15: {
          dt = V[n1];
          break;
        }
        case 0x18: {
          st = V[n1];
          break;
        }
        case 0x1E: {
          I = I + V[n1];
          break;
        }
        case 0x29: {
          I = V[n1] * 0x5;
          break;
        }
        case 0x33: {
          memory[I] = V[n1] / 100;
          memory[I + 1] = (V[n1] % 100) / 10;
          memory[I + 2] = V[n1] % 10;
          break;
        }
        case 0x55: {
          for (int i = 0; i <= n1; i++) {
            memory[I + i] = V[i];
          }
          break;
        }
        case 0x65: {
          for (int i = 0; i <= n1; i++) {
            V[i] = memory[I + i];
          }
          break;
        }
      }
      break;
    }
  }
  return true;
}

void CHIP::print() {}

int CHIP::get_Pixel(int x, int y)
{
  return display[x + y * 64];
}

bool CHIP::get_Draw()
{
  return draw_display;
}

void CHIP::set_Draw(bool val)
{
  draw_display = val;
}

int CHIP::get_Key(int key)
{
  return keyboard[key];
}

void CHIP::set_Key(int key, int val)
{
  keyboard[key] = val;
}

void CHIP::update_Timers()
{
  if (dt > 0) {
    dt--;
  }
  if (st > 0) {
    st--;
  }
}

int CHIP::get_SoundTimer()
{
  return st > 0;
}

CHIP::~CHIP() {}

int CHIP::get_nibble(int code, int bits, int mask)
{
  return (code & mask) >> bits;
}
