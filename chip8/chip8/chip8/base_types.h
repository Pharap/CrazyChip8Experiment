#pragma once

#include <cstdint>
#include <array>

namespace chip8
{
	using byte = std::uint8_t;
	using word = std::uint16_t;
	using pointer = std::uint16_t;

	template<std::size_t size>
	using byte_array = std::array<byte, size>;
}