#pragma once

#include "display_buffer.h"

namespace chip8
{
	struct display
	{
		using display_buffer = chip8::display_buffer<64, 32>;

		virtual ~display() = default;

		virtual void update(const display_buffer & buffer) = 0;

		virtual void render() = 0;
	};
}