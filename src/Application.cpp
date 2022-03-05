#include "../include/Application.h"
#include "../resource.h"
#include "../utils/utils.h"
#include "../include/ResourceMonitor.h"
#include "../include/Material.h"
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_impl_opengl3.h>

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
    return 0;
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
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK GLApp_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (pWin32PlatformContext->hWnd == hWnd)
    {
        ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam);
        return pWin32PlatformContext->pApp->WndProc(message, wParam, lParam);
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

GLApp::GLApp(GLApp_PlatformContext* context)
{
    if (nullptr != context)
    {
        this->platform_context = context;
        context->pApp = this;
        return;
    }
    if (nullptr == pWin32PlatformContext)
    {
        pWin32PlatformContext = new GLApp_Win32PlatformContext;
        platform_context = pWin32PlatformContext;
        platform_context->pApp = this;
    }
}

void GLApp::Start()
{
}

void GLApp::Update()
{
}

void GLApp::GetInitSize(long* width, long* height)
{
    *width = 1280;
    *height = 720;
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

void GLApp::OnImGUI()
{
}

#ifdef WIN32
LRESULT GLApp::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    auto ctx = (GLApp_Win32PlatformContext*)platform_context;
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(ctx->hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), ctx->hWnd, GLApp_About);
            break;
        case IDM_EXIT:
            DestroyWindow(ctx->hWnd);
            break;
        default:
            return DefWindowProc(ctx->hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_CREATE:
        //pWin32PlatformContext->pApp->OnCreate();
        break;
    case WM_SIZE:
        if (!ctx->ready)
            return DefWindowProc(ctx->hWnd, message, wParam, lParam);
        ctx->pApp->OnResize(LOWORD(lParam), HIWORD(lParam));
        ctx->pApp->Update();
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        ctx->pApp->OnDestroy();
        break;
    case WM_MOUSEMOVE:
        ctx->mouseX = GET_X_LPARAM(lParam);
        ctx->mouseY = GET_Y_LPARAM(lParam);
        break;
    case WM_INPUT:
    {
        UINT dwSize;
        //HRESULT hResult;
        //TCHAR szTempOutput[1024];
        GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
        if (dwSize == 0)
            return DefWindowProc(ctx->hWnd, message, wParam, lParam);
        LPBYTE lpb = new BYTE[dwSize];
        if (lpb == NULL)
        {
            return DefWindowProc(ctx->hWnd, message, wParam, lParam);
        }
        if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
            OutputDebugString(TEXT("GetRawInputData does not return correct size !\n"));

        RAWINPUT* raw = (RAWINPUT*)lpb;

        if (raw->header.dwType == RIM_TYPEKEYBOARD)
        {
            ctx->pApp->OnKeyboard((KEYS)(raw->data.keyboard.VKey), (KEYACTION)(raw->data.keyboard.Flags & 0x3));
            break;
        }
        else if (raw->header.dwType == RIM_TYPEMOUSE)
        {
            bool handled = false;
            if (raw->data.mouse.usFlags == MOUSE_MOVE_RELATIVE)
            {
                ctx->pApp->OnMouseMove(raw->data.mouse.lLastX, raw->data.mouse.lLastY, pWin32PlatformContext->mouseX, pWin32PlatformContext->mouseY);
                handled = true;
            }

            if (raw->data.mouse.usButtonFlags == RI_MOUSE_WHEEL)
            {
                ctx->pApp->OnMouseWheel((short)raw->data.mouse.usButtonData);
                handled = true;
            }


            if (handled)
                break;
        }

        delete[] lpb;
        return DefWindowProc(ctx->hWnd, message, wParam, lParam);
    }
    default:
        return DefWindowProc(ctx->hWnd, message, wParam, lParam);
    }
    return 0;
}
#endif

Vector2 GLApp::GetClientSize()
{
#ifdef WIN32
    RECT rect;
    GetClientRect(((GLApp_Win32PlatformContext*)platform_context)->hWnd, &rect);
    return Vector2(rect.right - rect.left, rect.bottom - rect.top);
#endif
    return Vector2();
}

float GLApp::GetClientAspect()
{
    auto size = GetClientSize();
    return size.x / size.y;
}

void GLApp::Run()
{
    auto ctx = (GLApp_Win32PlatformContext*)platform_context;
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


	WNDCLASSEXW wcex{ 0 };
    wcex.cbSize = sizeof(WNDCLASSEXW);
    wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wcex.lpfnWndProc = ::GLApp_WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = ctx->hInstance;
    wcex.hIcon = LoadIcon(ctx->hInstance, MAKEINTRESOURCE(IDI_GLRT));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = 0;
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_GLRT);
    wcex.lpszClassName = TEXT("GLRT");
    wcex.hIconSm = LoadIcon(ctx->hInstance, MAKEINTRESOURCE(IDI_SMALL));

    RECT rect{ 0 };
    GetInitSize(&rect.right, &rect.bottom);
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, TRUE);
    RegisterClassExW(&wcex);
    HWND hWnd = CreateWindowW(wcex.lpszClassName, wcex.lpszClassName, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, ctx->hInstance, nullptr);

    if (!hWnd)
        return;
    ctx->hWnd = hWnd;
    //OnCreate();
    ShowWindow(hWnd, ctx->nCmdShow);
    UpdateWindow(hWnd);

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

    //GLApp_Win32PlatformContext* ctx = (GLApp_Win32PlatformContext*)platform_context;
    ctx->hdc = GetDC(ctx->hWnd);
    int pixelFormat;
    pixelFormat = ChoosePixelFormat(ctx->hdc, &pfd);
    SetPixelFormat(ctx->hdc, pixelFormat, &pfd);

    ctx->hGLRC = wglCreateContext(ctx->hdc);
    wglMakeCurrent(ctx->hdc, ctx->hGLRC);
    if (GLVersion.major == 0 && GLVersion.minor == 0)
        gladLoadGL();

    ImGui::CreateContext();
    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplOpenGL3_Init("#version 330");

    QueryPerformanceFrequency(&ctx->frequency);
    QueryPerformanceCounter(&ctx->count);
    ctx->fps = 0;


    HACCEL hAccelTable = LoadAccelerators(ctx->hInstance, MAKEINTRESOURCE(IDC_GLRT));
    MSG msg;
    ResourceMonitor::Instance().Start();
    InvokeScript("init");

    Start();
    ctx->ready = true;
    while (true)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            if (msg.message == WM_QUIT)
                break;
        }
        else
        {
            wglMakeCurrent(ctx->hdc, ctx->hGLRC);
            Update();
            GLASSERT(ImGui_ImplOpenGL3_NewFrame());
            GLASSERT(ImGui_ImplWin32_NewFrame());
            GLASSERT(ImGui::NewFrame());
            GLASSERT(OnImGUI());
            GLASSERT(ImGui::Render());
            GLASSERT(ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()));
            wglSwapLayerBuffers(ctx->hdc, WGL_SWAP_MAIN_PLANE);

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
            }

            GLASSERT(ResourceMonitor::Instance().NotifyAll());
            GLASSERT(GlobalMaterial::Instance().Use());
        }
    }
}

#ifdef GLAPP_RUN
#ifdef WIN32
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
    if (nullptr == pWin32PlatformContext)
        return -1;
    pWin32PlatformContext->hInstance = hInstance;
    pWin32PlatformContext->nCmdShow = nCmdShow;
    pWin32PlatformContext->pApp->Run();
	return 0;
}
#endif
#endif


#include "../include/Camera.h"
#include <examples/empty3d/empty3d.h>
struct Viewer3D::Context
{
    Shader* m_pGridShader;
    Shader* m_pAxisShader;
    GLuint gridVAO = 0;
    GLuint axis = 0;
    GLuint skybox = 0;
    long w = 1280, h = 720;

    Camera						m_Camera;
    CameraFirstPersonController m_CamController;
};

void Viewer3D::InitHorizonGrid()
{
    pctx_viewer3d->m_pGridShader = ShaderLib::Instance().Load(ASSETPATH("glsl/grid"));
    //gridShader.Load(ASSETPATH("glsl/grid.vert"), ASSETPATH("glsl/grid.frag"));
    std::vector<Vector2> buffer;
    for (int i = -50; i <= 50; i++)
    {
        buffer.push_back(Vector2{ -50, i });
        buffer.push_back(Vector2{ 50, i });
        buffer.push_back(Vector2{ i, -50 });
        buffer.push_back(Vector2{ i, 50 });
    }
    GLASSERT(glGenVertexArrays(1, &pctx_viewer3d->gridVAO));
    GLASSERT(glBindVertexArray(pctx_viewer3d->gridVAO));
    GLuint vbo;
    GLASSERT(glGenBuffers(1, &vbo));
    GLASSERT(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    signed   long long int bufsize = buffer.size() * sizeof(Vector2);
    GLASSERT(glBufferData(GL_ARRAY_BUFFER, bufsize, &buffer[0], GL_STATIC_DRAW));
    GLASSERT(glEnableVertexAttribArray(0));
    GLASSERT(glVertexAttribPointer(0,
        GetVectorComponentCount(buffer.front()),
        GetVectorComponentType(buffer.front()),
        GL_FALSE,
        sizeof(Vector2), 0));
    GLASSERT(glBindVertexArray(0));
    GLASSERT(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void Viewer3D::InitAxis()
{
    pctx_viewer3d->m_pAxisShader = ShaderLib::Instance().Load(ASSETPATH("glsl/axis"));
    //axisShader.Load(ASSETPATH("glsl/axis.vert"), ASSETPATH("glsl/axis.frag"));
    std::vector<Vector3> buffer;
    buffer.push_back(Vector3{ 0,0,0 }); buffer.push_back(Vector3{ 1,0,0 });
    buffer.push_back(Vector3{ 1,0,0 }); buffer.push_back(Vector3{ 1,0,0 });

    buffer.push_back(Vector3{ 0,0,0 }); buffer.push_back(Vector3{ 0,1,0 });
    buffer.push_back(Vector3{ 0,1,0 }); buffer.push_back(Vector3{ 0,1,0 });

    buffer.push_back(Vector3{ 0,0,0 }); buffer.push_back(Vector3{ 0,0,1 });
    buffer.push_back(Vector3{ 0,0,1 }); buffer.push_back(Vector3{ 0,0,1 });

    GLASSERT(glGenVertexArrays(1, &pctx_viewer3d->axis));
    GLASSERT(glBindVertexArray(pctx_viewer3d->axis));
    GLuint vbo;
    GLASSERT(glGenBuffers(1, &vbo));
    GLASSERT(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    auto bufsize = buffer.size() * sizeof(Vector3);
    GLASSERT(glBufferData(GL_ARRAY_BUFFER, bufsize, &buffer[0], GL_STATIC_DRAW));
    GLASSERT(glEnableVertexAttribArray(0));
    GLASSERT(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), 0));
    GLASSERT(glEnableVertexAttribArray(1));
    GLASSERT(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), (void*)sizeof(Vector3)));
    GLASSERT(glBindVertexArray(0));
    GLASSERT(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void Viewer3D::SetCameraStartupParam(CameraStartupParam& param)
{
    param.projection.fovY = 3.1415926f / 3;
    param.projection.zFar = 5000;
    param.projection.zNear = 1;

    param.position = Vector3{ 4,3,4 };
    param.direction = -param.position;
    param.moveSpeed = 10.0f;
}

void Viewer3D::RenderScene()
{
}

void Viewer3D::Start()
{
    this->pctx_viewer3d = new Context;

    CameraStartupParam camparam;
    SetCameraStartupParam(camparam);
    pctx_viewer3d->m_Camera.SetProjectionMatrix(camparam.projection.fovY, GetClientAspect(), camparam.projection.zNear, camparam.projection.zFar);
    pctx_viewer3d->m_CamController.camera = &pctx_viewer3d->m_Camera;
    pctx_viewer3d->m_CamController.speed = camparam.moveSpeed;
    Vector3 initCamPos = camparam.position;
    Vector2 initCamYallPitch = Camera::GetYallPitchFromDirection(camparam.direction);
    /*	Vector3 initCamDir = camparam.direction;
        initCamDir = glm::normalize(initCamDir);
        vector
        float yall = glm::atan(initCamDir.z, initCamDir.x);*/
    pctx_viewer3d->m_CamController.Setup(initCamPos, initCamYallPitch.x, initCamYallPitch.y);

    InitHorizonGrid();
    InitAxis();
}

void Viewer3D::Update()
{
    GLASSERT(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    GLASSERT(glClearColor(0.2f, 0.2f, 0.2f, 1.0f));
    GLASSERT(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    RECT rt;
    auto size = GetClientSize();
    GLASSERT(glViewport(0, 0, size.x, size.y));

    DrawHorizonGrid(0.1f);
    DrawHorizonGrid(1.0f);
    DrawHorizonGrid(10.0f);
    DrawHorizonGrid(100.0f);

    RenderScene();

    GLASSERT(DrawAxis());
    pctx_viewer3d->m_CamController.Update(((GLApp_Win32PlatformContext*)platform_context)->deltaTime);
    GlobalMaterial::Instance().SetMainCamera(&pctx_viewer3d->m_Camera);
}

void Viewer3D::OnMouseMove(long dx, long dy, long x, long y) { pctx_viewer3d->m_CamController.OnMouseMove(dx, dy, x, y); }

void Viewer3D::OnKeyboard(KEYS key, KEYACTION action){ pctx_viewer3d->m_CamController.OnKeyboard(key, action);}

#ifdef WIN32
LRESULT Viewer3D::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    return GLApp::WndProc(message, wParam, lParam);
}
#endif // WIN32


void Viewer3D::DrawHorizonGrid(float size)
{
    GLASSERT(glDisable(GL_DEPTH_TEST));
    auto pos = pctx_viewer3d->m_CamController.position;
    Vector2 offset;
    offset.x = (long)(pos.x / size) * size;
    offset.y = (long)(pos.z / size) * size;
    pctx_viewer3d->m_pGridShader->Use();
    pctx_viewer3d->m_pGridShader->Set("cam", pctx_viewer3d->m_Camera.GetMatrix());
    pctx_viewer3d->m_pGridShader->Set("size", size);
    pctx_viewer3d->m_pGridShader->Set("viewpos", pctx_viewer3d->m_CamController.position);
    pctx_viewer3d->m_pGridShader->Set("gridcolor", Vector3{ 1,1,1 });
    pctx_viewer3d->m_pGridShader->Set("offset", offset);
    GLASSERT(glBindVertexArray(pctx_viewer3d->gridVAO));
    GLASSERT(glDrawArrays(GL_LINES, 0, 404));
    GLASSERT(glEnable(GL_DEPTH_TEST));
}
void Viewer3D::DrawAxis()
{
    GLASSERT(glDisable(GL_DEPTH_TEST));
    pctx_viewer3d->m_pAxisShader->Use();
    pctx_viewer3d->m_pAxisShader->Set("_mvp",
        pctx_viewer3d->m_Camera.GetProjectionMatrix() *
        Matrix4x4::LookAt(-pctx_viewer3d->m_CamController.GetForwardDirection() * 5.0f * pctx_viewer3d->m_Camera.projectionParam.zNear, Vector3{ 0,0,0 }, Vector3{ 0,1,0 }));
    GLASSERT(glBindVertexArray(pctx_viewer3d->axis));
    GLASSERT(glDrawArrays(GL_LINES, 0, 12));
    GLASSERT(glEnable(GL_DEPTH_TEST));
}

Viewer3D _;
