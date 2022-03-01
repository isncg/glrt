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

    RAWINPUTDEVICE Rid[2];

    Rid[0].usUsagePage = 0x01;          // HID_USAGE_PAGE_GENERIC
    Rid[0].usUsage = 0x02;              // HID_USAGE_GENERIC_MOUSE
    Rid[0].dwFlags = 0;    // adds mouse and also ignores legacy mouse messages
    Rid[0].hwndTarget = hWnd;

    Rid[1].usUsagePage = 0x01;          // HID_USAGE_PAGE_GENERIC
    Rid[1].usUsage = 0x06;              // HID_USAGE_GENERIC_KEYBOARD
    Rid[1].dwFlags = 0;    // adds keyboard and also ignores legacy keyboard messages
    Rid[1].hwndTarget = hWnd;

    if (RegisterRawInputDevices(Rid, 2, sizeof(Rid[0])) == FALSE)
    {
        log("reg err");
        //registration failed. Call GetLastError for the cause of the error
    }

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
    //if (allocConsole)
    //{
    //    //std::cout << "Application::Run" << std::endl;
    //}
 //   WNDCLASSEXW wcex;
 //   bool isRegistered = GetClassInfoEx(hInstance, pMainWindow->GetWindowClassName(), &wcex);
 //   if (!isRegistered)
 //   {
 //       pMainWindow->PopulateClassInfo(&wcex);
 //       RegisterClassExW(&wcex);
 //   }

 //   RECT rect{ 0 };
 //   pMainWindow->GetInitSize(&rect.right, &rect.bottom);
 //   AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, TRUE);
 //   //HWND hWnd = InitInstance(hInstance, nCmdShow);
 //   HWND hWnd = CreateWindowW(wcex.lpszClassName, pMainWindow->szTitle, WS_OVERLAPPEDWINDOW,
 //       CW_USEDEFAULT, 0, rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, hInstance, nullptr);

 //   if (!hWnd)
 //   {
 //       return -1;
 //   }
 //   pMainWindow->hWnd = hWnd;
 //   SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pMainWindow);
	//pMainWindow->OnCreate();
 //   ::ShowWindow(hWnd, nCmdShow);
 //   UpdateWindow(hWnd);

    //RAWINPUTDEVICE Rid[2];

    //Rid[0].usUsagePage = 0x01;          // HID_USAGE_PAGE_GENERIC
    //Rid[0].usUsage = 0x02;              // HID_USAGE_GENERIC_MOUSE
    //Rid[0].dwFlags = 0;    // adds mouse and also ignores legacy mouse messages
    //Rid[0].hwndTarget = hWnd;

    //Rid[1].usUsagePage = 0x01;          // HID_USAGE_PAGE_GENERIC
    //Rid[1].usUsage = 0x06;              // HID_USAGE_GENERIC_KEYBOARD
    //Rid[1].dwFlags = 0;    // adds keyboard and also ignores legacy keyboard messages
    //Rid[1].hwndTarget = hWnd;

    //if (RegisterRawInputDevices(Rid, 2, sizeof(Rid[0])) == FALSE)
    //{
    //    log("reg err");
    //    //registration failed. Call GetLastError for the cause of the error
    //}

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

			for (auto wnd : window_list)
				if (wnd != pMsgWindow)
					GLASSERT(wnd->OnIdle());
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