#pragma once
#include <cstdint>
#include <iostream>
#include <bitset>
#include "logger.hpp"

enum class PrintMode
{
    Dec,
    Hex,
    Bin
};

class Chip8
{
    private:
        uint8_t _memory[4096];
        uint8_t _reg_V[16];
        uint16_t _reg_I = 0;
        uint16_t _pc = 0;
        uint8_t _delay_timer = 0;
        uint8_t _sound_timer = 0;
        uint16_t _stack[16];
        uint16_t _sp = 0;
        uint8_t _key[16];
        uint8_t _gfx[64 * 32];

    private:
        int V_Size();    
    public:
        Chip8();
        ~Chip8();
        void LoadROM(const std::string& filename);
        void Cycle();
        void Debug_Print(PrintMode pm);
        void Debug_PrintGfx();
        uint8_t (&GetRegisters())[16];
};

Chip8::Chip8()
{
    logger::Info("Chip8 constructor called");
    for (int i = 0; i < V_Size(); i++)
    {
        _reg_V[i] = 0;
        _key[i] = 0;
        _stack[i] = 0;
    }
}

Chip8::~Chip8()
{
    logger::Info("Chip8 destructor called");
}

void Chip8::LoadROM(const std::string& filename)
{

}

void Chip8::Cycle()
{

}

void Chip8::Debug_Print(PrintMode pm = PrintMode::Hex)
{
    int n = V_Size() / 2;
    for (int i = 0; i < n; i++)
    {
        switch(pm)
        {
            case PrintMode::Dec:
                logger::Print("V{:X}: {:03}     V{:X}: {:03}\n", i, (_reg_V[i]), i+n, (_reg_V[i+n]));
                break;
            case PrintMode::Hex:
                logger::Print("V{:X}: {:02X}     V{:X}: {:02X}\n", i, (_reg_V[i]), i+n, (_reg_V[i+n]));
                break;
            case PrintMode::Bin:
                logger::Print("V{:X}: {:08b}     V{:X}: {:08b}\n", i, (_reg_V[i]), i+n, (_reg_V[i+n]));
                break;
        }
    }
    logger::Print("\n");
    switch(pm)
    {
        case PrintMode::Dec:
            logger::Print("I: {:05}   PC: {:05}\n", _reg_I, _pc);
            logger::Print("Delay Timer: {:03}   Sound Timer: {:03}\n", _delay_timer, _sound_timer);
            logger::Print("SP: {:05}\n", _sp);
            break;
        case PrintMode::Hex:
            logger::Print("I: {:04X}   PC: {:04X}\n", _reg_I, _pc);
            logger::Print("Delay Timer: {:02X}   Sound Timer: {:02X}\n", _delay_timer, _sound_timer);
            logger::Print("SP: {:04X}\n", _sp);
            break;
        case PrintMode::Bin:
            logger::Print("I: {:016b}   PC: {:016b}\n", _reg_I, _pc);
            logger::Print("Delay Timer: {:08b}   Sound Timer: {:08b}\n", _delay_timer, _sound_timer);
            logger::Print("SP: {:016b}\n", _sp);
            break;
    }
    n = n / 2;
    logger::Print("\n");
    for (int i = 0; i < n; i++)
    {
        switch(pm)
        {
            case PrintMode::Dec:
                logger::Print("S{:X}: {:05}     S{:X}: {:05}    S{:X}: {:05}    S{:X}: {:05}\n", i, _stack[i], i+n, _stack[i+n], i+(n*2), _stack[i+(n*2)], i+(n*3), _stack[i+(n*3)]);
                break;
            case PrintMode::Hex:
                logger::Print("S{:X}: {:04X}     S{:X}: {:04X}    S{:X}: {:04X}    S{:X}: {:04X}\n", i, _stack[i], i+n, _stack[i+n], i+(n*2), _stack[i+(n*2)], i+(n*3), _stack[i+(n*3)]);
                break;
            case PrintMode::Bin:
                logger::Print("S{:X}: {:016b}     S{:X}: {:016b}    S{:X}: {:016b}    S{:X}: {:016b}\n", i, _stack[i], i+n, _stack[i+n], i+(n*2), _stack[i+(n*2)], i+(n*3), _stack[i+(n*3)]);
                break;
        }
    }
    logger::Print("\n");
    for (int i = 0; i < n; i++)
    {
        switch(pm)
        {
            case PrintMode::Dec:
                logger::Print("Key{:X}: {:05}     Key{:X}: {:05}    Key{:X}: {:05}    Key{:X}: {:05}\n", i, _key[i], i+n, _key[i+n], i+(n*2), _key[i+(n*2)], i+(n*3), _key[i+(n*3)]);
                break;
            case PrintMode::Hex:
                logger::Print("Key{:X}: {:04X}     Key{:X}: {:04X}    Key{:X}: {:04X}    Key{:X}: {:04X}\n", i, _key[i], i+n, _key[i+n], i+(n*2), _key[i+(n*2)], i+(n*3), _key[i+(n*3)]);
                break;
            case PrintMode::Bin:
                logger::Print("Key{:X}: {:016b}     Key{:X}: {:016b}    Key{:X}: {:016b}    Key{:X}: {:016b}\n", i, _key[i], i+n, _key[i+n], i+(n*2), _key[i+(n*2)], i+(n*3), _key[i+(n*3)]);
                break;
        }
    }
}

void Chip8::Debug_PrintGfx()
{

}

int Chip8::V_Size()
{
    return sizeof(_reg_V) / sizeof(_reg_V[0]);
}

uint8_t (&Chip8::GetRegisters())[16]
{
    return _reg_V;
}

/*
Notes:
  Memory:
  - 0x000-0x1FF Chip 8 interperter
  - 0x050-0x0A0 Used for built-in 4x5 pixel font set (0-F)
  - 0x200-0xFFF Program ROM and work RAM
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
  Stack:
  - Used to store return addresses when subroutines are called
    Original version allocated 48 bytes for up to 12 levels
  Timer:
  - Two timers, bot count down at 60 Hz until 0
      Delay timer is for timing events in game, can be set and read
      Sound timer is for sound effects, when nonzero, beeping sound
        is played. Can only be set.
  Input:
  - Hey keyboard, 16 keys, range 0 to F
    Three opcodes used for detecting input
  Graphics and sound:
  - 64x32 pixels
  - Monochrome
  - Sprite pixels are XOR'd with corresponding screen pixel
  - Unset sprite pixels do nothing
  - The carry flag (VF) is set to 1 if any screen pixels are flipped
      from set to unset when a sprite is drawn and set to 0 otherwise
  Opcode table:
  - 35 opcodes
  - two bytes long
  - stored in big-endian
  - opcode in hexadecimal with the following symbols listed below
    - NNN   address
    - NN    8-bit constant
    - N     4-bit constant
    - X     4-bit register identifier
    - Y     4-bit register identifier
    - PC    Programm Counter
    - I     12-bit register (For memory address) (similar to void pointer)
    - VN    One of the 16 available variables. N may be 0 to F
  - instruction set:
    #   | opcode    | type      | C pseudocode          | explanation
    01  | 0NNN      | Call      |                       | Calls machine coude routing at address NNN
    02  | 00E0      | Display   | disp_clear()          | Clears the screen
    03  | 00EE      | Flow      | return;               | Returns from a subroutine
    04  | 1NNN      | Flow      | goto NNN;             | Jumps to address NNN
    05  | 2NNN      | Flow      | *(0xNNN)()            | Calls subroutine at NNN
    06  | 3XNN      | Cond      | if (Vx == NN)         | Skips the next instruction if VX equals NN
    07  | 4XNN      | Cond      | if (Vx != NN)         | Skips the next instruction if VX does not 
        |           |           |                       | equal NN
    08  | 5XY0      | Cond      | if (Vx == Vy)         | Skips the next instruction if VX equals VY
    09  | 6XNN      | Const     | Vx = NN               | Sets VX to NN
    10  | 7XNN      | Const     | Vx += NN              | Adds NN to Vx (carry flag is not changed)
    11  | 8XY0      | Assig     | Vx = Vy               | Sets VX to the value of VY
    12  | 8XY1      | BitOp     | Vx |= Vy              | Sets VX to VX or VY, bitwise OR
    13  | 8XY2      | BitOp     | Vx &= Vy              | Sets VX to VX and VY, bitwise AND
    14  | 8XY3      | BitOp     | Vx ^= Vy              | Sets VX to VX xor VY, bitwise XOR
    15  | 8XY4      | Math      | Vx += Vy              | Adds VY to VX. VF is set to 1 when 
        |           |           |                       | overflow, 0 otherwise
    16  | 8XY5      | Math      | Vx -= Vy              | VY is subtracted fom VX. VF is set to 0 
        |           |           |                       | when there's underflow, 1 otherwise
    17  | 8XY6      | BitOp     | Vx >>= 1              | Shifts VX to the right by 1, then stores 
        |           |           |                       | the least significant bit of VX prior to 
        |           |           |                       | the shift into VF
    18  | 8XY7      | Math      | Vx = Vy - Vx          | Sets VX to VY minus VX. VF is set to 0 when 
        |           |           |                       | there's an underflow, and 1 otherwise
    19  | 8XYE      | BitOp     | Vx <<= 1              | Shifts VX to the left by 1, then sets VF to 
        |           |           |                       | 1 if the most significant bit of VX prior 
        |           |           |                       | to that shift was set, or to 0 if it was 
        |           |           |                       | unset
    20  | 9XY0      | Cond      | if (Vx != Vy)         | Skips the next instruction if VX does not 
        |           |           |                       | equal VY
    21  | ANNN      | MEM       | I = NNN               | Sets I to the address NNN
    22  | BNNN      | Flow      | PC = V0 + NNN         | Jumps to the address NNN plus V0
    23  | CXNN      | Rand      | Vx = rand() & NN      | Sets VX to the result of a bitwise AND on
        |           |           |                       | a random rumber and NN 
    24  | DXYN      | Display   | draw(Vx, Vy, N)       | Draws a sprite at coordinate (VX, VY) that 
        |           |           |                       | has a width of 8px and height of Npx. Each
        |           |           |                       | row of 8 pixels is read as bit-coded 
        |           |           |                       | starting from memory location I. I values 
        |           |           |                       | does not change after exectution. VF is 
        |           |           |                       | set to 1 if any screen pixels are flipped 
        |           |           |                       | from set to unset when set to unset when 
        |           |           |                       | the sprite is drawn, and 0 otherwise
    25  | EX9E      | KeyOp     | if (key() == Vx)      | Skips the next instruction if the key 
        |           |           |                       | stored in VX(only consider the lowest 
        |           |           |                       | nibble) is pressed
    26  | EXA1      | KeyOp     | if (key() != Vx)      | Skips the next instruction if the key 
        |           |           |                       | stored in VX(only consided the lowest 
        |           |           |                       | nibble) is not pressed
    27  | FX07      | Timer     | Vx = get_delay()      | Sets VX to the value of the delay timer
    28  | FX0A      | KeyOp     | Vx = get_key()        | A key press is awaited, and then stored 
        |           |           |                       | in VX (blocking opration, all instruction 
        |           |           |                       | halted untile next key event, delay and 
        |           |           |                       | sound timers should continue processing)
    29  | FX15      | Timer     | delay_timer(Vx)       | Sets the delay timer to VX
    30  | FX18      | Sound     | sound_timer(Vx)       | Sets the sound timer to VX
    31  | FX1E      | MEM       | I += Vx               | Adds VX to I. VF is not affected
    32  | FX29      | MEM       | I = sprite_addr[Vcx]  | Sets I to the location of the sprite for 
        |           |           |                       | the character in VX(only consided the 
        |           |           |                       | lowest nibble) Characters 0-F are 
        |           |           |                       | represented by a 4x5 font
    33  | FX33      | BCD       | set_BCD(Vx)           | Stores the BCD representation of VX, with 
        |           |           | *(I+0) =              | the hundreds digit in memory at location 
        |           |           | BCD(3);               | in I, the tens at location I+1, and the 
        |           |           | *(I+1) =              | ones digit at location I+2
        |           |           | BCD(2);               |
        |           |           | *(I+2) =              |
        |           |           | BCD(1);               |
    34  | FX55      | MEM       | reg_dump(Vx, &I)      | Stores from V0 to VX (inclusive) in memory, 
                                                          starting at address I, The offset from I is
                                                          inscreased by 1 for each value written, 
                                                          but I is not modified
    35  | FX65      | MEM       | reg_load(Vx, &I)      | Fills from V0 to VX (inclusive) with values 
                                                          from memory, starting at address I. The 
                                                          offset from I is increased by 1 for each 
                                                          value read, but I is not modified
*/