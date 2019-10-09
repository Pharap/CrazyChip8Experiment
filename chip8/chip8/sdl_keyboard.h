#pragma once

#include <unordered_map>

#include <SDL.h>

#include "chip8.h"
#include "sdl_shared.h"

using key_mapping = std::unordered_map<chip8::key_id, SDL_Scancode>;

const key_mapping default_key_mapping
{
	{ chip8::key_id::key_1, SDL_Scancode::SDL_SCANCODE_1 },
	{ chip8::key_id::key_2, SDL_Scancode::SDL_SCANCODE_2 },
	{ chip8::key_id::key_2, SDL_Scancode::SDL_SCANCODE_3 },
	{ chip8::key_id::key_c, SDL_Scancode::SDL_SCANCODE_4 },

	{ chip8::key_id::key_4, SDL_Scancode::SDL_SCANCODE_Q },
	{ chip8::key_id::key_5, SDL_Scancode::SDL_SCANCODE_W },
	{ chip8::key_id::key_6, SDL_Scancode::SDL_SCANCODE_E },
	{ chip8::key_id::key_d, SDL_Scancode::SDL_SCANCODE_R },

	{ chip8::key_id::key_7, SDL_Scancode::SDL_SCANCODE_A },
	{ chip8::key_id::key_8, SDL_Scancode::SDL_SCANCODE_S },
	{ chip8::key_id::key_9, SDL_Scancode::SDL_SCANCODE_D },
	{ chip8::key_id::key_e, SDL_Scancode::SDL_SCANCODE_F },

	{ chip8::key_id::key_a, SDL_Scancode::SDL_SCANCODE_Z },
	{ chip8::key_id::key_0, SDL_Scancode::SDL_SCANCODE_X },
	{ chip8::key_id::key_b, SDL_Scancode::SDL_SCANCODE_C },
	{ chip8::key_id::key_f, SDL_Scancode::SDL_SCANCODE_V },
};

class sdl_keyboard : public chip8::keyboard
{
private:
	const Uint8 * keyboard_state;
	int keyboard_state_length;
	key_mapping mapping = default_key_mapping;

public:
	sdl_keyboard() = default;

	sdl_keyboard(key_mapping mapping) :
		mapping(mapping)
	{
	}

	~sdl_keyboard() override = default;

	void press(SDL_Scancode scancode)
	{
	}

	bool is_pressed(chip8::key_id key) const override
	{
		auto iterator = this->mapping.find(key);

		if(iterator == std::end(this->mapping))
			return false;

		auto scancode = iterator->second;
		auto key_state = this->keyboard_state[scancode];

		return (key_state != 0);
	}

	void update() override
	{
		if(this->keyboard_state == nullptr)
			this->keyboard_state = SDL_GetKeyboardState(&this->keyboard_state_length);
	}
};