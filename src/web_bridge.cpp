#ifdef __EMSCRIPTEN__

#include <emscripten/emscripten.h>

#include "emulator.hpp"

namespace
{
    Emulator* active_emulator = nullptr;
}

void SetActiveEmulator(Emulator* emulator)
{
    active_emulator = emulator;
}

extern "C"
{
    EMSCRIPTEN_KEEPALIVE
    int load_rom_from_path(const char* path)
    {
        if (active_emulator == nullptr || path == nullptr)
        {
            return 0;
        }

        return active_emulator->LoadRom(path) ? 1 : 0;
    }
}

#endif