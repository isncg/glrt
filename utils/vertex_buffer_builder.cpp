#include "vertex_buffer_builder.h"

vertex_buffer_builder::vertex_buffer_builder(int8_t* buffer, size_t size) :
    attr_index(0), offset(0), bb(buffer, size)
{
}

void vertex_buffer_builder::build()
{
    GLASSERT(glBufferData(GL_ARRAY_BUFFER, offset, bb.buffer, GL_STATIC_DRAW));
    for (auto& attr : attr_info)
    {
        GLASSERT(glEnableVertexAttribArray(attr.index));
        GLASSERT(glVertexAttribPointer(attr.index, attr.size, attr.type, attr.normalized, attr.stride, (void*)attr.offset));
    }
}
