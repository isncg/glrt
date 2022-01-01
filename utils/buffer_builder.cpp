#include "buffer_builder.h"
buffer_builder::buffer_builder(int8_t* buffer, size_t size) :
	buffer(buffer), pointer(buffer), size(size)
{
}
