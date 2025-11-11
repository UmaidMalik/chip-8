#include "chip8.hpp"
#include <cstdint>
#include <iostream>
#include <bitset>
#include <cstdio>
#include <chrono>
#include "logger.hpp"
#include "random.hpp"

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
        logger::Error("Failed to open ROM: {}", filename);
        logger::Error("fopen resulted in nullptr");
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

    if (_waiting_for_vblank)
    {
        MaybeTick_vblank();
        return;
    }

    
    if (_waiting_for_key)
    {
        UpdateTimers();
        return;
    }

    Fetch();
    Decode();
    /*
    logger::Debug("opcode: {:04X}", _opcode);
    logger::Debug("instruction: {:X}", _opcode >> 12);
    logger::Debug("NNN: {:03X}", _NNN);
    logger::Debug("NN: {:02X}", __NN);
    logger::Debug("X: {:02X}", _X);
    logger::Debug("Y: {:02X}", _Y);
    */
    Execute();
    MaybeTick_vblank();
}

void Chip8::Update()
{
    
}

void Chip8::Fetch()
{
    _opcode = (_memory[_pc] << 8) | _memory[_pc + 1];
}

void Chip8::Decode()
{
    _NNN = 0x0FFF & _opcode;
    __NN = 0x00FF & _opcode;
    ___N = 0x000F & _opcode;
    Get_X();
    Get_Y();
}

void Chip8::Execute()
{
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
    uint8_t _waiting_register = -1;
    InitFontset();
    _next_vblank = std::chrono::steady_clock::now();
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
    
    if (_waiting_for_key && _waiting_register != -1)
    {
        _V[_waiting_register] = k;
    }
    
}

void Chip8::OnKeyReleased(uint8_t k)
{
    _key[k] = 0x0;
    if (_waiting_for_key)
    {
        IncrementProgramCounter();
    }
    _waiting_for_key = false;
    _waiting_register = -1;
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
        if (_sp >= 0)
        {
            _sp--;
        }
        _pc = _stack[_sp];
        _stack[_sp] = 0;
        IncrementProgramCounter();
    }
    else
    {
        logger::Debug("This opcode {:04X}, is of 0x0NNN, and is ignored", _opcode);
        IncrementProgramCounter();
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
    uint8_t Vx = _V[_X];
    uint8_t Vy = _V[_Y];
    switch (option)
    {
        case 0x8000:
            _V[_X] = _V[_Y];
            break;
        case 0x8001:
            _V[_X] |= _V[_Y];
            VF_FlagClear(); // chip-8 compatibility
            break;
        case 0x8002:
            _V[_X] &= _V[_Y];
            VF_FlagClear(); // chip-8 compatibility
            break;
        case 0x8003:
            _V[_X] ^= _V[_Y];
            VF_FlagClear(); // chip-8 compatibility
            break;
        case 0x8004:
            _V[_X] += _V[_Y];
            (Vx + Vx > 255) ? VF_Flag() : VF_FlagClear();
            break;
        case 0x8005:
        {
            _V[_X] = Vx - Vy;
            (Vx >= Vy) ? VF_Flag() : VF_FlagClear();
            // doing this ordering to pass test cases because some ROMs use VF as VX
            break;
        }
        case 0x8006:
        {
            _V[_X] = _V[_Y]; // chip-8 compatibility
            Vx = _V[_X] & 0x01;
            _V[_X] >>= 1;
            _V[0xF] = Vx;
            break;
        } 
        case 0x8007:
        {
            _V[_X] = Vy - Vx;
            (Vy >= Vx) ? VF_Flag() : VF_FlagClear();
            break;
        }
        case 0x800E:
        {
            _V[_X] = _V[_Y]; // chip-8 compatibility
            Vx = (_V[_X] >> 7) & 0x01;
            _V[_X] <<= 1;
            _V[0xF] = Vx;
            break;
        }
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
    uint16_t base_x = _V[_X] % W;
    uint16_t base_y = _V[_Y] % H;
    auto idx = [&](int x, int y)
    {
        return (y * W) + x;
    };
    _V[0xF] = 0x0;
    for (size_t row = 0; row < ___N; row++)
    {
        uint8_t sprite = _memory[(_I + row) & 0xFFF];
        for (size_t col = 0; col < 8; col++)
        {
            if ((sprite & (0x80 >> col)) == 0)
            {
                continue;
            }

            int x = static_cast<int>(base_x) + static_cast<int>(col);
            int y = static_cast<int>(base_y) + static_cast<int>(row);
            
            int k; //= idx((x % W, y % H);
            
            // clipping
            if (x < 0 || x >= W || y < 0 || y >= H)
            {
                continue;
            }

            k = idx(x, y);

            if (_gfx[k] == 0x1)
            {
                VF_Flag();
            }
            _gfx[k] ^= 1;
            
        }
    }
    _waiting_for_vblank = true;
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
        --_delay_timer;
    }
}

void Chip8::UpdateSoundTimer()
{
    if (_sound_timer > 0)
    {
        --_sound_timer;
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

void Chip8::MaybeTick_vblank()
{
    using namespace std::chrono;
    auto now = steady_clock::now();
    if (now >= _next_vblank)
    {
        UpdateTimers();

        _waiting_for_vblank = false;

        _next_vblank += duration_cast<steady_clock::duration>(duration<double>(1.0 / 60.0));
    
        while (now > _next_vblank)
        {
            _next_vblank += duration_cast<steady_clock::duration>(duration<double>(1.0 / 60.0));
        }
    }
}