#include "OpenGLWindow.h"
#pragma comment (lib, "opengl32.lib")
#include<iostream>
#include<sstream>
void OpenGLWindow::PopulateClassInfo(WNDCLASSEXW* pwcex)
{
	Window::PopulateClassInfo(pwcex);
	pwcex->style |= CS_OWNDC;
}


//https://blog.csdn.net/YanEast/article/details/107311868
static HMODULE glInst;
static void* cWGLGetProcAddr(const char* name)
{
	auto ret = wglGetProcAddress(name);
	if (ret == NULL)
		ret = GetProcAddress(glInst, name);
	return ret;
}

void OpenGLWindow::OnCreate()
{
	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    //Flags
		PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
		32,                   // Colordepth of the framebuffer.
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		24,                   // Number of bits for the depthbuffer
		8,                    // Number of bits for the stencilbuffer
		0,                    // Number of Aux buffers in the framebuffer.
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};

	HDC hdc = GetDC(hWnd);
	int  pixelFormat;
	pixelFormat = ChoosePixelFormat(hdc, &pfd);
	SetPixelFormat(hdc, pixelFormat, &pfd);

	hGLRC = wglCreateContext(hdc);
	wglMakeCurrent(hdc, hGLRC);
	glInst = LoadLibrary(TEXT("opengl32.dll"));
	gladLoadGLLoader(cWGLGetProcAddr);

	std::stringstream ss;
	ss << "GL_VERSION: " << glGetString(GL_VERSION);
	SetWindowTextA(hWnd, (LPCSTR)ss.str().c_str());
}
