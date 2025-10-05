#pragma once
#include <cstdint>

class Chip8
{
    private:
        uint8_t memory[4096];
        uint8_t reg_V[16];
        uint16_t reg_I;
        
    public:
        Chip8();
        ~Chip8();
};

/*
Notes:
  Memory:
  - Chip8 interpreter occupies 512 bytes
  - Most programs being at mem location 512 (0x200)
  - Uppermost 256 bytes (0xF00-0xFFF) are for display
      refresh, and 96 bytes below that (0xEA0-0xEFF) 
      are reserved for call stack, internal use, and 
      other variables
  Register:
  - 16 8-bit registers
  - Named V0 to VF
  - VF doubles as a flag for some instructions
      should be avoided
      in add operation, it's the carry flag
      in sub operation, it's a "no borrow" flag
      in draw instruction, it's set upon pixel collision
  - Address register is named I and is 12 bits wide
      used for opcodes that involve memory operations

    

*/