#pragma once

#include <type_traits>
#include <exception>

#include "base_types.h"
#include "opcodes.h"
#include "registers.h"

namespace chip8
{
	struct instruction_base
	{
		opcode_id opcode;

		constexpr instruction_base(opcode_id opcode)
			: opcode(opcode)
		{
		}
	};

	struct instruction_no_arguments : instruction_base
	{
		constexpr instruction_no_arguments(opcode_id opcode)
			: instruction_base(opcode)
		{
		}
	};

	struct instruction_address : instruction_base
	{
		pointer address;

		constexpr instruction_address(opcode_id opcode, pointer address)
			: instruction_base(opcode), address(address)
		{
		}
	};

	struct instruction_register : instruction_base
	{
		register_id reg;

		constexpr instruction_register(opcode_id opcode, register_id reg)
			: instruction_base(opcode), reg(reg)
		{
		}
	};

	struct instruction_register_immediate : instruction_base
	{
		register_id destination;
		byte immediate;

		constexpr instruction_register_immediate(opcode_id opcode, register_id destination, byte immediate)
			: instruction_base(opcode), destination(destination), immediate(immediate)
		{
		}
	};

	struct instruction_register_register : instruction_base
	{
		register_id destination;
		register_id source;

		constexpr instruction_register_register(opcode_id opcode, register_id destination, register_id source)
			: instruction_base(opcode), destination(destination), source(source)
		{
		}
	};

	struct instruction_draw : instruction_base
	{
		register_id x;
		register_id y;
		byte size;

		constexpr instruction_draw(opcode_id opcode, register_id x, register_id y, byte size)
			: instruction_base(opcode), x(x), y(y), size(size)
		{
		}
	};

	class tagged_instruction
	{
	private:
		enum class instruction_tag
		{
			unknown,
			no_arguments,
			address,
			reg,
			register_immediate,
			register_register,
			draw,
		};

		union instruction_data
		{
			instruction_no_arguments no_arguments;
			instruction_address address;
			instruction_register reg;
			instruction_register_immediate register_immediate;
			instruction_register_register register_register;
			instruction_draw draw;

			constexpr instruction_data(instruction_no_arguments instruction)
				: no_arguments(instruction)
			{
			}

			constexpr instruction_data(instruction_address instruction)
				: address(instruction)
			{
			}

			constexpr instruction_data(instruction_register instruction)
				: reg(instruction)
			{
			}

			constexpr instruction_data(instruction_register_immediate instruction)
				: register_immediate(instruction)
			{
			}

			constexpr instruction_data(instruction_register_register instruction)
				: register_register(instruction)
			{
			}

			constexpr instruction_data(instruction_draw instruction)
				: draw(instruction)
			{
			}
		};

		instruction_tag tag;
		instruction_data data;

	public:
		constexpr tagged_instruction(instruction_no_arguments instruction)
			: tag(instruction_tag::no_arguments), data(instruction)
		{
		}

		constexpr tagged_instruction(opcode_id opcode)
			: tagged_instruction(instruction_no_arguments(opcode))
		{
		}

		constexpr tagged_instruction(instruction_address instruction)
			: tag(instruction_tag::address), data(instruction)
		{
		}

		constexpr tagged_instruction(opcode_id opcode, pointer address)
			: tagged_instruction(instruction_address(opcode, address))
		{
		}

		constexpr tagged_instruction(instruction_register instruction)
			: tag(instruction_tag::reg), data(instruction)
		{
		}

		constexpr tagged_instruction(opcode_id opcode, register_id reg)
			: tagged_instruction(instruction_register(opcode, reg))
		{
		}

		constexpr tagged_instruction(instruction_register_immediate instruction)
			: tag(instruction_tag::register_immediate), data(instruction)
		{
		}

		constexpr tagged_instruction(opcode_id opcode, register_id reg, byte immediate)
			: tagged_instruction(instruction_register_immediate(opcode, reg, immediate))
		{
		}

		constexpr tagged_instruction(instruction_register_register instruction)
			: tag(instruction_tag::register_register), data(instruction)
		{
		}

		constexpr tagged_instruction(opcode_id opcode, register_id x, register_id y)
			: tagged_instruction(instruction_register_register(opcode, x, y))
		{
		}

		constexpr tagged_instruction(instruction_draw instruction)
			: tag(instruction_tag::draw), data(instruction)
		{
		}

		constexpr tagged_instruction(opcode_id opcode, register_id x, register_id y, byte size)
			: tagged_instruction(instruction_draw(opcode, x, y, size))
		{
		}

		opcode_id get_opcode() const
		{
			switch(this->tag)
			{
			case instruction_tag::no_arguments:
				return this->data.no_arguments.opcode;
			case instruction_tag::address:
				return this->data.address.opcode;
			case instruction_tag::reg:
				return this->data.reg.opcode;
			case instruction_tag::register_register:
				return this->data.register_register.opcode;
			case instruction_tag::register_immediate:
				return this->data.register_immediate.opcode;
			case instruction_tag::draw:
				return this->data.draw.opcode;
			case instruction_tag::unknown:
				throw std::exception();
			default:
				throw std::exception();
			}
		}

		pointer get_address() const
		{
			if(this->tag == instruction_tag::address)
				return this->data.address.address;

			throw std::exception();
		}

		register_id get_register() const
		{
			if(this->tag == instruction_tag::reg)
				return this->data.reg.reg;

			throw std::exception();
		}

		register_id get_source_register() const
		{
			if(this->tag == instruction_tag::register_register)
				return this->data.register_register.source;

			throw std::exception();
		}

		register_id get_destination_register() const
		{
			if(this->tag == instruction_tag::register_register)
				return this->data.register_register.destination;

			if(this->tag == instruction_tag::register_immediate)
				return this->data.register_immediate.destination;

			throw std::exception();
		}

		byte get_immediate() const
		{
			if(this->tag == instruction_tag::register_immediate)
				return this->data.register_immediate.immediate;

			throw std::exception();
		}

		register_id get_x_register() const
		{
			if(this->tag == instruction_tag::draw)
				return this->data.draw.x;

			throw std::exception();
		}

		register_id get_y_register() const
		{
			if(this->tag == instruction_tag::draw)
				return this->data.draw.y;

			throw std::exception();
		}

		byte get_sprite_size() const
		{
			if(this->tag == instruction_tag::draw)
				return this->data.draw.size;

			throw std::exception();
		}

		instruction_no_arguments get_instruction_no_arguments() const
		{
			if(this->tag == instruction_tag::no_arguments)
				return this->data.no_arguments;

			throw std::exception();
		}

		instruction_address get_instruction_address() const
		{
			if(this->tag == instruction_tag::address)
				return this->data.address;

			throw std::exception();
		}

		instruction_register get_instruction_register() const
		{
			if(this->tag == instruction_tag::reg)
				return this->data.reg;

			throw std::exception();
		}

		instruction_register_immediate get_instruction_register_immediate() const
		{
			if(this->tag == instruction_tag::register_immediate)
				return this->data.register_immediate;

			throw std::exception();
		}

		instruction_register_register get_instruction_register_register() const
		{
			if(this->tag == instruction_tag::register_register)
				return this->data.register_register;

			throw std::exception();
		}
		
		instruction_draw get_instruction_draw() const
		{
			if(this->tag == instruction_tag::draw)
				return this->data.draw;

			throw std::exception();
		}



		operator instruction_no_arguments() const
		{
			if(this->tag == instruction_tag::no_arguments)
				return this->data.no_arguments;

			throw std::exception();
		}

		operator instruction_address() const
		{
			if(this->tag == instruction_tag::address)
				return this->data.address;

			throw std::exception();
		}

		operator instruction_register() const
		{
			if(this->tag == instruction_tag::reg)
				return this->data.reg;

			throw std::exception();
		}

		operator instruction_register_immediate() const
		{
			if(this->tag == instruction_tag::register_immediate)
				return this->data.register_immediate;

			throw std::exception();
		}

		operator instruction_register_register() const
		{
			if(this->tag == instruction_tag::register_register)
				return this->data.register_register;

			throw std::exception();
		}

		operator instruction_draw() const
		{
			if(this->tag == instruction_tag::draw)
				return this->data.draw;

			throw std::exception();
		}
	};
}