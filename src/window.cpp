#include "window.hpp"
#include <chrono>
#include "chip8.hpp"
#include "logger.hpp"

Window::Window()
{
    logger::Info("Window constructor called");
}

Window::~Window()
{
    logger::Info("Window destructor called");
    SDL_DestroyTexture(_gfx_texture);
    SDL_DestroyRenderer(_renderer);
    SDL_DestroyWindow(_window);
    SDL_Quit();
}

bool Window::Init()
{
    return SDL_Init(SDL_INIT_VIDEO) ? true : (logger::Info("SDL_Init failed: {}", SDL_GetError()), false);
}

bool Window::CreateWindow()
{
    _window = SDL_CreateWindow("Chip-8", WINDOW_SCALE*W, WINDOW_SCALE*H, SDL_WINDOW_RESIZABLE);
    return _window ? true :
    (
        logger::Error("Failed to create window: {}", SDL_GetError()),
        _running = false,
        false
    );
}

bool Window::CreateRenderer()
{ 
    _renderer = SDL_CreateRenderer(_window, nullptr);
    return _renderer ? true : 
    (
        logger::Error("Failed to create renderer: {}", SDL_GetError()),
        _running = false,
        false
    );
}

bool Window::CreateTexture()
{
    _gfx_texture = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, W, H);
    if(!_gfx_texture)
    {
        logger::Error("Failed to create graphics texture: {}", SDL_GetError());
        _running = false;
        return false;
    }

    SDL_SetTextureScaleMode(_gfx_texture, SDL_SCALEMODE_NEAREST);
    SDL_SetRenderLogicalPresentation(_renderer, W, H, SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);

    return true;
}

bool Window::Setup()
{
    _running = true;
    return (Init() && CreateWindow() && CreateRenderer() && CreateTexture()) 
    ? true : (logger::Error("Window setup failed"), false);
}

bool Window::Running()
{
    return _running;
}

void Window::Exit()
{
    _running = false;
}

SDL_Window* Window::GetWindow()
{
    return _window;
}

SDL_Renderer* Window::GetRenderer()
{
    return _renderer;
}

SDL_Texture* Window::GetGfxTexture()
{
    return _gfx_texture;
}



