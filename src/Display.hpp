#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include <SDL2/SDL.h>
#include <vector>
#include <string>
#include <cstdint>
#include <stdexcept>

class Display {
    SDL_Window*   _window   = nullptr;
    SDL_Renderer* _renderer = nullptr;
    SDL_Texture*  _texture  = nullptr;
    std::vector<uint32_t> _pixels;
    int _w, _h;

public:
    Display(int w, int h, const std::string& title) : _w(w), _h(h), _pixels(w * h, 0) {
        if (SDL_Init(SDL_INIT_VIDEO) != 0)
            throw std::runtime_error(SDL_GetError());

        _window = SDL_CreateWindow(title.c_str(),
                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                      w, h, SDL_WINDOW_SHOWN);
        if (!_window) throw std::runtime_error(SDL_GetError());

        _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);
        if (!_renderer) throw std::runtime_error(SDL_GetError());

        _texture = SDL_CreateTexture(_renderer,
                       SDL_PIXELFORMAT_ARGB8888,
                       SDL_TEXTUREACCESS_STREAMING, w, h);
        if (!_texture) throw std::runtime_error(SDL_GetError());
    }

    ~Display() {
        SDL_DestroyTexture(_texture);
        SDL_DestroyRenderer(_renderer);
        SDL_DestroyWindow(_window);
        SDL_Quit();
    }

    // Write one pixel into the CPU-side buffer (0xFFRRGGBB).
    void setPixel(int col, int row, uint8_t r, uint8_t g, uint8_t b) {
        _pixels[row * _w + col] =
            (uint32_t(0xFF) << 24) |
            (uint32_t(r)    << 16) |
            (uint32_t(g)    <<  8) |
             uint32_t(b);
    }

    // Push the pixel buffer to the window — call once per row for live preview.
    void present() {
        SDL_UpdateTexture(_texture, nullptr, _pixels.data(), _w * sizeof(uint32_t));
        SDL_RenderClear(_renderer);
        SDL_RenderCopy(_renderer, _texture, nullptr, nullptr);
        SDL_RenderPresent(_renderer);
    }

    // Returns true if the user closed the window.
    bool pollQuit() {
        SDL_Event e;
        while (SDL_PollEvent(&e))
            if (e.type == SDL_QUIT) return true;
        return false;
    }

    // Block until the window is closed (call after rendering is done).
    void waitUntilClosed() {
        SDL_Event e;
        while (SDL_WaitEvent(&e))
            if (e.type == SDL_QUIT) break;
    }
};

#endif // DISPLAY_HPP
