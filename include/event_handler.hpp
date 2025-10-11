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

EventHandler::EventHandler(Window* window = nullptr)
    : _window(window)
{
    logger::Info("EventHandler constructor called");
    InitKeyMap();
}

EventHandler::~EventHandler()
{
    _window = nullptr;
    logger::Info("~EventHandler destructor called");
}

void EventHandler::ProcessEvents(Chip8& chip8)
{
    while (SDL_PollEvent(&_event))
    {
        auto it = _key_map.find(_event.key.key);
        switch(_event.type)
        {
            
            case SDL_EVENT_QUIT:
                _window->Exit();
                break;
            case SDL_EVENT_KEY_DOWN:
                if (it != _key_map.end())
                {
                    chip8.OnKeyPressed(it->second);
                }
                else
                {
                    logger::Debug("Not in map: {}", SDL_GetKeyName(_event.key.key));
                }
                break;
            case SDL_EVENT_KEY_UP:
                if (it != _key_map.end())
                {
                    chip8.OnKeyReleased(it->second);
                }
                else
                {
                    logger::Debug("Not in map: {}", SDL_GetKeyName(_event.key.key));
                }
                break;
        }
    }

}

void EventHandler::SetWindow(Window* window)
{
    _window = window;
}

void EventHandler::InitKeyMap()
{
    _key_map[SDLK_1] = 0x1; _key_map[SDLK_2] = 0x2; _key_map[SDLK_3] = 0x3; _key_map[SDLK_4] = 0xC;
    _key_map[SDLK_Q] = 0x4; _key_map[SDLK_W] = 0x5; _key_map[SDLK_E] = 0x6; _key_map[SDLK_R] = 0xD;
    _key_map[SDLK_A] = 0x7; _key_map[SDLK_S] = 0x8; _key_map[SDLK_D] = 0x9; _key_map[SDLK_F] = 0xE;
    _key_map[SDLK_Z] = 0xA; _key_map[SDLK_X] = 0x0; _key_map[SDLK_C] = 0xB; _key_map[SDLK_V] = 0xF;
}

