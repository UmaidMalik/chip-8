#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <iostream>
#include "chip8.hpp"
#include "logger.hpp"

int main(int argc, char* argv[])
{
    logger::Info("Hello World\n");

    Chip8 chip8;
    chip8.GetRegisters()[0] = 0xBB;
    chip8.GetRegisters()[1] = 0xFF;
    uint8_t* my = chip8.GetRegisters();
    my[0x4] = 0xFF;
    my[0xF] = 0xFE;
    my[0xB] = 0xFE;
    my[0xA] = 0xBF;
    my[0xC] = 0xCE;
    chip8._memory[chip8._pc] = 0x3B;//0xA2;
    chip8._memory[chip8._pc + 1] = 0x70;//0xF0;
    chip8.Cycle();
    logger::Debug("opcode: {:04X}", chip8._opcode);
    chip8.Debug_Print(PrintMode::Hex);


    if (!SDL_Init(SDL_INIT_VIDEO))
    {

        logger::Info("SDL_Init failed: {}", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Chip-8", 20*64, 20*32, SDL_WINDOW_RESIZABLE);
    if (!window)
    {
        logger::Info("Failed to create window: {}", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer)
    {
        logger::Info("Failed to crete renderer: {}", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    bool running = true;
    SDL_Event event;

    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                running = false;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 64, 128, 255);
        SDL_RenderClear(renderer);

        for (int i = 0; i < 20; i++)
        {
            int offset = 5 * i;
            SDL_SetRenderDrawColor(renderer, 255 - offset, 0 + offset * 1.2, 0 + offset, 255);
            SDL_FRect rect = { float(300 + offset), float(200 - offset), float(200 + offset), float(150 - offset) };
            SDL_RenderFillRect(renderer, &rect);
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

