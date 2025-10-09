#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <iostream>
#include <unordered_map>
#include "chip8.hpp"
#include "logger.hpp"

void UploadGrid(SDL_Texture* tex, const uint8_t* grid, uint32_t fg, uint32_t bg);
const int window_scale = 20;

int main(int argc, char* argv[])
{
    logger::Info("Welcome to CHIP-8\n");
    Chip8 chip8;
    auto chip8_gfx = chip8.GetGfx();
    std::unordered_map<SDL_Keycode, uint8_t> key_map;
    key_map[SDLK_1] = 0x1; key_map[SDLK_2] = 0x2; key_map[SDLK_3] = 0x3; key_map[SDLK_4] = 0xC;
    key_map[SDLK_Q] = 0x4; key_map[SDLK_W] = 0x5; key_map[SDLK_E] = 0x6; key_map[SDLK_R] = 0xD;
    key_map[SDLK_A] = 0x7; key_map[SDLK_S] = 0x8; key_map[SDLK_D] = 0x9; key_map[SDLK_F] = 0xE;
    key_map[SDLK_Z] = 0xA; key_map[SDLK_X] = 0x0; key_map[SDLK_C] = 0xB; key_map[SDLK_V] = 0xF;
    
    //test1
    /*
    chip8._V[1] = 0x50;
    chip8._V[2] = 0x10;
    chip8._memory[0x200] = 0x81;
    chip8._memory[0x201] = 0x25;
    chip8.Cycle();
    chip8.Debug_Print();
    */
    //test2 
    /*
    chip8._V[1] = 0x10;
    chip8._V[2] = 0x20;
    chip8._memory[0x200] = 0x81;
    chip8._memory[0x201] = 0x25;
    chip8.Cycle();
    chip8.Debug_Print();
    */
    //test3 
    
    //chip8._V[1] = 0x00;
    //chip8._V[2] = 0x01;
    //chip8._memory[0x200] = 0x81;
    //chip8._memory[0x201] = 0x25;
    //chip8.Cycle();
    //chip8.Debug_Print();
    

    chip8.LoadROM("roms/4-flags.ch8");
    //chip8.LoadROM("roms/5-quirks.ch8");
    //chip8.LoadROM("roms/br8kout.ch8");
    // test
    /*
    for(int i = 0; i < 39; i++)
    {
        chip8.Cycle();
    }
    */
    //

    

    if (!SDL_Init(SDL_INIT_VIDEO))
    {

        logger::Info("SDL_Init failed: {}", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Chip-8", window_scale*W, window_scale*H, SDL_WINDOW_RESIZABLE);
    if (!window)
    {
        logger::Error("Failed to create window: {}", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer)
    {
        logger::Error("Failed to create renderer: {}", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Texture* gfx_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, W, H);
    if(!gfx_texture)
    {
        logger::Error("Failed to create graphics texture: {}", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_SetTextureScaleMode(gfx_texture, SDL_SCALEMODE_NEAREST);

    SDL_SetRenderLogicalPresentation(renderer, W, H, SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);

    bool running = true;
    SDL_Event event;

    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            auto it = key_map.find(event.key.key);
            switch(event.type)
            {
                
                case SDL_EVENT_QUIT:
                    running = false;
                    break;
                case SDL_EVENT_KEY_DOWN:
                    if (it != key_map.end())
                    {
                        chip8.OnKeyPressed(it->second);
                    }
                    else
                    {
                        logger::Debug("Not in map: {}", SDL_GetKeyName(event.key.key));
                    }
                    break;
                case SDL_EVENT_KEY_UP:
                    if (it != key_map.end())
                    {
                        chip8.OnKeyReleased(it->second);
                    }
                    else
                    {
                        logger::Debug("Not in map: {}", SDL_GetKeyName(event.key.key));
                    }
                    break;
            }
        }

        
        UploadGrid(gfx_texture, chip8_gfx, 0xF0FF00FF, 0x000000FF);
        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, gfx_texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);

        chip8.Cycle();
        //SDL_Delay(16);

        //chip8.Debug_Print();
        //chip8.Debug_PrintGfx();
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

void UploadGrid(SDL_Texture* tex, const uint8_t* grid, uint32_t fg, uint32_t bg)
{
    void* pixels;
    int pitch;
    if (!SDL_LockTexture(tex, nullptr, &pixels, &pitch))
    {
        logger::Error("LockTexture failed: {}", SDL_GetError());
        return;
    }

    uint32_t* row = (uint32_t*)pixels;
    int stride = pitch / 4;
    const uint8_t *g = grid;
    for (int y = 0; y < H; ++y) {
        uint32_t *dst = row + y * stride;
        for (int x = 0; x < W; ++x) {
            // Branchless choose: bg ^ ((bg ^ fg) & mask)
            uint32_t mask = (uint32_t)-((int)g[x]); // 0 -> 0x00000000, 1 -> 0xFFFFFFFF
            dst[x] = bg ^ ((bg ^ fg) & mask);
        }
        g += W;
    }
    SDL_UnlockTexture(tex);
}

