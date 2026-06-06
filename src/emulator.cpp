#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#include "emulator.hpp"
#include <iostream>
#include <unordered_map>
#include <chrono>
#include "logger.hpp"
#include "chip8.hpp"
#include "window.hpp"
#include "event_handler.hpp"

Emulator::Emulator() : _event_handler(&_window)
{
    logger::Info("Chip8 constructor called");
}

Emulator::~Emulator()
{
    logger::Info("Chip8 destructor called");
}

#ifdef __EMSCRIPTEN__ 

namespace
{
    void BrowserMainLoop(void* argument)
    {
        auto* emulator = static_cast<Emulator*>(argument);
        emulator->Tick();
    }
}

void Emulator::Run()
{
    emscripten_set_main_loop_arg(BrowserMainLoop, this, 0, true);
}

#else

void Emulator::Run()
{
    while (_window.Running())
    {
        Tick();
        //chip8.Debug_Print();
        //chip8.Debug_PrintGfx();
    }
}

#endif

void Emulator::Tick()
{
    _event_handler.ProcessEvents(_chip8);

    const auto current = std::chrono::steady_clock::now();
    const double deltaTime =
        std::chrono::duration<double>(current - prev).count();

    prev = current;

    cpu_accum += deltaTime;
    frame_accum += deltaTime;

    while (cpu_accum >= SEC_PER_CPU)
    {
        _chip8.Cycle();
        cpu_accum -= SEC_PER_CPU;
    }

    if (frame_accum >= SEC_PER_FRAME)
    {
        _chip8.MaybeTick_vblank();
        frame_accum -= SEC_PER_FRAME;

        UploadGrid(fg_color, bg_color);

        SDL_RenderClear(_window.GetRenderer());
        SDL_RenderTexture(
            _window.GetRenderer(),
            _window.GetGfxTexture(),
            nullptr,
            nullptr
        );
        SDL_RenderPresent(_window.GetRenderer());
    }
}

bool Emulator::LoadRom(const std::string& filename)
{
    if (!_chip8.LoadROM(filename))
    {
        logger::Error("Emulator failed to load ROM: {}", filename);
        return false;
    }

    cpu_accum = 0.0;
    frame_accum = 0.0;
    prev = std::chrono::steady_clock::now();

    logger::Info("Loaded ROM: {}", filename);
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

bool Emulator::Setup(const std::string& rom_path)
{
    logger::Info("Welcome to CHIP-8");

    if (!_window.Setup())
    {
        logger::Error("Window Setup() indicates failure has occurred");
        return false;
    }

    if (!LoadRom(rom_path))
    {
        logger::Error("Emulator failed to load ROM");
        return false;
    }

    return true;
}

