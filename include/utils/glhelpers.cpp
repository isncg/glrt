#include <Windows.h>
#include "glhelpers.h"
#include "stdhelpers.h"
void glEnableVertexAttribArrayEx(int index, bool enable)
{
	if (enable)
		glEnableVertexAttribArray(index);
	else
		glDisableVertexAttribArray(index);
}


void glAssert(const char* info)
{
	GLenum err = glGetError();
	if (err)
	{
		MessageBoxA(NULL, string_format("[%d]\n%s", err, info).c_str(), "OpenGL Error", MB_ICONERROR);
	}
}