#pragma once

#include <iostream>
#include <vector>
#include <deque>
#include <memory>

#include "base_types.h"
#include "opcodes.h"
#include "instructions.h"

namespace chip8
{
	class byte_writer
	{
	public:
		virtual ~byte_writer() = default;

		virtual bool has_bytes_available(std::size_t amount) const = 0;

		virtual void write_byte(byte value) = 0;
		
		void write_word_big_endian(word value)
		{
			const byte high = ((value >> 8) & 0xFF);
			const byte low = ((value >> 0) & 0xFF);

			this->write_byte(high);
			this->write_byte(low);
		}

		void write_word_little_endian(word value)
		{
			const byte low = ((value >> 0) & 0xFF);
			const byte high = ((value >> 8) & 0xFF);

			this->write_byte(low);
			this->write_byte(high);
		}
	};

	template< typename Container, typename std::enable_if<std::is_same<typename Container::value_type, byte>::value, int>::type = 0 >
	class container_byte_writer : public byte_writer
	{
	public:
		using container_type = Container;
		using value_type = typename container_type::value_type;
		using size_type = typename container_type::size_type;

	private:
		container_type container;

	public:
		container_type & get_container()
		{
			return this->container;
		}

		const container_type & get_container() const
		{
			return this->container;
		}

		bool has_bytes_available(std::size_t amount) const override
		{
			const auto difference = (this->container.max_size() - this->container.size());
			return (difference >= amount);
		}

		void write_byte(byte value) override
		{
			this->container.push_back(value);
		}
	};

	using vector_byte_writer = container_byte_writer<std::vector<byte>>;
	using deque_byte_writer = container_byte_writer<std::deque<byte>>;

	class encoder
	{
	private:
		byte_writer & writer;
		pointer program_offset = 0x200;

	public:
		encoder(byte_writer & writer) :
			writer(writer)
		{
		}

		encoder(byte_writer & writer, pointer program_offset) :
			writer(writer), program_offset(program_offset)
		{
		}

		void encode_clear_screen()
		{
			this->write_word(0x00E0);
		}

		void encode_return()
		{
			this->write_word(0x00EE);
		}

		void encode_exit()
		{
			this->write_word(0x00FD);
		}

		void encode_jump(pointer address)
		{
			this->write_address(0x1, address);
		}

		void encode_jump_register_0(pointer address)
		{
			this->write_address(0xB, address);
		}

		void encode_random(register_id x, byte immediate)
		{
			this->write_special(0xC, x, immediate);
		}

		void encode_draw(register_id x, register_id y, byte size)
		{
			this->write_special(0xD, x, y, size);
		}

		void encode_call(pointer address)
		{
			this->write_address(0x2, address);
		}

		void encode_load_i(pointer address)
		{
			this->write_address(0xA, address);
		}

		void encode_skip_if_equal(register_id x, byte immediate)
		{
			this->write_special(0x3, x, immediate);
		}

		void encode_skip_if_equal(register_id x, register_id y)
		{
			this->write_special(0x5, x, y, 0x0);
		}

		void encode_skip_if_not_equal(register_id x, byte immediate)
		{
			this->write_special(0x4, x, immediate);
		}

		void encode_skip_if_not_equal(register_id x, register_id y)
		{
			this->write_special(0x9, x, y, 0x0);
		}

		void encode_load(register_id destination, register_id source)
		{
			this->write_special(0x8, destination, source, 0x0);
		}

		void encode_load(register_id destination, byte immediate)
		{
			this->write_special(0x6, destination, immediate);
		}

		void encode_add(register_id destination, byte immediate)
		{
			this->write_special(0x7, destination, immediate);
		}

		void encode_or(register_id destination, register_id source)
		{
			this->write_special(0x8, destination, source, 0x1);
		}

		void encode_and(register_id destination, register_id source)
		{
			this->write_special(0x8, destination, source, 0x2);
		}

		void encode_xor(register_id destination, register_id source)
		{
			this->write_special(0x8, destination, source, 0x3);
		}

		void encode_add(register_id destination, register_id source)
		{
			this->write_special(0x8, destination, source, 0x4);
		}

		void encode_subtract(register_id destination, register_id source)
		{
			this->write_special(0x8, destination, source, 0x5);
		}

		void encode_shift_right(register_id destination, register_id source)
		{
			this->write_special(0x8, destination, source, 0x6);
		}

		void encode_reverse_subtract(register_id destination, register_id source)
		{
			this->write_special(0x8, destination, source, 0x7);
		}

		void encode_shift_left(register_id destination, register_id source)
		{
			this->write_special(0x8, destination, source, 0x8);
		}

		void encode_skip_if_key_pressed(register_id x)
		{
			this->write_special(0xE, x, 0x9E);
		}

		void encode_skip_if_key_not_pressed(register_id x)
		{
			this->write_special(0xE, x, 0xA1);
		}

		void encode_read_delay_timer(register_id x)
		{
			this->write_special(0xF, x, 0x07);
		}

		void encode_await_key_press(register_id x)
		{
			this->write_special(0xF, x, 0x0A);
		}

		void encode_write_sound_timer(register_id x)
		{
			this->write_special(0xF, x, 0x18);
		}

		void encode_add_i_register(register_id x)
		{
			this->write_special(0xF, x, 0x1E);
		}

		void encode_load_digit_sprite(register_id x)
		{
			this->write_special(0xF, x, 0x29);
		}

		void encode_load_bcd(register_id x)
		{
			this->write_special(0xF, x, 0x33);
		}

		void encode_store_registers_i(register_id x)
		{
			this->write_special(0xF, x, 0x55);
		}

		void encode_load_registers_i(register_id x)
		{
			this->write_special(0xF, x, 0x65);
		}

	private:
		void write_byte(byte value)
		{
			this->writer.write_byte(value);
		}

		void write_word(word value)
		{
			this->writer.write_word_big_endian(value);
		}

		void write_address(byte category, pointer address)
		{
			const word instruction = (((category & 0x0F) << 12) | (((address + program_offset) & 0x0FFF) << 0));
			this->write_word(instruction);
		}

		word create_special(byte category_type, register_id register_x, register_id register_y, byte function_type)
		{
			const byte category = (category_type & 0x0F);
			const byte x = to_index(register_x);
			const byte y = to_index(register_y);
			const byte function = (function_type & 0x0F);

			return ((category << 12) | (x << 8) | (y << 4) | (function << 0));
		}

		void write_special(byte category, register_id x, register_id y, byte function)
		{
			this->write_word(this->create_special(category, x, y, function));
		}

		word create_special(byte category_type, register_id register_x, byte function_type)
		{
			const byte category = (category_type & 0x0F);
			const byte x = to_index(register_x);
			const byte function = (function_type & 0xFF);

			return ((category << 12) | (x << 8) | (function << 0));
		}

		void write_special(byte category, register_id x, byte function)
		{
			this->write_word(this->create_special(category, x, function));
		}
	};
}