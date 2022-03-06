#pragma once
#include <glad/glad.h>
#include "buffer_builder.h"
#include "glhelpers.h"

struct vertex_attribute_info
{
	GLuint index;
	GLint size;
	GLenum type;
	GLboolean normalized;
	GLsizei stride;
	__int64 offset;
};

class vertex_buffer_builder
{
public:
	vertex_buffer_builder(int8_t* buffer, size_t size);
	GLuint attr_index;
	__int64 offset;
	std::vector<vertex_attribute_info> attr_info;
	buffer_builder bb;
	template<typename T>
	
	void append_attribute_data(const typename std::vector<T>& vec);
	void build();
};


template<typename T>
inline void vertex_buffer_builder::append_attribute_data(const typename std::vector<T>& vec)
{
	bool valid = vec.size() > 0;
	if (valid)
	{
		/*
		index
		Specifies the index of the generic vertex attribute to be modified.

		size
		Specifies the number of components per generic vertex attribute. Must be 1, 2, 3, 4. Additionally, the symbolic constant GL_BGRA is accepted by glVertexAttribPointer. The initial value is 4.

		type
		Specifies the data type of each component in the array.
		The symbolic constants GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT, GL_UNSIGNED_SHORT, GL_INT, and GL_UNSIGNED_INT are accepted by glVertexAttribPointer and glVertexAttribIPointer.
		Additionally GL_HALF_FLOAT, GL_FLOAT, GL_DOUBLE, GL_FIXED, GL_INT_2_10_10_10_REV, GL_UNSIGNED_INT_2_10_10_10_REV and GL_UNSIGNED_INT_10F_11F_11F_REV are accepted by glVertexAttribPointer.
		GL_DOUBLE is also accepted by glVertexAttribLPointer and is the only token accepted by the type parameter for that function. The initial value is GL_FLOAT.

		normalized
		For glVertexAttribPointer, specifies whether fixed-point data values should be normalized (GL_TRUE) or converted directly as fixed-point values (GL_FALSE) when they are accessed.

		stride
		Specifies the byte offset between consecutive generic vertex attributes. If stride is 0, the generic vertex attributes are understood to be tightly packed in the array. The initial value is 0.

		pointer
		Specifies a offset of the first component of the first generic vertex attribute in the array in the data store of the buffer currently bound to the GL_ARRAY_BUFFER target. The initial value is 0.*/
		auto size = GetVectorComponentCount(vec.front());
		auto type = GetVectorComponentType(vec.front());
		int stride = sizeof(T);
		attr_info.push_back(vertex_attribute_info{
			attr_index,
			size,
			type,
			GL_FALSE,
			stride,
			offset
			});
		auto ptr = bb.append(vec);
		offset = ptr - bb.buffer;
	}
	attr_index++;
}