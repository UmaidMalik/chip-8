#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "emulator.hpp"
#include "logger.hpp"

#ifdef __EMSCRIPTEN__
#include "web_bridge.hpp"
#endif

int main(int argc, char* argv[])
{
    static Emulator emulator;

#ifdef __EMSCRIPTEN__
    SetActiveEmulator(&emulator);
    const char* rom_path = "roms/1-chip8-logo.ch8";
#else
    if (argc != 2)
    {
        logger::Error("Usage: Chip8 <rom-file>");
        return 1;
    }

    const char* rom_path = argv[1];
#endif

    if (!emulator.Setup(rom_path))
    {
        logger::Error("Failed to initialize emulator");
        return 1;
    }

    emulator.Run();
    return 0;
}