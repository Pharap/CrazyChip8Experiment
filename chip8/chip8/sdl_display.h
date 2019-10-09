#pragma once

#include <SDL.h>

#include "chip8.h"
#include "sdl_shared.h"

class sdl_display : public chip8::display
{
private:
	sdl_renderer_pointer renderer;
	int x;
	int y;
	int pixel_width;
	int pixel_height;

public:
	sdl_display(sdl_renderer_pointer renderer, int x, int y, int pixel_width, int pixel_height) :
		renderer(renderer), x(x), y(y), pixel_width(pixel_width), pixel_height(pixel_height)
	{
	}

	~sdl_display() override = default;

	void update(const display_buffer & buffer) override
	{
		using size_type = typename display_buffer::size_type;

		constexpr SDL_Colour black { 0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE };
		constexpr SDL_Colour white { 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE };

		for(size_type y = 0; y < buffer.get_height(); ++y)
		{
			int draw_y = (this->y + static_cast<int>(y));

			for(size_type x = 0; x < buffer.get_width(); ++x)
			{
				int draw_x = (this->x + static_cast<int>(x));

				SDL_Rect rect { draw_x * this->pixel_width, draw_y * this->pixel_height, this->pixel_width, this->pixel_height };
				
				auto colour = buffer.test(x, y) ? white : black;

				SDL_SetRenderDrawColor(this->renderer.get(), colour.r, colour.g, colour.b, colour.a);
				SDL_RenderFillRect(this->renderer.get(), &rect);
			}
		}
	}

	void render() override
	{
		SDL_RenderPresent(this->renderer.get());
	}
};