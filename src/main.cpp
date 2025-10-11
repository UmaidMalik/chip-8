#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <iostream>
#include <unordered_map>
#include <chrono>
#include "chip8.hpp"
#include "logger.hpp"
#include "window.hpp"
#include "event_handler.hpp"
#include "emulator.hpp"

int main(int argc, char* argv[])
{
    Emulator emulator;
    emulator.Setup();
    emulator.Run();
    return 0;
}