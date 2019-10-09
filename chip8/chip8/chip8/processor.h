#pragma once

#include <memory>
#include <iterator>

#include "base_types.h"
#include "registers.h"
#include "opcodes.h"
#include "instructions.h"
#include "instruction_decoder.h"
#include "stack.h"
#include "display_buffer.h"
#include "display.h"
#include "keyboard.h"
#include "sprite_rom.h"

namespace chip8
{
	enum class processor_state
	{
		halted,
		running,
		idle,
		awaiting_key,
	};

	class processor
	{
	public:
		static constexpr std::size_t memory_capacity = 0x0FFF;

		static constexpr std::size_t rom_start_offset = 0x0000;
		static constexpr std::size_t rom_end_offset = 0x0200;
		static constexpr std::size_t rom_memory_capacity = (rom_end_offset - rom_start_offset);

		static constexpr std::size_t program_start_offset = 0x200;
		static constexpr std::size_t program_end_offset = memory_capacity;
		static constexpr std::size_t program_memory_capacity = (program_end_offset - program_start_offset);

		static constexpr std::size_t font_character_size = 5;

	private:
		using display_pointer = std::shared_ptr<display>;
		using keyboard_pointer = std::shared_ptr<keyboard>;

	private:
		processor_state state = processor_state::halted;

		pointer program_counter = program_start_offset;
		pointer i_register = 0;

		register_set registers;
		stack<pointer, 16> call_stack;
		keyboard_pointer keyboard;
		display_pointer display;
		display_buffer<64, 32> buffer;
		byte_array<4096> memory;

	public:
		processor(display_pointer && display, keyboard_pointer && keyboard) :
			display(std::forward<display_pointer>(display)),
			keyboard(std::forward<keyboard_pointer>(keyboard))
		{
		}

		processor_state get_state()
		{
			return this->state;
		}

		void reset()
		{
			this->program_counter = program_start_offset;
		}

		void start()
		{
			if(this->state == processor_state::running)
				throw std::logic_error("cannot start an already-running processor");

			this->state = processor_state::running;
		}

		void stop()
		{
			this->state = processor_state::halted;
		}

		void pause()
		{
			if(this->state != processor_state::running)
				throw std::logic_error("cannot pause a non-running processor");

			this->state = processor_state::idle;
		}

		void resume()
		{
			if(this->state != processor_state::idle)
				throw std::logic_error("cannot resume a non-idle processor");

			this->state = processor_state::running;
		}

		void update_display()
		{
			this->display->update(this->buffer);
			this->display->render();
		}

		void run(std::size_t cycle_count)
		{
			if(this->state == processor_state::halted)
				return;

			this->state = processor_state::running;
			
			for(std::size_t cycle = 0; cycle < cycle_count; ++cycle)
			{
				if(this->state == processor_state::halted)
					return;

				step();
			}

			this->update_display();

			if(this->state != processor_state::halted && this->state != processor_state::awaiting_key)
				this->state = processor_state::idle;
		}

		void step()
		{
			switch(this->state)
			{
			case processor_state::running:
				this->execute();
				break;
			case processor_state::awaiting_key:
			{
				//this->keyboard->
			}
				break;
			}
		}

		template< std::size_t sprite_count, std::size_t sprite_size >
		void load_sprite_rom(const byte(&sprites)[sprite_count][sprite_size])
		{
			auto destination = std::begin(this->memory);

			for(std::size_t sprite_index = 0; sprite_index < sprite_count; ++sprite_index)
				destination = std::copy(std::begin(sprites[sprite_index]), std::end(sprites[sprite_index]), destination);
		}

		void load_default_sprite_rom()
		{
			this->load_sprite_rom(chip8::default_sprite_rom);
		}

		template< std::size_t size >
		void load_program(byte(&array)[size])
		{
			static_assert(size < program_memory_capacity, "rom too large");

			auto memory_begin = std::begin(this->memory);
			std::advance(memory_begin, 0x200);

			static_cast<void>(std::copy(std::begin(array), std::end(array), memory_begin));
		}

		template< typename InputIterator >
		void load_program(InputIterator begin, InputIterator end)
		{
			auto memory_begin = std::begin(this->memory);
			std::advance(memory_begin, 0x200);

			auto memory_end = std::end(this->memory);

			const auto memory_size = static_cast<std::size_t>(std::distance(memory_begin, memory_end));
			const auto input_size = static_cast<std::size_t>(std::distance(begin, end));

			if(input_size > memory_size)
				throw std::length_error("provided range of elements is larger than program space");

			static_cast<void>(std::copy(begin, end, memory_begin));
		}

	private:
		bool draw(const byte * data, byte x, byte y, byte size)
		{
			bool overwrite = false;

			for(size_t index = 0; index < size; ++index)
			{
				std::size_t buffer_y = (y + index);

				byte value = data[index];
				for(size_t shift = 0; shift < 8; ++shift)
				{
					std::size_t buffer_x = (x + shift);

					size_t bit_index = (7 - shift);

					bool bit = (((value >> bit_index) & 0x01) != 0);										
					bool old_value = buffer.wrap_at(buffer_x, buffer_y);
					bool new_value = (bit ^ old_value);

					if(old_value && !new_value)
						overwrite = true;

					buffer.wrap_at(buffer_x, buffer_y) = new_value;
				}
			}

			return overwrite;
		}

		void skip_instruction()
		{
			this->program_counter += sizeof(word);
		}

		void execute()
		{
			if(this->program_counter >= program_end_offset)
				return;

			byte high = this->memory[this->program_counter];
			++this->program_counter;

			byte low = this->memory[this->program_counter];
			++this->program_counter;

			word instruction_value = ((high << 8) | (low << 0));

			auto instruction = decode_standard(instruction_value);

			this->execute(instruction);
		}

		void execute(tagged_instruction instruction)
		{
			switch(instruction.get_opcode())
			{
			case opcode_id::clear_screen:
				this->execute_clear_screen();
				break;

			case opcode_id::function_return:
				this->execute_function_return();
				break;

			case opcode_id::jump_address:
				this->execute_jump_address(instruction);
				break;

			case opcode_id::call_address:
				this->execute_call_address(instruction);
				break;

			case opcode_id::skip_if_equal_register_immediate:
				this->execute_skip_if_equal_register_immediate(instruction);
				break;

			case opcode_id::skip_if_not_equal_register_immediate:
				this->execute_skip_if_not_equal_register_immediate(instruction);
				break;

			case opcode_id::skip_if_equal_register_register:
				this->execute_skip_if_equal_register_register(instruction);
				break;

			case opcode_id::load_register_immediate:
				this->execute_load_register_immediate(instruction);
				break;

			case opcode_id::add_register_immediate:
				this->execute_add_register_immediate(instruction);
				break;

			case opcode_id::load_register_register:
				this->execute_load_register_register(instruction);
				break;

			case opcode_id::or_register_register:
				this->execute_or_register_register(instruction);
				break;

			case opcode_id::and_register_register:
				this->execute_and_register_register(instruction);
				break;

			case opcode_id::xor_register_register:
				this->execute_xor_register_register(instruction);
				break;

			case opcode_id::add_register_register:
				this->execute_add_register_register(instruction);
				break;

			case opcode_id::subtract_register_register:
				this->execute_subtract_register_register(instruction);
				break;

			case opcode_id::shift_right_register_register:
				this->execute_shift_right_register_register(instruction);
				break;

			case opcode_id::reverse_subtract_register_register:
				this->execute_reverse_subtract_register_register(instruction);
				break;

			case opcode_id::shift_left_register_register:
				this->execute_shift_left_register_register(instruction);
				break;

			case opcode_id::skip_if_not_equal_register_register:
				this->execute_skip_if_not_equal_register_register(instruction);
				break;

			case opcode_id::load_i_immediate:
				this->execute_load_i_immediate(instruction);
				break;

			case opcode_id::jump_address_register_0:
				this->execute_jump_address_register_0(instruction);
				break;

			case opcode_id::random_register_immediate:
				this->execute_random_register_immediate(instruction);
				break;

			case opcode_id::draw_x_y_size:
				this->execute_draw_x_y_size(instruction);
				break;

			case opcode_id::skip_if_key_pressed_register:
				this->execute_skip_if_key_pressed_register(instruction);
				break;

			case opcode_id::skip_if_key_not_pressed_register:
				this->execute_skip_if_key_not_pressed_register(instruction);
				break;

			case opcode_id::read_delay_timer_register:
				this->execute_read_delay_timer_register(instruction);
				break;

			case opcode_id::await_key_press_register:
				this->execute_await_key_press_register(instruction);
				break;

			case opcode_id::write_delay_timer_register:
				this->execute_write_delay_timer_register(instruction);
				break;

			case opcode_id::write_sound_timer_register:
				this->execute_write_sound_timer_register(instruction);
				break;

			case opcode_id::add_i_register:
				this->execute_add_i_register(instruction);
				break;

			case opcode_id::load_digit_sprite_register:
				this->execute_load_digit_sprite_register(instruction);
				break;

			case opcode_id::load_bcd_register:
				this->execute_load_bcd_register(instruction);
				break;

			case opcode_id::store_registers_i_register:
				this->execute_store_registers_i_register(instruction);
				break;

			case opcode_id::load_registers_i_register:
				this->execute_load_registers_i_register(instruction);
				break;

			case opcode_id::exit:
				this->execute_exit();
				break;
			}
		}
	
		void execute_clear_screen()
		{
			this->buffer.clear();
			this->display->update(this->buffer);
		}

		void execute_function_return()
		{
			const auto return_address = this->call_stack.top();
			this->call_stack.pop();
			this->program_counter = return_address;
		}

		void execute_jump_address(instruction_address instruction)
		{
			this->program_counter = instruction.address;
		}

		void execute_call_address(instruction_address instruction)
		{
			this->call_stack.push(this->program_counter);
			this->program_counter = instruction.address;
		}

		void execute_skip_if_equal_register_immediate(instruction_register_immediate instruction)
		{
			const auto register_value = this->registers[instruction.destination];

			if(register_value == instruction.immediate)
				this->skip_instruction();
		}

		void execute_skip_if_not_equal_register_immediate(instruction_register_immediate instruction)
		{
			const auto register_value = this->registers[instruction.destination];

			if(register_value != instruction.immediate)
				this->skip_instruction();
		}

		void execute_skip_if_equal_register_register(instruction_register_register instruction)
		{
			const auto left_value = this->registers[instruction.destination];
			const auto right_value = this->registers[instruction.source];

			if(left_value == right_value)
				this->skip_instruction();
		}

		void execute_load_register_immediate(instruction_register_immediate instruction)
		{
			this->registers[instruction.destination] = instruction.immediate;
		}

		void execute_add_register_immediate(instruction_register_immediate instruction)
		{
			this->registers[instruction.destination] += instruction.immediate;
		}

		void execute_load_register_register(instruction_register_register instruction)
		{
			this->registers[instruction.destination] = this->registers[instruction.source];
		}

		void execute_or_register_register(instruction_register_register instruction)
		{
			this->registers[instruction.destination] |= this->registers[instruction.source];
		}

		void execute_and_register_register(instruction_register_register instruction)
		{
			this->registers[instruction.destination] &= this->registers[instruction.source];
		}

		void execute_xor_register_register(instruction_register_register instruction)
		{
			this->registers[instruction.destination] ^= this->registers[instruction.source];
		}

		void execute_add_register_register(instruction_register_register instruction)
		{
			this->registers[instruction.destination] += this->registers[instruction.source];
		}

		void execute_subtract_register_register(instruction_register_register instruction)
		{
			this->registers[instruction.destination] -= this->registers[instruction.source];
		}

		void execute_shift_right_register_register(instruction_register_register instruction)
		{
			this->registers[instruction.destination] >>= 1;
		}

		void execute_reverse_subtract_register_register(instruction_register_register instruction)
		{
			this->registers[instruction.source] -= this->registers[instruction.destination];
		}

		void execute_shift_left_register_register(instruction_register_register instruction)
		{
			this->registers[instruction.destination] <<= 1;
		}

		void execute_skip_if_not_equal_register_register(instruction_register_register instruction)
		{
			const auto left_value = this->registers[instruction.destination];
			const auto right_value = this->registers[instruction.source];

			if(left_value != right_value)
				this->skip_instruction();
		}

		void execute_load_i_immediate(instruction_address instruction)
		{
			this->i_register = instruction.address;
		}

		void execute_jump_address_register_0(instruction_address instruction)
		{
			this->program_counter = (instruction.address + this->registers[0]);
		}

		void execute_random_register_immediate(instruction_register_immediate instruction)
		{
		}

		void execute_draw_x_y_size(instruction_draw instruction)
		{
			const byte * sprite = &this->memory[this->i_register];
			const byte x = this->registers[instruction.x];
			const byte y = this->registers[instruction.y];
			const byte size = instruction.size;

			this->draw(sprite, x, y, size);
		}

		void execute_skip_if_key_pressed_register(instruction_register instruction)
		{
			const auto value = this->registers[instruction.reg];

			this->keyboard->update();
			if(this->keyboard->is_pressed(static_cast<key_id>(value)))
				this->program_counter += 2;
		}

		void execute_skip_if_key_not_pressed_register(instruction_register instruction)
		{
			const auto value = this->registers[instruction.reg];

			this->keyboard->update();
			if(!this->keyboard->is_pressed(static_cast<key_id>(value)))
				this->program_counter += 2;
		}

		void execute_read_delay_timer_register(instruction_register instruction)
		{
		}

		void execute_await_key_press_register(instruction_register instruction)
		{
		}

		void execute_write_delay_timer_register(instruction_register instruction)
		{
		}

		void execute_write_sound_timer_register(instruction_register instruction)
		{
		}

		void execute_add_i_register(instruction_register instruction)
		{
			this->i_register += this->registers[instruction.reg];
		}

		void execute_load_digit_sprite_register(instruction_register instruction)
		{
			const auto sprite_index = this->registers[instruction.reg];
			this->i_register = (sprite_index * font_character_size);
		}

		void execute_load_bcd_register(instruction_register instruction)
		{
			const byte value = this->registers[instruction.reg];

			const byte hundreds = (value / 100);
			const byte tens = ((value % 100) / 10);
			const byte units = ((value % 10) / 1);

			this->memory[this->i_register + 0] = hundreds;
			this->memory[this->i_register + 1] = tens;
			this->memory[this->i_register + 2] = units;
		}

		void execute_store_registers_i_register(instruction_register instruction)
		{
			const auto limit = to_index(instruction.reg);
			for(std::size_t index = 0; index < limit; ++index)
				this->memory[this->i_register + index] = this->registers[index];
		}

		void execute_load_registers_i_register(instruction_register instruction)
		{
			const auto limit = to_index(instruction.reg);
			for(std::size_t index = 0; index < limit; ++index)
				this->registers[index] = this->memory[this->i_register + index];
		}

		void execute_exit()
		{
			this->state = processor_state::halted;
		}
	};
}