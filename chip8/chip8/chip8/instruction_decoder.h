#pragma once

#include "opcodes.h"
#include "instructions.h"

namespace chip8
{
	namespace decode_helpers
	{
		pointer get_address(word instruction)
		{
			return static_cast<pointer>(instruction & 0x0FFF);
		}

		byte get_immediate(word instruction)
		{
			return static_cast<byte>(instruction & 0x00FF);
		}

		register_id get_x_register(word instruction)
		{
			return static_cast<register_id>((instruction & 0x0F00) >> 8);
		}

		register_id get_y_register(word instruction)
		{
			return static_cast<register_id>((instruction & 0x00F0) >> 4);
		}

		byte get_sprite_size(word instruction)
		{
			return static_cast<byte>((instruction & 0x000F) >> 0);
		}

		byte get_function_type(word instruction)
		{
			return static_cast<byte>((instruction & 0x000F) >> 0);
		}

		tagged_instruction decode_special_0(word instruction)
		{
			switch(instruction)
			{
			case 0x00E0:
				return { opcode_id::clear_screen };
			case 0x00EE:
				return { opcode_id::function_return };
			case 0x00FD:
				return { opcode_id::exit };
			default:
				throw std::exception();
			}
		}

		tagged_instruction decode_special_5(word instruction)
		{
			byte function_type = get_function_type(instruction);

			switch(function_type)
			{
			case 0x0:
				return { opcode_id::skip_if_equal_register_register, get_x_register(instruction), get_y_register(instruction) };
			default:
				throw std::exception();
			}
		}

		tagged_instruction decode_special_8(word instruction)
		{
			byte function_type = get_function_type(instruction);
			auto destination = get_x_register(instruction);
			auto source = get_y_register(instruction);

			switch(function_type)
			{
			case 0x0:
				return { opcode_id::load_register_register, destination, source };
			case 0x1:
				return { opcode_id::or_register_register, destination, source };
			case 0x2:
				return { opcode_id::and_register_register, destination, source };
			case 0x3:
				return { opcode_id::xor_register_register, destination, source };
			case 0x4:
				return { opcode_id::add_register_register, destination, source };
			case 0x5:
				return { opcode_id::subtract_register_register, destination, source };
			case 0x6:
				return { opcode_id::shift_right_register_register, destination, source };
			case 0x7:
				return { opcode_id::reverse_subtract_register_register, destination, source };
			case 0x8:
				return { opcode_id::shift_left_register_register, destination, source };
			default:
				throw std::exception();
			}
		}

		tagged_instruction decode_special_9(word instruction)
		{
			byte function_type = get_function_type(instruction);

			switch(function_type)
			{
			case 0x0:
				return { opcode_id::skip_if_not_equal_register_register, get_x_register(instruction), get_y_register(instruction) };
			default:
				throw std::exception();
			}
		}

		tagged_instruction decode_special_e(word instruction)
		{
			auto x = get_x_register(instruction);
			auto function_type = get_immediate(instruction);

			switch(function_type)
			{
			case 0x9E:
				return { opcode_id::skip_if_key_pressed_register, x };
			case 0xA1:
				return { opcode_id::skip_if_key_not_pressed_register, x };
			default:
				throw std::exception();
			}
		}

		tagged_instruction decode_special_f(word instruction)
		{
			auto x = get_x_register(instruction);
			auto function_type = get_immediate(instruction);

			switch(function_type)
			{
			case 0x07:
				return { opcode_id::read_delay_timer_register, x };
			case 0x0A:
				return { opcode_id::await_key_press_register, x };
			case 0x15:
				return { opcode_id::write_delay_timer_register, x };
			case 0x18:
				return { opcode_id::write_sound_timer_register, x };
			case 0x1E:
				return { opcode_id::add_i_register, x };
			case 0x29:
				return { opcode_id::load_digit_sprite_register, x };
			case 0x33:
				return { opcode_id::load_bcd_register, x };
			case 0x55:
				return { opcode_id::store_registers_i_register, x };
			case 0x65:
				return { opcode_id::load_registers_i_register, x };
			default:
				throw std::exception();
			}
		}
	}

	tagged_instruction decode_standard(word instruction)
	{
		byte most_significant_nibble = ((instruction >> 12) & 0x0F);

		using namespace decode_helpers;

		switch(most_significant_nibble)
		{
		case 0x0:
			return decode_special_0(instruction);
		case 0x1:
			return { opcode_id::jump_address, get_address(instruction) };
		case 0x2:
			return { opcode_id::call_address, get_address(instruction) };
		case 0x3:
			return { opcode_id::skip_if_equal_register_immediate, get_x_register(instruction), get_immediate(instruction) };
		case 0x4:
			return { opcode_id::skip_if_not_equal_register_immediate, get_x_register(instruction), get_immediate(instruction) };
		case 0x5:
			return decode_special_5(instruction);
		case 0x6:
			return { opcode_id::load_register_immediate, get_x_register(instruction), get_immediate(instruction) };
		case 0x7:
			return { opcode_id::add_register_immediate, get_x_register(instruction), get_immediate(instruction) };
		case 0x8:
			return decode_special_8(instruction);
		case 0x9:
			return decode_special_9(instruction);
		case 0xA:
			return { opcode_id::load_i_immediate, get_address(instruction) };
		case 0xB:
			return { opcode_id::jump_address_register_0, get_address(instruction) };
		case 0xC:
			return { opcode_id::random_register_immediate, get_x_register(instruction), get_immediate(instruction) };
		case 0xD:
			return { opcode_id::draw_x_y_size, get_x_register(instruction), get_y_register(instruction), get_sprite_size(instruction) };
		case 0xE:
			return decode_special_e(instruction);
		case 0xF:
			return decode_special_f(instruction);
		default:
			throw std::exception();
		}
	}
}