#pragma once

#include <SDL3/SDL.h>
#include <unordered_map>
#include "window.hpp"

class EventHandler
{
    private:
        SDL_Event _event;
        Window* _window = nullptr;
        std::unordered_map<SDL_Keycode, uint8_t> _key_map;
    public:
        EventHandler(Window* window);
        ~EventHandler();
        void ProcessEvents(Chip8& chip8);
        void SetWindow(Window* window);
        void InitKeyMap();
};