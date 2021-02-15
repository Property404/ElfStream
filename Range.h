#pragma once
#include <cstddef>
struct Range
{
	size_t start = 0;
	size_t size = 0;

	Range(){};

	Range(const size_t _start, const size_t _size)
	{start = _start; size=_size;}

	size_t getEnd() const noexcept{
		return start+size;
	}
};
