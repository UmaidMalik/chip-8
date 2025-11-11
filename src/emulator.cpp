#include "emulator.hpp"
#include <iostream>
#include <unordered_map>
#include <chrono>
#include "logger.hpp"
#include "chip8.hpp"
#include "window.hpp"
#include "event_handler.hpp"

Emulator::Emulator()
{
    logger::Info("Chip8 constructor called");
}

Emulator::~Emulator()
{
    logger::Info("Chip8 destructor called");
}

bool Emulator::Setup()
{
    logger::Info("Welcome to CHIP-8\n");

    if (!_window.Setup())
    {
        logger::Error("Window Setup() indicates failure has occurred");
        return false;
    }

    _event_handler.SetWindow(&_window);

    if (!LoadRom(""))
    {
        logger::Error("Emulator failed to load ROM");
        return false;
    }

    return true;
}

void Emulator::Run()
{
    while (_window.Running())
    {
        _event_handler.ProcessEvents(_chip8);

        auto curr = std::chrono::steady_clock::now();
        double delta_time = std::chrono::duration<double>(curr - prev).count();
        prev = curr;

        cpu_accum += delta_time;
        frame_accum += delta_time;

        while (cpu_accum >= SEC_PER_CPU)
        {
            _chip8.Cycle();
            cpu_accum -= SEC_PER_CPU;
        }

        while (frame_accum >= SEC_PER_FRAME)
        {
            _chip8.MaybeTick_vblank();
            frame_accum -= SEC_PER_FRAME;
        }

        UploadGrid(0xF0FF00FF, 0x000000FF);
        SDL_RenderClear(_window.GetRenderer());
        SDL_RenderTexture(_window.GetRenderer(), _window.GetGfxTexture(), nullptr, nullptr);
        SDL_RenderPresent(_window.GetRenderer());



        //chip8.Debug_Print();
        //chip8.Debug_PrintGfx();
    }
}

bool Emulator::LoadRom(const std::string& filename)
{
    bool load_result;
    //load_result = _chip8.LoadROM("roms/1-chip8-logo.ch8");
    //load_result = _chip8.LoadROM("roms/2-ibm-logo.ch8");
    //load_result = _chip8.LoadROM("roms/3-corax+.ch8");
    //load_result = _chip8.LoadROM("roms/4-flags.ch8");
    //load_result = _chip8.LoadROM("roms/5-quirks.ch8");
    //load_result = _chip8.LoadROM("roms/6-keypad.ch8");
    //load_result = _chip8.LoadROM("roms/7-beep.ch8");
    //load_result = _chip8.LoadROM("roms/CONNECT4"); 
    //load_result = _chip8.LoadROM("roms/Breakout.ch8");
    //load_result = _chip8.LoadROM("roms/PONG2"); //<= used opcode 0000 !
    //load_result = _chip8.LoadROM("roms/15PUZZLE");
    load_result = _chip8.LoadROM("roms/GUESS");
    if (!load_result)
    {
        logger::Error("Emulator failed to load ROM");
        return false;
    }
    return true;
}

void Emulator::UploadGrid(uint32_t fg, uint32_t bg)
{
    void* pixels;
    int pitch;
    if (!SDL_LockTexture(_window.GetGfxTexture(), nullptr, &pixels, &pitch))
    {
        logger::Error("LockTexture failed: {}", SDL_GetError());
        return;
    }

    uint32_t* row = (uint32_t*)pixels;
    int stride = pitch / 4;
    const uint8_t *g = _chip8.GetGfx();
    for (int y = 0; y < H; ++y) {
        uint32_t *dst = row + y * stride;
        for (int x = 0; x < W; ++x) {
            uint32_t mask = (uint32_t)-((int)g[x]); 
            dst[x] = bg ^ ((bg ^ fg) & mask);
        }
        g += W;
    }
    SDL_UnlockTexture(_window.GetGfxTexture());
}
