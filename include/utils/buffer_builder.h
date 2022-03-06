#pragma once
#include <vector>
#include "stdhelpers.h"
class buffer_builder
{
public:
	size_t size;
	int8_t* buffer;
	int8_t* pointer;
	buffer_builder(int8_t* buffer, size_t size);
	template<typename T>
	int8_t* append(const typename std::vector<T>& vec);
};


template<typename T>
inline int8_t* buffer_builder::append(const typename std::vector<T>& vec)
{
	auto datasize = vectorsizeof(vec);
	if ((pointer - buffer) + datasize > size)
		return nullptr;
	memcpy_s(pointer, size - (pointer - buffer), &vec.front(), datasize);
	pointer += datasize;
	return pointer;
}