#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <chrono>
#include "chip8.hpp"
#include "logger.hpp"

extern constexpr int WINDOW_SCALE = 20;

class Window
{
    private:
        SDL_Window* _window;
        SDL_Renderer* _renderer;
        SDL_Texture* _gfx_texture;
        bool _running = false;
    public:
        Window();
        ~Window();
        bool Init();
        bool CreateWindow();
        bool CreateRenderer();
        bool CreateTexture();
        bool Setup();
        bool Running();
        void Exit();
        SDL_Window* GetWindow();
        SDL_Renderer* GetRenderer();
        SDL_Texture* GetGfxTexture();
};