#pragma once

#include <memory>

#include <SDL.h>

using sdl_window_pointer = std::shared_ptr<SDL_Window>;

sdl_window_pointer sdl_create_window(const char* title, int x, int y, int w, int h, Uint32 flags)
{
	return sdl_window_pointer(SDL_CreateWindow(title, x, y, w, h, flags), SDL_DestroyWindow);
}

using sdl_renderer_pointer = std::shared_ptr<SDL_Renderer>;

sdl_renderer_pointer sdl_create_renderer(sdl_window_pointer window, int index, Uint32 flags)
{
	return sdl_renderer_pointer(SDL_CreateRenderer(window.get(), index, flags), SDL_DestroyRenderer);
}