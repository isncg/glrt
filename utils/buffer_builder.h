#pragma once
#include <vector>
class buffer_builder
{
public:
	int size;
	int8_t* buffer;
	int8_t* pointer;
	buffer_builder(int8_t* buffer, int size);
	template<typename T>
	int8_t* append(const typename std::vector<T>& vec);
};


template<typename T>
inline int8_t* buffer_builder::append(const typename std::vector<T>& vec)
{
	int datasize = vectorsizeof(vec);
	if ((pointer - buffer) + datasize > size)
		return nullptr;
	memcpy_s(pointer, size - (pointer - buffer), &vec.front(), datasize);
	pointer += datasize;
	return pointer;
}