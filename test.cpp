#include <cstring>
#include <ctype.h>
#include <fstream>
#include <iostream>
#include <stdint.h>
#include <string.h>

using namespace std;

uint16_t get_nibble(uint16_t code, uint16_t bits, uint16_t mask)
{
  return (code & mask) >> bits;
}

int main()
{

  string file_path =
    "/home/fenrir/Programming/CHIP-8 Emulator/roms/IBM Logo.ch8";
  ifstream file;
  file.open(file_path, ios::in | ios::binary);
  if (file.is_open()) {
    cout << "Open" << endl;
    file.seekg(0, ios::end);
    cout << dec << file.tellg();
  } else {
    cout << "failed to open";
  }
  return 0;
}
