#include <glad/glad.h>
#include<iostream>
#include<sstream>
#include "framework.h"
#include "Resource.h"
#include "Application.h"
#include "Window.h"
#include "utils/glhelpers.h"
static HMODULE glInst;
static void* cWGLGetProcAddr(const char* name)
{
	auto ret = wglGetProcAddress(name);
	if (ret == NULL)
		ret = GetProcAddress(glInst, name);
	return ret;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	Window* pWindow = (Window*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	if (pWindow == nullptr)
		return DefWindowProc(hWnd, message, wParam, lParam);
	return pWindow->WndProc(message, wParam, lParam);
}


LRESULT Window::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(Application::Instance().hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_CREATE:
		OnCreate();
		break;
	case WM_SIZE:
		OnResize(LOWORD(lParam), HIWORD(lParam));
		OnIdle();
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		OnDestroy();
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void Window::OnPaint(HDC hdc)
{
}

void Window::OnDestroy()
{
	ReleaseDC(hWnd, hdc);
	FreeLibrary(glInst);
	wglDeleteContext(hGLRC);
}

void Window::OnCreate()
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
}

void Window::OnResize(int width, int height)
{
	if (ready)
		glViewport(0, 0, width, height);
}

void Window::OnIdle()
{
	if (!ready) return;
	glAssert("before onidle");
	//HDC hdc = GetDC(hWnd);
	wglMakeCurrent(GetDC(hWnd), hGLRC);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	Render();
	wglSwapLayerBuffers(hdc, WGL_SWAP_MAIN_PLANE);
}

void Window::Render()
{
}

LPCWSTR Window::GetWindowClassName()
{
	return TEXT("Window");
}

void Window::PopulateClassInfo(WNDCLASSEXW* pwcex)
{
	auto hInstance = Application::Instance().hInstance;
	pwcex->cbSize = sizeof(WNDCLASSEX);
	pwcex->style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	pwcex->lpfnWndProc = ::WndProc;
	pwcex->cbClsExtra = 0;
	pwcex->cbWndExtra = 0;
	pwcex->hInstance = hInstance;
	pwcex->hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GLRT));
	pwcex->hCursor = LoadCursor(nullptr, IDC_ARROW);
	pwcex->hbrBackground = 0;
	pwcex->lpszMenuName = MAKEINTRESOURCEW(IDC_GLRT);
	pwcex->lpszClassName = GetWindowClassName();
	pwcex->hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SMALL));
}
