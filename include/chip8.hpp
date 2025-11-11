#pragma once
#include <cstdint>
#include <iostream>
#include <bitset>
#include <cstdio>
#include <chrono>
#include "logger.hpp"
#include "random.hpp"

enum class PrintMode
{
    Dec,
    Hex,
    Bin
};

extern constexpr int W = 64;
extern constexpr int H = 32;
constexpr static uint16_t RAM = 4096;
constexpr static uint16_t rom_start = 0x200;

class Chip8
{
    public: // <--- change back to private after
        uint8_t _memory[RAM];
        uint8_t _V[16];
        uint16_t _I = 0;
        uint16_t _pc = rom_start;
        uint8_t _delay_timer = 0;
        uint8_t _sound_timer = 0;
        uint16_t _stack[16];
        uint16_t _sp = 0;
        uint8_t _key[16];
        uint8_t _gfx[W * H];
        uint16_t _opcode = 0;
        uint16_t _NNN = 0;
        uint16_t __NN = 0;
        uint16_t ___N = 0;
        uint8_t _X = 0;
        uint8_t _Y = 0;
        bool _waiting_for_key = false;
        uint8_t _waiting_register = -1;
        bool _draw_flag = false;
        size_t _tic = 0;
        bool _waiting_for_vblank = false;
        std::chrono::steady_clock::time_point _next_vblank;
        const uint8_t _fontset[80]
        {
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
        const size_t _fontset_start = 0x0;
    private:
        int V_Size();
        int KeySize();
        int StackSize();
        int FontsetSize();
        void IncrementProgramCounter(int n);
        void Fetch();
        void Decode();
        void Execute();
        void Execute_0x0();
        void Execute_0x1();
        void Execute_0x2();
        void Execute_0x3();
        void Execute_0x4();
        void Execute_0x5();
        void Execute_0x6();
        void Execute_0x7();
        void Execute_0x8();
        void Execute_0x9();
        void Execute_0xA();
        void Execute_0xB();
        void Execute_0xC();
        void Execute_0xD();
        void Execute_0xE();
        void Execute_0xF();  
        void Get_X();
        void Get_Y();
        void VF_Flag();
        void VF_FlagClear();
        void UpdateDelayTimer();
        void UpdateSoundTimer();
        void UpdateTimers();
        void InitFontset();
    public:
        Chip8();
        ~Chip8();
        bool LoadROM(const std::string& filename);
        void Cycle();
        void Update();
        bool DrawFlag();
        void Reset();
        void ClearScreen();
        void Debug_Print(PrintMode pm);
        void Debug_PrintGfx();
        uint8_t (&GetRegisters())[16];
        uint8_t (&GetGfx())[W * H];
        uint8_t &GetDelayTimer();
        uint8_t &GetSoundTimer();
        void SetDelayTimer(uint8_t t);
        void SetSoundTimer(uint8_t t);
        void SetKey(uint8_t k);
        void UnsetKey(uint8_t k);
        void OnKeyPressed(uint8_t k);
        void OnKeyReleased(uint8_t k);
        void MaybeTick_vblank();
};

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
      done? |   #   | opcode    | type      | C pseudocode          | explanation
            |   01  | 0NNN      | Call      |                       | Calls machine code routing at address NNN
        Y   |   02  | 00E0      | Display   | disp_clear()          | Clears the screen
        Y   |   03  | 00EE      | Flow      | return;               | Returns from a subroutine
        Y   |   04  | 1NNN      | Flow      | goto NNN;             | Jumps to address NNN
        Y   |   05  | 2NNN      | Flow      | *(0xNNN)()            | Calls subroutine at NNN
        Y   |   06  | 3XNN      | Cond      | if (Vx == NN)         | Skips the next instruction if VX equals NN
        Y   |   07  | 4XNN      | Cond      | if (Vx != NN)         | Skips the next instruction if VX does not 
            |       |           |           |                       | equal NN
        Y   |   08  | 5XY0      | Cond      | if (Vx == Vy)         | Skips the next instruction if VX equals VY
        Y   |   09  | 6XNN      | Const     | Vx = NN               | Sets VX to NN
        Y   |   10  | 7XNN      | Const     | Vx += NN              | Adds NN to Vx (carry flag is not changed)
        Y   |   11  | 8XY0      | Assig     | Vx = Vy               | Sets VX to the value of VY
        Y   |   12  | 8XY1      | BitOp     | Vx |= Vy              | Sets VX to VX or VY, bitwise OR
        Y   |   13  | 8XY2      | BitOp     | Vx &= Vy              | Sets VX to VX and VY, bitwise AND
        Y   |   14  | 8XY3      | BitOp     | Vx ^= Vy              | Sets VX to VX xor VY, bitwise XOR
        Y   |   15  | 8XY4      | Math      | Vx += Vy              | Adds VY to VX. VF is set to 1 when 
            |       |           |           |                       | overflow, 0 otherwise
        Y   |   16  | 8XY5      | Math      | Vx -= Vy              | VY is subtracted fom VX. VF is set to 0 
            |       |           |           |                       | when there's underflow, 1 otherwise
        Y   |   17  | 8XY6      | BitOp     | Vx >>= 1              | Shifts VX to the right by 1, then stores 
            |       |           |           |                       | the least significant bit of VX prior to 
            |       |           |           |                       | the shift into VF
        Y   |   18  | 8XY7      | Math      | Vx = Vy - Vx          | Sets VX to VY minus VX. VF is set to 0 when 
            |       |           |           |                       | there's an underflow, and 1 otherwise
        Y   |   19  | 8XYE      | BitOp     | Vx <<= 1              | Shifts VX to the left by 1, then sets VF to 
            |       |           |           |                       | 1 if the most significant bit of VX prior 
            |       |           |           |                       | to that shift was set, or to 0 if it was 
            |       |           |           |                       | unset
        Y   |   20  | 9XY0      | Cond      | if (Vx != Vy)         | Skips the next instruction if VX does not 
            |       |           |           |                       | equal VY
        Y   |   21  | ANNN      | MEM       | I = NNN               | Sets I to the address NNN
        Y   |   22  | BNNN      | Flow      | PC = V0 + NNN         | Jumps to the address NNN plus V0
        Y   |   23  | CXNN      | Rand      | Vx = rand() & NN      | Sets VX to the result of a bitwise AND on
            |       |           |           |                       | a random rumber and NN 
        Y   |   24  | DXYN      | Display   | draw(Vx, Vy, N)       | Draws a sprite at coordinate (VX, VY) that 
            |       |           |           |                       | has a width of 8px and height of Npx. Each
            |       |           |           |                       | row of 8 pixels is read as bit-coded 
            |       |           |           |                       | starting from memory location I. I values 
            |       |           |           |                       | does not change after exectution. VF is 
            |       |           |           |                       | set to 1 if any screen pixels are flipped 
            |       |           |           |                       | from set to unset when set to unset when 
            |       |           |           |                       | the sprite is drawn, and 0 otherwise
        Y   |   25  | EX9E      | KeyOp     | if (key() == Vx)      | Skips the next instruction if the key 
            |       |           |           |                       | stored in VX(only consider the lowest 
            |       |           |           |                       | nibble) is pressed
        Y   |   26  | EXA1      | KeyOp     | if (key() != Vx)      | Skips the next instruction if the key 
            |       |           |           |                       | stored in VX(only consided the lowest 
            |       |           |           |                       | nibble) is not pressed
        Y   |   27  | FX07      | Timer     | Vx = get_delay()      | Sets VX to the value of the delay timer
        Y   |   28  | FX0A      | KeyOp     | Vx = get_key()        | A key press is awaited, and then stored 
            |       |           |           |                       | in VX (blocking opration, all instruction 
            |       |           |           |                       | halted until next key event, delay and 
            |       |           |           |                       | sound timers should continue processing)
        Y   |   29  | FX15      | Timer     | delay_timer(Vx)       | Sets the delay timer to VX
        Y   |   30  | FX18      | Sound     | sound_timer(Vx)       | Sets the sound timer to VX
        Y   |   31  | FX1E      | MEM       | I += Vx               | Adds VX to I. VF is not affected
            |   32  | FX29      | MEM       | I = sprite_addr[Vcx]  | Sets I to the location of the sprite for 
            |       |           |           |                       | the character in VX(only consider the 
            |       |           |           |                       | lowest nibble) Characters 0-F are 
            |       |           |           |                       | represented by a 4x5 font
            |   33  | FX33      | BCD       | set_BCD(Vx)           | Stores the BCD representation of VX, with 
            |       |           |           | *(I+0) =              | the hundreds digit in memory at location 
            |       |           |           | BCD(3);               | in I, the tens at location I+1, and the 
            |       |           |           | *(I+1) =              | ones digit at location I+2
            |       |           |           | BCD(2);               |
            |       |           |           | *(I+2) =              |
            |       |           |           | BCD(1);               |
            |   34  | FX55      | MEM       | reg_dump(Vx, &I)      | Stores from V0 to VX (inclusive) in memory, 
            |       |           |           |                       | starting at address I, The offset from I is
            |       |           |           |                       | increased by 1 for each value written, 
            |       |           |           |                       | but I is not modified
            |   35  | FX65      | MEM       | reg_load(Vx, &I)      | Fills from V0 to VX (inclusive) with values 
            |       |           |           |                       | from memory, starting at address I. The 
            |       |           |           |                       | offset from I is increased by 1 for each 
            |       |           |           |                       | value read, but I is not modified
*/

/*
    Keyboard Mapping

    Chip8 Keypad Layout -> WASD Keyboard
            123C                1234
            456D                QWER
            789E                ASDF
            A0BF                ZXCV
*/