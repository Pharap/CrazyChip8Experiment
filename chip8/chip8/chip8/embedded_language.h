#pragma once

#include "instruction_encoder.h"

//
// VOODOO!!!
//
// [jungle drums intensify]
//

namespace chip8
{
	namespace lang
	{
		struct program
		{
		public:
			using encoder_type = chip8::encoder;
			using writer_type = vector_byte_writer;
			using container_type = typename writer_type::container_type;
			using size_type = typename container_type::size_type;

		private:
			writer_type writer;
			encoder_type encoder;

		public:
			program()
				: writer(), encoder(writer)
			{
			}

			encoder_type & get_encoder()
			{
				return this->encoder;
			}

			std::vector<byte> & get_writer_container()
			{
				return this->writer.get_container();
			}
		};

		constexpr struct end_program_t {} end_program;
		
		constexpr struct i_register_t {} i_register;	

		constexpr struct clear_screen_t {} clear_screen;

		constexpr struct key_t {} key;

		constexpr struct return_t {} ret;


		struct load_register_immediate_t
		{
			register_id reg;
			byte immediate;
		};


		struct load_register_register_t
		{
			register_id destination;
			register_id source;
		};


		struct await_key_t
		{
			register_id reg;
		};


		struct key_pressed_t
		{
			register_id reg;
		};


		struct key_not_pressed_t
		{
			register_id reg;
		};

		key_pressed_t key_pressed(register_id reg)
		{
			return { reg };
		}

		key_not_pressed_t operator !(key_pressed_t expression)
		{
			return { expression.reg };
		}


		template< register_id reg >
		struct register_t : std::integral_constant<register_id, reg>
		{
			load_register_immediate_t operator =(byte immediate) const
			{
				return { reg, immediate };
			}

			template< register_id reg_1 >
			load_register_register_t operator =(register_t<reg_1>) const
			{
				return { reg, reg_1 };
			}

			await_key_t operator =(key_t) const
			{
				return { reg };
			}
		};


		constexpr register_t<register_id::reg_0> reg_0;
		constexpr register_t<register_id::reg_1> reg_1;
		constexpr register_t<register_id::reg_2> reg_2;
		constexpr register_t<register_id::reg_3> reg_3;
		constexpr register_t<register_id::reg_4> reg_4;
		constexpr register_t<register_id::reg_5> reg_5;
		constexpr register_t<register_id::reg_6> reg_6;
		constexpr register_t<register_id::reg_7> reg_7;
		constexpr register_t<register_id::reg_8> reg_8;
		constexpr register_t<register_id::reg_9> reg_9;
		constexpr register_t<register_id::reg_a> reg_a;
		constexpr register_t<register_id::reg_b> reg_b;
		constexpr register_t<register_id::reg_c> reg_c;
		constexpr register_t<register_id::reg_d> reg_d;
		constexpr register_t<register_id::reg_e> reg_e;
		constexpr register_t<register_id::reg_f> reg_f;

		
		struct register_equals_immediate_t
		{
			register_id reg;
			byte immediate;
		};

		register_equals_immediate_t operator ==(register_id reg, byte immediate)
		{
			return { reg, immediate };
		}


		struct register_not_equals_immediate_t
		{
			register_id reg;
			byte immediate;
		};

		register_not_equals_immediate_t operator !=(register_id reg, byte immediate)
		{
			return { reg, immediate };
		}


		struct register_equals_register_t
		{
			register_id x;
			register_id y;
		};

		template< register_id reg_x, register_id reg_y >
		register_equals_register_t operator ==(register_t<reg_x>, register_t<reg_y>)
		{
			return { reg_x, reg_y };
		}


		struct register_not_equals_register_t
		{
			register_id x;
			register_id y;
		};

		template< register_id reg_x, register_id reg_y >
		register_not_equals_register_t operator !=(register_t<reg_x>, register_t<reg_y>)
		{
			return { reg_x, reg_y };
		}


		template< typename T >
		struct skip_if_t;


		template<>
		struct skip_if_t<register_equals_immediate_t>
		{
			register_equals_immediate_t expression;
		};

		constexpr skip_if_t<register_equals_immediate_t> skip_if(register_equals_immediate_t expression)
		{
			return { expression };
		}


		template<>
		struct skip_if_t<register_not_equals_immediate_t>
		{
			register_not_equals_immediate_t expression;
		};

		constexpr skip_if_t<register_not_equals_immediate_t> skip_if(register_not_equals_immediate_t expression)
		{
			return { expression };
		}


		template<>
		struct skip_if_t<register_equals_register_t>
		{
			register_equals_register_t expression;
		};

		constexpr skip_if_t<register_equals_register_t> skip_if(register_equals_register_t expression)
		{
			return { expression };
		}


		template<>
		struct skip_if_t<register_not_equals_register_t>
		{
			register_not_equals_register_t expression;
		};

		constexpr skip_if_t<register_not_equals_register_t> skip_if(register_not_equals_register_t expression)
		{
			return { expression };
		}


		template<>
		struct skip_if_t<key_pressed_t>
		{
			key_pressed_t expression;
		};

		constexpr skip_if_t<key_pressed_t> skip_if(key_pressed_t expression)
		{
			return { expression };
		}


		template<>
		struct skip_if_t<key_not_pressed_t>
		{
			key_not_pressed_t expression;
		};

		constexpr skip_if_t<key_not_pressed_t> skip_if(key_not_pressed_t expression)
		{
			return { expression };
		}


		struct add_register_immediate_t
		{
			register_id reg;
			byte immediate;
		};

		add_register_immediate_t operator +=(register_id reg, byte immediate)
		{
			return { reg, immediate };
		}


		struct add_register_register_t
		{
			register_id destination;
			register_id source;
		};

		add_register_register_t operator +=(register_id destination, register_id source)
		{
			return { destination, source };
		}


		struct subtract_register_register_t
		{
			register_id destination;
			register_id source;
		};

		subtract_register_register_t operator -=(register_id destination, register_id source)
		{
			return { destination, source };
		}


		struct add_i_register_register_t
		{
			register_id reg;
		};

		add_i_register_register_t operator +=(i_register_t, register_id reg)
		{
			return { reg };
		}


		struct label
		{
		public:
			using index_type = typename program::size_type;

		private:
			bool was_set = false;
			index_type index = 0;

			void set()
			{
				this->was_set = true;
			}

		public:
			constexpr label() = default;

			constexpr bool is_set() const
			{
				return this->was_set;
			}

			constexpr index_type get_address() const
			{
				return this->index;
			}

			void set(index_type index)
			{
				if(this->is_set())
					throw std::logic_error("attempt to reassign a label");

				this->index = index;
				this->set();
			}
		};

		struct jump_t
		{
		private:
			const label & target;

		public:
			constexpr jump_t(const label & target) :
				target(target)
			{
			}

			constexpr const label & get_target() const
			{
				return this->target;
			}
		};

		constexpr jump_t jump(const label & target)
		{
			return jump_t(target);
		}

		struct sprite_load_t
		{
			register_id reg;
		};

		sprite_load_t load_sprite(register_id reg)
		{
			return { reg };
		}

		struct sprite_draw_t
		{
			register_id x;
			register_id y;
			byte size;
		};

		sprite_draw_t draw_sprite(register_id x, register_id y, byte size)
		{
			return { x, y, size };
		}



		std::vector<byte> operator ,(program & program, end_program_t)
		{
			program.get_encoder().encode_exit();
			return std::move(program.get_writer_container());
		}

		program & operator ,(program & program, label & label_declaration)
		{
			label_declaration.set(program.get_writer_container().size());
			return program;
		}

		program & operator ,(program & program, jump_t jump)
		{
			if(!jump.get_target().is_set())
				throw std::logic_error("attempt to jump to unset label");

			program.get_encoder().encode_jump(jump.get_target().get_address());
			return program;
		}

		program & operator ,(program & program, add_register_immediate_t statement)
		{
			program.get_encoder().encode_add(statement.reg, statement.immediate);
			return program;
		}

		program & operator ,(program & program, add_register_register_t statement)
		{
			program.get_encoder().encode_add(statement.destination, statement.source);
			return program;
		}

		program & operator ,(program & program, subtract_register_register_t statement)
		{
			program.get_encoder().encode_subtract(statement.destination, statement.source);
			return program;
		}

		program & operator ,(program & program, load_register_immediate_t statement)
		{
			program.get_encoder().encode_load(statement.reg, statement.immediate);
			return program;
		}

		program & operator ,(program & program, load_register_register_t statement)
		{
			program.get_encoder().encode_load(statement.destination, statement.source);
			return program;
		}

		program & operator ,(program & program, skip_if_t<register_equals_immediate_t> skip_statement)
		{
			const auto expression = skip_statement.expression;
			program.get_encoder().encode_skip_if_equal(expression.reg, expression.immediate);
			return program;
		}

		program & operator ,(program & program, skip_if_t<register_not_equals_immediate_t> skip_statement)
		{
			const auto expression = skip_statement.expression;
			program.get_encoder().encode_skip_if_not_equal(expression.reg, expression.immediate);
			return program;
		}

		program & operator ,(program & program, skip_if_t<register_equals_register_t> skip_statement)
		{
			const auto expression = skip_statement.expression;
			program.get_encoder().encode_skip_if_equal(expression.x, expression.y);
			return program;
		}

		program & operator ,(program & program, skip_if_t<register_not_equals_register_t> skip_statement)
		{
			const auto expression = skip_statement.expression;
			program.get_encoder().encode_skip_if_not_equal(expression.x, expression.y);
			return program;
		}

		program & operator ,(program & program, skip_if_t<key_pressed_t> skip_statement)
		{
			const auto expression = skip_statement.expression;
			program.get_encoder().encode_skip_if_key_pressed(expression.reg);
			return program;
		}

		program & operator ,(program & program, skip_if_t<key_not_pressed_t> skip_statement)
		{
			const auto expression = skip_statement.expression;
			program.get_encoder().encode_skip_if_key_not_pressed(expression.reg);
			return program;
		}

		program & operator ,(program & program, sprite_load_t statement)
		{
			program.get_encoder().encode_load_digit_sprite(statement.reg);
			return program;
		}

		program & operator ,(program & program, sprite_draw_t statement)
		{
			program.get_encoder().encode_draw(statement.x, statement.y, statement.size);
			return program;
		}
	}
}