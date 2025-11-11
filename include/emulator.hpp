#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <chrono>
#include "chip8.hpp"
#include "window.hpp"
#include "event_handler.hpp"



double cpu_accum = 0.0, frame_accum = 0.0;
auto prev = std::chrono::steady_clock::now();

class Emulator
{
    private:
        Chip8 _chip8;
        Window _window;
        EventHandler _event_handler;
        uint32_t bg_color = 0x000000FF;
        uint32_t fg_color = 0xF0FF00FF;
        const double CPU_HZ = 10000.0;
        const double FRAME_HZ = 60.0;
        const double SEC_PER_CPU = 1.0 / CPU_HZ;
        const double SEC_PER_FRAME = 1.0 / FRAME_HZ;
    public:
        Emulator();
        ~Emulator();
        bool Setup();
        void Run();
        bool LoadRom(const std::string& filename);
        void UploadGrid(uint32_t fg, uint32_t bg);
};