#pragma once

#include <cstddef>
#include <array>
#include <exception>

namespace chip8
{
	class stack_overflow_exception : public std::logic_error
	{
	public:
		stack_overflow_exception(const char * message) :
			std::logic_error(message)
		{
		}
	};

	class stack_underflow_exception : public std::logic_error
	{
	public:
		stack_underflow_exception(const char * message) :
			std::logic_error(message)
		{
		}
	};

	template< typename T, std::size_t count >
	class stack
	{
	private:
		using array_type = std::array<T, count>;

	public:
		using value_type = typename array_type::value_type;
		using size_type = typename array_type::size_type;
		using reference = typename array_type::reference;
		using const_reference = typename array_type::const_reference;
		using pointer = typename array_type::pointer;
		using const_pointer = typename array_type::const_pointer;
		using iterator = typename array_type::iterator;
		using const_iterator = typename array_type::const_iterator;
		using reverse_iterator = typename array_type::reverse_iterator;
		using const_reverse_iterator = typename array_type::const_reverse_iterator;

	public:
		static constexpr size_type capacity = count;

	private:
		array_type items;
		size_type next = 0;

	public:

		constexpr size_type size() const
		{
			return this->next;
		}

		constexpr size_type max_size() const
		{
			return capacity;
		}

		constexpr bool empty() const
		{
			return (this->next == 0);
		}

		reference top()
		{
			return this->items[this->next - 1];
		}

		const_reference top() const
		{
			return this->items[this->next - 1];
		}

		void push(const value_type & value)
		{
			if(this->size() == this->max_size())
				throw stack_overflow_exception("attempt to push to full stack");

			this->items[this->next] = value;
			++this->next;
		}

		void push(value_type && value)
		{
			if(this->size() == this->max_size())
				throw stack_overflow_exception();

			this->items[this->next] = std::move(value);
			++this->next;
		}

		template< typename ... Args >
		void emplace(Args && ... args)
		{
			if(this->size() == this->max_size())
				throw stack_overflow_exception();

			::new (static_cast<void *>(&this->items[this->next])) T(std::forward<Args>(args)...);
			++this->next;
		}

		void pop()
		{
			if(this->empty())
				throw stack_underflow_exception("attempt to pop empty stack");

			--this->next;
			this->items[this->next].~value_type();
		}

		void swap(stack<T, count> & other)
			noexcept(noexcept(std::declval<array_type>().swap(std::declval<array_type>())) && noexcept(std::swap(std::declval<size_type &>(), std::declval<size_type>())))
		{
			this->items.swap(other.items);
			std::swap(this->next, other.next);
		}

		reference at(size_type index)
		{
			return this->items.at(index);
		}

		constexpr const_reference at(size_type index) const
		{
			return this->items.at(index);
		}

		reference operator[](size_type index)
		{
			return this->items.operator[](index);
		}

		constexpr const_reference operator[](size_type index) const
		{
			return this->items.operator[](index);
		}
	};
}