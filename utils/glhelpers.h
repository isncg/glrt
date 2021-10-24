#pragma once
#include <glad/glad.h>
void glEnableVertexAttribArrayEx(int index, bool enable);
void glAssert(const char* info);

#define GLASSERT(expr) expr; \
glAssert(#expr)