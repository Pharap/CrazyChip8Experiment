#pragma once

#include "base_types.h"

namespace chip8
{
	enum class opcode_id : byte
	{
		clear_screen,
		function_return,
		jump_address,
		call_address,
		skip_if_equal_register_immediate,
		skip_if_not_equal_register_immediate,
		skip_if_equal_register_register,
		load_register_immediate,
		add_register_immediate,
		load_register_register,
		or_register_register,
		and_register_register,
		xor_register_register,
		add_register_register,
		subtract_register_register,
		shift_right_register_register,
		reverse_subtract_register_register,
		shift_left_register_register,
		skip_if_not_equal_register_register,
		load_i_immediate,
		jump_address_register_0,
		random_register_immediate,
		draw_x_y_size,
		skip_if_key_pressed_register,
		skip_if_key_not_pressed_register,
		read_delay_timer_register,
		await_key_press_register,
		write_delay_timer_register,
		write_sound_timer_register,
		add_i_register,
		load_digit_sprite_register,
		load_bcd_register,
		store_registers_i_register,
		load_registers_i_register,
		exit,
	};
}