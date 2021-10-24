#include "OpenGLWindow.h"
#pragma comment (lib, "opengl32.lib")
#include<iostream>
#include<sstream>
#include"Renderer.h"
#include "utils/glhelpers.h"
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

	hdc = GetDC(hWnd);
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

	shader.Load("glsl/mesh.vert", "glsl/mesh.frag");
	shader.Use();

	mesh.vertices.push_back(Vector3{ -0.5,-0.75,0 });
	mesh.vertices.push_back(Vector3{  0.5,-0.75,0 });
	mesh.vertices.push_back(Vector3{  0,   0.75,0 });

	mesh.uv.push_back(Vector2{ 0,  0 });
	mesh.uv.push_back(Vector2{ 1,  0 });
	mesh.uv.push_back(Vector2{ 0.5,1 });

	mesh.triangles.push_back(0);
	mesh.triangles.push_back(1);
	mesh.triangles.push_back(2);

	renderer.Set(&mesh);
	ready = true;

	glAssert("oncreate finish");
}

void OpenGLWindow::OnDestroy()
{
	ReleaseDC(hWnd, hdc);
	FreeLibrary(glInst);
	wglDeleteContext(hGLRC);
}

void OpenGLWindow::OnIdle()
{
	glAssert("before onidle");
	HDC hdc = GetDC(hWnd);
	wglMakeCurrent(GetDC(hWnd), hGLRC);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	Render();
	wglSwapLayerBuffers(hdc, WGL_SWAP_MAIN_PLANE);
}

void OpenGLWindow::Render()
{
	if (!ready) return;
	glClearColor(0.2, 0.2, 0.2, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shader.Use();
	GLASSERT(renderer.Draw());
}
