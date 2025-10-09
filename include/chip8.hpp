#pragma once
#include <cstdint>
#include <iostream>
#include <bitset>
#include <cstdio>
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
constexpr static uint16_t rom_start = 0x200;

class Chip8
{
    public: // <--- change back to private after
        uint8_t _memory[4096];
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
        uint8_t _waiting_register = 0x0;
        bool _draw_flag = false;
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
};

Chip8::Chip8()
{
    logger::Info("Chip8 constructor called");
    Reset();
}

Chip8::~Chip8()
{
    logger::Info("Chip8 destructor called");
}

bool Chip8::LoadROM(const std::string& filename)
{
    Reset(); // clear current RAM
    FILE* file = fopen(filename.c_str(), "rb");
    bool result = true;
    if(file == nullptr)
    {
        logger::Error("Failed to open ROM: fopen resulted in nullptr");
        return false;
    }
    std::fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    std::fseek(file, 0, SEEK_SET); 
    
    uint8_t* buffer = new uint8_t[file_size];

    size_t bytes_read = fread(buffer, 1, file_size, file);

    if (bytes_read == file_size)
    {
        for (size_t i = 0; i < bytes_read; i++)
        {
            _memory[rom_start + i] = buffer[i];
        }
    }
    else
    {
        logger::Error("Failed to read ROM");
        result = false;
    }

    delete[] buffer;
    fclose(file);
    return result;
}

void Chip8::Cycle()
{
    _opcode = (_memory[_pc] << 8) | _memory[_pc + 1];
    
    _NNN = 0x0FFF & _opcode;
    __NN = 0x00FF & _opcode;
    ___N = 0x000F & _opcode;
    Get_X();
    Get_Y();
    /*
    logger::Debug("opcode: {:04X}", _opcode);
    logger::Debug("instruction: {:X}", _opcode >> 12);
    logger::Debug("NNN: {:03X}", _NNN);
    logger::Debug("NN: {:02X}", __NN);
    logger::Debug("X: {:02X}", _X);
    logger::Debug("Y: {:02X}", _Y);
    */
    switch (_opcode >> 12)
    {
        case 0x0:
            Execute_0x0();
            break;
        case 0x1:
            Execute_0x1();
            break;
        case 0x2:
            Execute_0x2();
            break;
        case 0x3:
            Execute_0x3();
            break;
        case 0x4:
            Execute_0x4();
            break;
        case 0x5:
            Execute_0x5();
            break;
        case 0x6:
            Execute_0x6();
            break;
        case 0x7:
            Execute_0x7();
            break;
        case 0x8:
            Execute_0x8();
            break;
        case 0x9:
            Execute_0x9();
            break;
        case 0xA:
            Execute_0xA();
            break;
        case 0xB:
            Execute_0xB();
            break;
        case 0xC:
            Execute_0xC();
            break;
        case 0xD:
            Execute_0xD();
            break;
        case 0xE:
            Execute_0xE();
            break;
        case 0xF:
            Execute_0xF();
            break;
    }
    UpdateTimers();
}

bool Chip8::DrawFlag()
{
    return _draw_flag;
}

void Chip8::Reset()
{
    std::fill(std::begin(_memory), std::end(_memory), 0);
    std::fill(std::begin(_V), std::end(_V), 0);
    _I = 0;
    _pc = rom_start;
    _delay_timer = 0;
    _sound_timer = 0;
    std::fill(std::begin(_stack), std::end(_stack), 0);
    _sp = 0;
    std::fill(std::begin(_key), std::end(_key), 0);
    std::fill(std::begin(_gfx), std::end(_gfx), 0);
    _opcode = 0;
    _draw_flag = false;
    bool _waiting_for_key = false;
    uint8_t _waiting_register = 0x0;
    InitFontset();
}

void Chip8::ClearScreen()
{
    std::fill(std::begin(_gfx), std::end(_gfx), 0);
}

void Chip8::Debug_Print(PrintMode pm = PrintMode::Hex)
{
    int n = V_Size() / 2;
    for (int i = 0; i < n; i++)
    {
        switch(pm)
        {
            case PrintMode::Dec:
                logger::Print("V{:X}: {:03}     V{:X}: {:03}\n", i, (_V[i]), i+n, (_V[i+n]));
                break;
            case PrintMode::Hex:
                logger::Print("V{:X}: {:02X}     V{:X}: {:02X}\n", i, (_V[i]), i+n, (_V[i+n]));
                break;
            case PrintMode::Bin:
                logger::Print("V{:X}: {:08b}     V{:X}: {:08b}\n", i, (_V[i]), i+n, (_V[i+n]));
                break;
        }
    }
    logger::Print("\n");
    switch(pm)
    {
        case PrintMode::Dec:
            logger::Print("I: {:05}   PC: {:05}\n", _I, _pc);
            logger::Print("Delay Timer: {:03}   Sound Timer: {:03}\n", _delay_timer, _sound_timer);
            logger::Print("SP: {:05}\n", _sp);
            break;
        case PrintMode::Hex:
            logger::Print("I: {:04X}   PC: {:04X}\n", _I, _pc);
            logger::Print("Delay Timer: {:02X}   Sound Timer: {:02X}\n", _delay_timer, _sound_timer);
            logger::Print("SP: {:04X}\n", _sp);
            break;
        case PrintMode::Bin:
            logger::Print("I: {:016b}   PC: {:016b}\n", _I, _pc);
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
                logger::Print("Key{:X}: {}     Key{:X}: {}    Key{:X}: {}    Key{:X}: {}\n", i, _key[i], i+n, _key[i+n], i+(n*2), _key[i+(n*2)], i+(n*3), _key[i+(n*3)]);
                break;
            case PrintMode::Hex:
                logger::Print("Key{:X}: {:01X}     Key{:X}: {:01X}    Key{:X}: {:01X}    Key{:X}: {:01X}\n", i, _key[i], i+n, _key[i+n], i+(n*2), _key[i+(n*2)], i+(n*3), _key[i+(n*3)]);
                break;
            case PrintMode::Bin:
                logger::Print("Key{:X}: {:04b}     Key{:X}: {:04b}    Key{:X}: {:04b}    Key{:X}: {:04b}\n", i, _key[i], i+n, _key[i+n], i+(n*2), _key[i+(n*2)], i+(n*3), _key[i+(n*3)]);
                break;
        }
    }
}

void Chip8::Debug_PrintGfx()
{
    auto idx = [&](int x, int y)
    {
        return (y * W) + x;
    };
    for (size_t col = 0; col < W; col++)
    {
        logger::Print("_");
    }
    logger::Print("\n");
    for (size_t row = 0; row < H; row++)
    {
        logger::Print("|");
        for (size_t col = 0; col < W; col++)
        {
            int i = idx(col, row);
            if (_gfx[i] != 0)
            {
                logger::Print("X");
            }
            else
            {
                logger::Print(" ");
            }
        }
        logger::Print("|\n");
    }
    for (size_t col = 0; col < W; col++)
    {
        logger::Print("_");
    }
    logger::Print("\n");
}

int Chip8::V_Size()
{
    return sizeof(_V) / sizeof(_V[0]);
}

int Chip8::KeySize()
{
    return sizeof(_key) / sizeof(_key[0]);
}

int Chip8::StackSize()
{
    return sizeof(_stack) / sizeof(_stack[0]);
}

int Chip8::FontsetSize()
{
return sizeof(_fontset) / sizeof(_fontset[0]);
}

void Chip8::IncrementProgramCounter(int n = 1)
{
    for (int i = 0; i < n; i++)
    {
        _pc += 2;
    }
}

uint8_t (&Chip8::GetRegisters())[16]
{
    return _V;
}

uint8_t (&Chip8::GetGfx())[W * H]
{
    return _gfx;
}

uint8_t& Chip8::GetDelayTimer()
{
    return _delay_timer;
}

uint8_t& Chip8::GetSoundTimer()
{
    return _sound_timer;
}

void Chip8::SetDelayTimer(uint8_t t)
{
    _delay_timer = t;
}

void Chip8::SetSoundTimer(uint8_t t)
{
    _sound_timer = t;
}

void Chip8::SetKey(uint8_t k)
{
    _key[k] = 0x1;
}

void Chip8::UnsetKey(uint8_t k)
{
    _key[k] = 0x0;
}

void Chip8::OnKeyPressed(uint8_t k)
{
    _key[k] = 0x1;

    if (_waiting_for_key)
    {
        _V[_waiting_register] = k;
        _waiting_for_key = false;
        IncrementProgramCounter();
    }
}

void Chip8::OnKeyReleased(uint8_t k)
{
    _key[k] = 0x0;
}

void Chip8::Execute_0x0()
{
    if (_opcode == 0x00E0)
    {
        ClearScreen();
        IncrementProgramCounter();
    }
    else if (_opcode == 0x00EE)
    {
        //_stack[_sp] = 0x0;
        _sp--;
        _pc = _stack[_sp];
        IncrementProgramCounter();
    }
    else
    {
        logger::Debug("This opcode {:04X}, is of 0x0NNN, and is ignored", _opcode);
    }
}

void Chip8::Execute_0x1()
{
    _pc = _NNN;
}

void Chip8::Execute_0x2()
{
    if (_sp >= StackSize())
    {
        logger::Error("Stack Overflow: sp={:0X}", _sp);
    }
    _stack[_sp] = _pc;
    _sp++;
    _pc = _NNN;
}

void Chip8::Execute_0x3()
{
    uint8_t n = 1;
    uint16_t X = 0x0F00 & _opcode;
    X >>= 8;
    if (_V[X] == __NN)
    {
        n = 2; // skips the next instruction
    }
    IncrementProgramCounter(n);
}

void Chip8::Execute_0x4()
{
    uint8_t n = 1;
    if (_V[_X] != __NN)
    {
        n = 2; // skips the next instruction
    }
    IncrementProgramCounter(n);
}

void Chip8::Execute_0x5()
{
    uint8_t n = 1;
    if (_V[_X] == _V[_Y])
    {
        n = 2; // skips the next instruction
    }
    IncrementProgramCounter(n);
}

void Chip8::Execute_0x6()
{
    _V[_X] = __NN;
    IncrementProgramCounter();
}

void Chip8::Execute_0x7()
{
    _V[_X] += __NN;
    IncrementProgramCounter();
}

void Chip8::Execute_0x8()
{

    u_int16_t option = 0xF00F & _opcode;
    switch (option)
    {
        case 0x8000:
            _V[_X] = _V[_Y];
            break;
        case 0x8001:
            _V[_X] |= _V[_Y];
            VF_FlagClear();
            break;
        case 0x8002:
            _V[_X] &= _V[_Y];
            VF_FlagClear();
            break;
        case 0x8003:
            _V[_X] ^= _V[_Y];
            VF_FlagClear();
            break;
        case 0x8004:
            _V[_X] += _V[_Y];
            (_V[_Y] > _V[_X]) ? VF_Flag() : VF_FlagClear();
            break;
        case 0x8005:
        {
            uint8_t Vx = _V[_X];
            uint8_t Vy = _V[_Y];
            _V[_X] = Vx - Vy;
            (Vx >= Vy) ? VF_Flag() : VF_FlagClear();
            // doing this ordering to pass test cases because some ROMs use VF as VX
            break;
        }
        case 0x8006:
            _V[_X] = _V[_Y];
            _V[0xF] = 0x01 & _V[_X];
            _V[_X] >>= 1;
            break;
        case 0x8007:
        {
            uint8_t Vx = _V[_X];
            uint8_t Vy = _V[_Y];
            _V[_X] = Vy - Vx;
            (Vy >= Vx) ? VF_Flag() : VF_FlagClear();
            break;
        }
        case 0x800E:
            _V[_X] = _V[_Y];
            _V[0xF] = (_V[_X] >> 7) & 0x01;
            _V[_X] <<= 1;
            break;
        default:
            logger::Warn("Unknown opcode {:04X}", _opcode);
            break;
    }
   IncrementProgramCounter();
}

void Chip8::Execute_0x9()
{
    uint8_t n = 1;
    if (_V[_X] != _V[_Y])
    {
        n = 2; // skips the next instruction
    }
    IncrementProgramCounter(n);
}

void Chip8::Execute_0xA()
{
    _I = 0x0FFF & _NNN;
    IncrementProgramCounter();
}

void Chip8::Execute_0xB()
{
    _pc = _V[0x0] + _NNN;
}

void Chip8::Execute_0xC()
{
    uint8_t rand_num = rng::RandomNum_8Bit();
    _V[_X] =  rand_num & __NN;
    IncrementProgramCounter();
}

void Chip8::Execute_0xD()
{
    uint16_t pos_x = _V[_X];
    uint16_t pos_y = _V[_Y];
    auto idx = [&](int x, int y)
    {
        return (y * W) + x;
    };
    //_V[0xF] = 0x0;
    for (size_t row = 0; row < ___N; row++)
    {
        uint8_t sprite = _memory[_I + row];
        for (size_t col = 0; col < 8; col++)
        {
            if ((sprite & (0x80 >> col)) != 0)
            {
                int k = idx((pos_x + col) % W, (pos_y + row) % H);
                
                if (_gfx[k] == 0x1)
                {
                    _V[0xF] = 0x1;
                }
                
                _gfx[k] ^= 1;
            }
        }
    }
    IncrementProgramCounter();
}

void Chip8::Execute_0xE()
{
   uint8_t k = _V[_X];
   k &= 0x0F;    // Only use lowest nibble of VX as key index
   uint8_t n = 1;
   switch (_opcode & 0x00FF)
   {
        case 0x9E:
            if (_key[k] == 0x1)
            {
                n = 2;
            }
            break;
        case 0xA1:
            if (_key[k] != 0x1)
            {
                n = 2;
            }
            break;
        default:
            logger::Warn("Unknown opcode {:04X}", _opcode);
            break;
   }
   IncrementProgramCounter(n);
}

void Chip8::Execute_0xF()
{
    uint8_t n = 1;
    int pressed = -1;
    uint8_t font_char = 0x0;
    switch (_opcode & 0x00FF)
    {
        case 0x07:
            _V[_X] = GetDelayTimer();
            break;
        case 0x0A:    
            _waiting_for_key = true;
            _waiting_register = _X;
            return;
        case 0x15:
            SetDelayTimer(_V[_X]);
            break;
        case 0x18:
            SetSoundTimer(_V[_X]);
            break;
        case 0x1E:
            _I += _V[_X];
            break;
        case 0x29:
            font_char = _V[_X] & 0x0F;
            _I = _fontset_start + (font_char * 0x05);
            break;
        case 0x33:
            _memory[_I] = _V[_X] / 100;
            _memory[_I + 1] = (_V[_X] / 10) % 10;
            _memory[_I + 2] = _V[_X] % 10;
            break;
        case 0x55:
            for (int X = 0; X <= _X; X++)
            {
                _memory[_I + X] = _V[X];
            }
            _I += _X + 1;
            break;
        case 0x65:
            for (int X = 0; X <= _X; X++)
            {
                _V[X] = _memory[_I + X];
            }
            _I += _X + 1;
            break;
        default:
            logger::Warn("Unknown opcode {:04X}", _opcode);
            break;
    }
    IncrementProgramCounter(n);
}

void Chip8::Get_X()
{
    _X = (_opcode & 0x0F00) >> 8;
}

void Chip8::Get_Y()
{
    _Y = (_opcode & 0x00F0) >> 4;
}

void Chip8::VF_Flag()
{
    _V[0xF] = 1;
}

void Chip8::VF_FlagClear()
{
    _V[0xF] = 0;
}

void Chip8::UpdateDelayTimer()
{
    if (_delay_timer > 0)
    {    
        _delay_timer--;
    }
}

void Chip8::UpdateSoundTimer()
{
    if (_sound_timer > 0)
    {
        _sound_timer--;
    }
}

void Chip8::UpdateTimers()
{
    UpdateDelayTimer();
    UpdateSoundTimer();
}

void Chip8::InitFontset()
{
    for (int i = 0; i < FontsetSize(); i++)
    {
        _memory[i] = _fontset[i];
    }
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