#pragma once

#include <cstddef>
#include <cstdint>
#include <bitset>

namespace chip8
{
	template< std::size_t width_value, std::size_t height_value >
	class display_buffer
	{
	public:
		using size_type = std::size_t;

	private:
		static constexpr size_type width = width_value;
		static constexpr size_type height = height_value;
		static constexpr size_type pixel_count = (width * height);

	private:
		using buffer_type = std::bitset<pixel_count>;

	public:
		using reference = typename buffer_type::reference;

	private:
		buffer_type buffer;

		static constexpr size_type flatten(size_type x, size_type y)
		{
			return ((y * width) + x);
		}

	public:
		constexpr size_type get_width() const
		{
			return width;
		}

		constexpr size_type get_height() const
		{
			return height;
		}

		void clear()
		{
			this->buffer.reset();
		}

		bool test(size_type x, size_type y) const
		{
			const size_type index = flatten(x, y);
			return this->buffer.test(index);
		}

		bool at(size_type x, size_type y) const
		{
			const size_type index = flatten(x, y);
			return this->buffer[index];
		}

		reference at(size_type x, size_type y)
		{
			const size_type index = flatten(x, y);
			return this->buffer[index];
		}

		bool wrap_at(size_type x, size_type y) const
		{
			return this->at(x % this->get_width(), y % this->get_height());
		}

		reference wrap_at(size_type x, size_type y)
		{
			return this->at(x % this->get_width(), y % this->get_height());
		}
	};
}