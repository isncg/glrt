#include <Windows.h>
#include <glad/glad.h>
#include <iostream>
#include <sstream>
#include <windowsx.h>
#include <strsafe.h>
#include <framework.h>
#include "../Resource.h"
#include "../include/GLRT.h"
#include <utils/utils.h>
typedef BOOL(APIENTRY* PFNWGLSWAPINTERVALFARPROC)(int);
PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT = 0;



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

class Win32FrameStatics : public IFrameStatics
{
	LARGE_INTEGER count{ 0 };
	LARGE_INTEGER frequency{ 0 };
	double fps;
	double deltaTime;
public:
	std::function<void()> UpdateFpsCallback;
	Win32FrameStatics(std::function<void()> updateFpsCallback)
	{
		this->UpdateFpsCallback = updateFpsCallback;
	}
	double fpsAveraged = 0;
	double fpsAvgAcc = 0;
	long long fpsAvgAccCount = 0;
	void Init();
	virtual double GetFPS() override;
	virtual double GetDelta() override;
	virtual void FrameUpdate() override;
};



void Window::UpdateTitle()
{
	title.str("");
	title << "GL_VERSION: " << glGetString(GL_VERSION) << " FPS: " << ((Win32FrameStatics*)frameStatics)->fpsAveraged;
	SetWindowTextA(hWnd, (LPCSTR)title.str().c_str());
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
		/*RECT rt;
		GetClientRect(hWnd, &rt);
		OnResize(rt.right - rt.left, rt.bottom - rt.top);*/
		OnIdle();
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		OnDestroy();
		break;
	case WM_MOUSEMOVE:
		mouseX = GET_X_LPARAM(lParam);
		mouseY = GET_Y_LPARAM(lParam);
		break;
	case WM_INPUT:
	{
		UINT dwSize;
		//HRESULT hResult;
		//TCHAR szTempOutput[1024];
		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
		if (dwSize == 0)
			return DefWindowProc(hWnd, message, wParam, lParam);
		LPBYTE lpb = new BYTE[dwSize];
		if (lpb == NULL)
		{
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
			OutputDebugString(TEXT("GetRawInputData does not return correct size !\n"));

		RAWINPUT* raw = (RAWINPUT*)lpb;

		if (raw->header.dwType == RIM_TYPEKEYBOARD)
		{
			OnKeyboard((KEYS)(raw->data.keyboard.VKey), (KEYACTION)(raw->data.keyboard.Flags & 0x3));
			break;
		}
		else if (raw->header.dwType == RIM_TYPEMOUSE)
		{
			bool handled = false;
			if (raw->data.mouse.usFlags == MOUSE_MOVE_RELATIVE)
			{
				OnMouseMove(raw->data.mouse.lLastX, raw->data.mouse.lLastY, mouseX, mouseY);
				handled = true;
			}

			if (raw->data.mouse.usButtonFlags == RI_MOUSE_WHEEL)
			{
				OnMouseWheel((short)raw->data.mouse.usButtonData);
				handled = true;
			}


			if (handled)
				break;
		}

		delete[] lpb;
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void Window::GetInitSize(long* width, long* height)
{
	*width = 1280;
	*height = 720;
}

bool Window::IsEnableVsync()
{
	return false;
}

void Window::OnPaint(HDC hdc)
{
}

void Window::OnDestroy()
{
	ReleaseDC(hWnd, hdc);
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
	if (GLVersion.major == 0 && GLVersion.minor == 0)
		gladLoadGL();
	wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress("wglSwapIntervalEXT");
	if (wglSwapIntervalEXT)
		wglSwapIntervalEXT(IsEnableVsync() ? 1 : 0);


	ready = true;
	auto frameStatics = new Win32FrameStatics([this]()
		{
			UpdateTitle();
		});
	frameStatics->Init();
	this->frameStatics = frameStatics;
}

void Window::OnResize(long width, long height)
{
	//if (ready)
	//	glViewport(0, 0, width, height);
}

void Window::OnIdle()
{
	if (!ready) return;
	glAssert("before onidle");
	BeforeRender();
	GLASSERT(RenderPipline());	
	AfterRender();
}

void Window::RenderPipline()
{
	Render();
	PostRender();
	OnGUI();
}

void Window::BeforeRender()
{
	wglMakeCurrent(GetDC(hWnd), hGLRC);
	GLASSERT(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	GLASSERT(glClearColor(0.2f, 0.2f, 0.2f, 1.0f));
	GLASSERT(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	RECT rt;
	GetClientRect(hWnd, &rt);
	GLASSERT(glViewport(0, 0, rt.right - rt.left, rt.bottom - rt.top));
}

void Window::Render()
{
}

void Window::PostRender()
{
}

void Window::OnGUI()
{
}

void Window::AfterRender()
{
	wglSwapLayerBuffers(hdc, WGL_SWAP_MAIN_PLANE);
	frameStatics->FrameUpdate();
}

void Window::OnMouseMove(long dx, long dy, long x, long y)
{

}

void Window::OnMouseWheel(int delta)
{
}

void Window::OnKeyboard(KEYS key, KEYACTION action)
{
}

Vector2 Window::GetViewportSize()
{
	RECT rect;
	GetClientRect(hWnd, &rect);
	return Vector2(rect.right - rect.left, rect.bottom - rect.top);
}

float Window::GetClientAspect()
{
	auto size = GetViewportSize();
	return size.x / size.y;
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

//IFrameStatics* Window::GetFrameStatics()
//{
//	return &frameStatics;
//}



void Win32FrameStatics::Init()
{
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&count);
	fps = 0;
}

double Win32FrameStatics::GetFPS()
{
	return fps;
}

double Win32FrameStatics::GetDelta()
{
	return deltaTime;
}

void Win32FrameStatics::FrameUpdate()
{
	LARGE_INTEGER current;
	QueryPerformanceCounter(&current);
	auto delta = current.QuadPart - count.QuadPart;
	this->deltaTime = (double)(delta) / frequency.QuadPart;
	this->fps = frequency.QuadPart / (double)(delta);
	count = current;

	fpsAvgAccCount++;
	fpsAvgAcc += fps;
	if (fpsAvgAccCount / fps >= 0.5)
	{
		fpsAveraged = fpsAvgAcc / fpsAvgAccCount;
		fpsAvgAccCount = 0;
		fpsAvgAcc = 0;
		//if (UpdateFpsCallback != nullptr)
			UpdateFpsCallback();
	}
}
