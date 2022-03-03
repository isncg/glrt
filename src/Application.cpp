#include "../include/Application.h"
#include "../resource.h"
#include "../utils/utils.h"
#include "../include/ResourceMonitor.h"
#include "../include/Material.h"
#pragma comment (lib, "opengl32.lib")

extern void InitScript();


int Application::ShowWindow(Window* pWindow)
{
    WNDCLASSEXW wcex;
    bool isRegistered = GetClassInfoEx(hInstance, pWindow->GetWindowClassName(), &wcex);
    if (!isRegistered)
    {
        pWindow->PopulateClassInfo(&wcex);
        RegisterClassExW(&wcex);
    }

    RECT rect{ 0 };
    pWindow->GetInitSize(&rect.right, &rect.bottom);
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, TRUE);
    //HWND hWnd = InitInstance(hInstance, nCmdShow);
    HWND hWnd = CreateWindowW(wcex.lpszClassName, pWindow->szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return -1;
    }
    pWindow->hWnd = hWnd;
    SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pWindow);
    pWindow->OnCreate();
    ::ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    if (nullptr == pCurrentWindow)
        pCurrentWindow = pWindow;
    window_list.push_back(pWindow);
}

void Application::Init(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    this->hInstance = hInstance;
    this->nCmdShow = nCmdShow;
    AllocConsole();
    FILE* fDummy;
    freopen_s(&fDummy, "CONOUT$", "w", stdout);
    log("Application::Run");
    InitScript();
}

int Application::Run(Window* pMainWindow)
{
    RAWINPUTDEVICE Rid[2];

    Rid[0].usUsagePage = 0x01;          // HID_USAGE_PAGE_GENERIC
    Rid[0].usUsage = 0x02;              // HID_USAGE_GENERIC_MOUSE
    Rid[0].dwFlags = 0;    // adds mouse and also ignores legacy mouse messages
    Rid[0].hwndTarget = 0;

    Rid[1].usUsagePage = 0x01;          // HID_USAGE_PAGE_GENERIC
    Rid[1].usUsage = 0x06;              // HID_USAGE_GENERIC_KEYBOARD
    Rid[1].dwFlags = 0;    // adds keyboard and also ignores legacy keyboard messages
    Rid[1].hwndTarget = 0;

    if (RegisterRawInputDevices(Rid, 2, sizeof(Rid[0])) == FALSE)
    {
        log("reg err");
        //registration failed. Call GetLastError for the cause of the error
    }

    if (pMainWindow)
        ShowWindow(pMainWindow);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GLRT));
    MSG msg;
    ResourceMonitor::Instance().Start();
    InvokeScript("init");
    
    Window* pMsgWindow = nullptr;
    while (true)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            pMsgWindow = (Window*)GetWindowLongPtr(msg.hwnd, GWLP_USERDATA);
            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            if (msg.message == WM_QUIT)
            {
                if (nullptr == pMsgWindow)
                    break;
                window_list.remove(pMsgWindow);
                if (pCurrentWindow == pMsgWindow)
                    pCurrentWindow = window_list.front();
                
                delete pMsgWindow;
                if (window_list.size() == 0)
                    break;
            }

        }
        else
        {
            pMsgWindow = nullptr;
            for (auto wnd : window_list)
                GLASSERT(wnd->OnIdle());
            //GLASSERT(pMainWindow->OnIdle());
            GLASSERT(ResourceMonitor::Instance().NotifyAll());
            GLASSERT(GlobalMaterial::Instance().Use());
        }
    }

    return (int)msg.wParam;
}

//std::string GetAssetPath(std::string && path)
//{
//    return string_format("%s%s", Application::Instance().assetDir, path.c_str());
//}



#include <glm/glm.hpp>
GLint GetVectorComponentCount(const Color32& tag) { return 4; }
GLint GetVectorComponentCount(const Color& tag) { return 4; }
GLint GetVectorComponentCount(const ColorRGB& tag) { return 3; }

GLenum GetVectorComponentType(const Vector3& tag) { return GL_FLOAT; }
GLenum GetVectorComponentType(const Vector4& tag) { return GL_FLOAT; }
GLenum GetVectorComponentType(const Vector2& tag) { return GL_FLOAT; }
GLenum GetVectorComponentType(const Color&   tag)   { return GL_FLOAT; }
GLenum GetVectorComponentType(const ColorRGB& tag){ return GL_FLOAT; }
GLenum GetVectorComponentType(const Color32& tag) { return GL_UNSIGNED_BYTE; }

#include <windowsx.h>
class GLApp_Win32PlatformContext : public GLApp_PlatformContext
{
public:
    HINSTANCE hInstance;
    int       nCmdShow;
    //friend LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    bool ready = false;
    LONG mouseX, mouseY;
    std::stringstream title;
    void UpdateTitle();
    HWND hWnd;
    HDC hdc;
    HGLRC hGLRC;
    LPCWSTR szTitle = TEXT("GLRT");

    LARGE_INTEGER count{ 0 };
    LARGE_INTEGER frequency{ 0 };
    double fps;
    double deltaTime;
    double fpsAveraged = 0;
    double fpsAvgAcc = 0;
    long long fpsAvgAccCount = 0;

}*pWin32PlatformContext;

INT_PTR CALLBACK GLApp_About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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

#define GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))

LRESULT CALLBACK GLApp_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(pWin32PlatformContext->hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, GLApp_About);
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
        pWin32PlatformContext->pApp->OnCreate();
        break;
    case WM_SIZE:
        if (GLVersion.major == 0 && GLVersion.minor == 0)
            return DefWindowProc(hWnd, message, wParam, lParam);
		pWin32PlatformContext->pApp->OnResize(LOWORD(lParam), HIWORD(lParam));
		pWin32PlatformContext->pApp->Update();
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        pWin32PlatformContext->pApp->OnDestroy();
        break;
    case WM_MOUSEMOVE:
        pWin32PlatformContext->mouseX = GET_X_LPARAM(lParam);
        pWin32PlatformContext->mouseY = GET_Y_LPARAM(lParam);
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
            pWin32PlatformContext->pApp->OnKeyboard((KEYS)(raw->data.keyboard.VKey), (KEYACTION)(raw->data.keyboard.Flags & 0x3));
            break;
        }
        else if (raw->header.dwType == RIM_TYPEMOUSE)
        {
            bool handled = false;
            if (raw->data.mouse.usFlags == MOUSE_MOVE_RELATIVE)
            {
                pWin32PlatformContext->pApp->OnMouseMove(raw->data.mouse.lLastX, raw->data.mouse.lLastY, pWin32PlatformContext->mouseX, pWin32PlatformContext->mouseY);
                handled = true;
            }

            if (raw->data.mouse.usButtonFlags == RI_MOUSE_WHEEL)
            {
                pWin32PlatformContext->pApp->OnMouseWheel((short)raw->data.mouse.usButtonData);
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

GLApp::GLApp(GLApp_PlatformContext* context)
{
    if (nullptr != context)
    {
        context->pApp = this;
        return;
    }
    pWin32PlatformContext = new GLApp_Win32PlatformContext;
    platform_context = pWin32PlatformContext;
    platform_context->pApp = this;
}

void GLApp::Start()
{
}

void GLApp::Update()
{
	auto ctx = (GLApp_Win32PlatformContext*)platform_context;
    LARGE_INTEGER current;
    QueryPerformanceCounter(&current);
    auto delta = current.QuadPart - ctx->count.QuadPart;
    ctx->deltaTime = (double)(delta) / ctx->frequency.QuadPart;
    ctx->fps = ctx->frequency.QuadPart / (double)(delta);
    ctx->count = current;

    ctx->fpsAvgAccCount++;
    ctx->fpsAvgAcc += ctx->fps;
    if (ctx->fpsAvgAccCount / ctx->fps >= 0.5)
    {
        ctx->fpsAveraged = ctx->fpsAvgAcc / ctx->fpsAvgAccCount;
        ctx->fpsAvgAccCount = 0;
        ctx->fpsAvgAcc = 0;
        //if (UpdateFpsCallback != nullptr)
        //UpdateFpsCallback();
    }
}

void GLApp::GetInitSize(long* width, long* height)
{
}

void GLApp::OnCreate()
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

    GLApp_Win32PlatformContext* ctx = (GLApp_Win32PlatformContext*)platform_context;
    ctx->hdc = GetDC(ctx->hWnd);
    int pixelFormat;
    pixelFormat = ChoosePixelFormat(ctx->hdc, &pfd);
    SetPixelFormat(ctx->hdc, pixelFormat, &pfd);

    ctx->hGLRC = wglCreateContext(ctx->hdc);
    wglMakeCurrent(ctx->hdc, ctx->hGLRC);
    if (GLVersion.major == 0 && GLVersion.minor == 0)
        gladLoadGL();
    //wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress("wglSwapIntervalEXT");
    //if (wglSwapIntervalEXT)
    //    wglSwapIntervalEXT(IsEnableVsync() ? 1 : 0);

    QueryPerformanceFrequency(&ctx->frequency);
    QueryPerformanceCounter(&ctx->count);
    ctx->fps = 0;
}

void GLApp::OnDestroy()
{
}

void GLApp::OnResize(long width, long height)
{
}

void GLApp::OnMouseMove(long dx, long dy, long x, long y)
{
}

void GLApp::OnMouseWheel(int delta)
{
}

void GLApp::OnKeyboard(KEYS key, KEYACTION action)
{
}

Vector2 GLApp::GetClientSize()
{
    return Vector2();
}

float GLApp::GetClientAspect()
{
    return 0.0f;
}

void GLApp::Run()
{
    AllocConsole();
    FILE* fDummy;
    freopen_s(&fDummy, "CONOUT$", "w", stdout);
    log("Application::Run");
    InitScript();

    RAWINPUTDEVICE Rid[2];

    Rid[0].usUsagePage = 0x01;          // HID_USAGE_PAGE_GENERIC
    Rid[0].usUsage = 0x02;              // HID_USAGE_GENERIC_MOUSE
    Rid[0].dwFlags = 0;    // adds mouse and also ignores legacy mouse messages
    Rid[0].hwndTarget = 0;

    Rid[1].usUsagePage = 0x01;          // HID_USAGE_PAGE_GENERIC
    Rid[1].usUsage = 0x06;              // HID_USAGE_GENERIC_KEYBOARD
    Rid[1].dwFlags = 0;    // adds keyboard and also ignores legacy keyboard messages
    Rid[1].hwndTarget = 0;

    if (RegisterRawInputDevices(Rid, 2, sizeof(Rid[0])) == FALSE)
    {
        log("reg err");
        //registration failed. Call GetLastError for the cause of the error
    }


    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEXW);
    wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wcex.lpfnWndProc = ::GLApp_WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = pWin32PlatformContext->hInstance;
    wcex.hIcon = LoadIcon(pWin32PlatformContext->hInstance, MAKEINTRESOURCE(IDI_GLRT));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = 0;
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_GLRT);
    wcex.lpszClassName = TEXT("GLRT");
    wcex.hIconSm = LoadIcon(pWin32PlatformContext->hInstance, MAKEINTRESOURCE(IDI_SMALL));

    RECT rect{ 0 };
    GetInitSize(&rect.right, &rect.bottom);
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, TRUE);
    //HWND hWnd = InitInstance(hInstance, nCmdShow);
    HWND hWnd = CreateWindowW(wcex.lpszClassName, wcex.lpszClassName, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, pWin32PlatformContext->hInstance, nullptr);

    if (!hWnd)
        return;
    pWin32PlatformContext->hWnd = hWnd;
    OnCreate();
    ShowWindow(hWnd, pWin32PlatformContext->nCmdShow);
    UpdateWindow(hWnd);


    HACCEL hAccelTable = LoadAccelerators(pWin32PlatformContext->hInstance, MAKEINTRESOURCE(IDC_GLRT));
    MSG msg;
    ResourceMonitor::Instance().Start();
    InvokeScript("init");

    while (true)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            if (msg.message == WM_QUIT && msg.hwnd == ((GLApp_Win32PlatformContext*)platform_context)->hWnd)
                break;
        }
        else
        {
            Update();
            GLASSERT(ResourceMonitor::Instance().NotifyAll());
            GLASSERT(GlobalMaterial::Instance().Use());
        }
    }
}

#ifdef GLAPP_RUN
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
    win32_instance->hInstance = hInstance;
    win32_instance->nCmdShow = nCmdShow;
	win32_instance->pApp->Run();
	return 0;
}
#endif