#pragma once

#include "keys.h"

namespace chip8
{
	struct keyboard
	{
		virtual ~keyboard() = default;

		virtual bool is_pressed(key_id key) const = 0;

		virtual bool is_released(key_id key) const
		{
			return !this->is_pressed(key);
		}

		virtual void update() = 0;
	};
}
