#include "Application.h"
#include "resource.h"
#include "utils/log.h"
#pragma comment (lib, "opengl32.lib")

void Application::Init(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    this->hInstance = hInstance;
    this->nCmdShow = nCmdShow;
}

int Application::Run(Window* pMainWindow, bool allocConsole)
{
    if (allocConsole)
    {
        AllocConsole();
        FILE* fDummy;
        freopen_s(&fDummy, "CONOUT$", "w", stdout);
        //std::cout << "Application::Run" << std::endl;
    }
    log("Application::Run");
    WNDCLASSEXW wcex;
    bool isRegistered = GetClassInfoEx(hInstance, pMainWindow->GetWindowClassName(), &wcex);
    if (!isRegistered)
    {
        pMainWindow->PopulateClassInfo(&wcex);
        RegisterClassExW(&wcex);
    }

	RECT rect{ 0,0,1280,720 };
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, TRUE);
    //HWND hWnd = InitInstance(hInstance, nCmdShow);
    HWND hWnd = CreateWindowW(wcex.lpszClassName, pMainWindow->szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return -1;
    }
    pMainWindow->hWnd = hWnd;
    SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pMainWindow);
	pMainWindow->OnCreate();
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);


    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GLRT));
    MSG msg;

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
            pMainWindow->OnIdle();
        }
    }

    return (int)msg.wParam;
}



