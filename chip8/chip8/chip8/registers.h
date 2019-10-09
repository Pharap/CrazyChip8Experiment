#pragma once

#include "base_types.h"

namespace chip8
{
	enum class register_id : byte
	{
		reg_0,
		reg_1,
		reg_2,
		reg_3,
		reg_4,
		reg_5,
		reg_6,
		reg_7,
		reg_8,
		reg_9,
		reg_a,
		reg_b,
		reg_c,
		reg_d,
		reg_e,
		reg_f,
	};

	constexpr std::size_t to_index(register_id id)
	{
		return static_cast<std::size_t>(id);
	}

	struct register_set
	{
	private:
		using array_type = byte_array<16>;

	public:
		using size_type = typename array_type::size_type;

	private:
		array_type registers;

	public:
		byte & at(size_type index)
		{
			return this->registers.at(index);
		}

		const byte & at(size_type index) const
		{
			return this->registers.at(index);
		}

		byte & operator[](register_id id)
		{
			return this->registers[to_index(id)];
		}

		const byte & operator[](register_id id) const
		{
			return this->registers[to_index(id)];
		}

		byte & operator[](size_type index)
		{
			return this->registers[index];
		}

		const byte & operator[](size_type index) const
		{
			return this->registers[index];
		}
	};
}