#pragma once
#include "framework.h"
#include "Window.h"
#include "utils/singleton.h"
class Application: public Singleton<Application>
{
    SINGLETON_CTOR(Application)
public:

    HINSTANCE hInstance =0;
    HINSTANCE hPrevInstance =0;
    LPWSTR    lpCmdLine =0 ;
    int       nCmdShow = 0;

    void Init(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow);
    int Run(Window* pMainWindow);
    template<class T>
    int Run();
};

template<class T>
inline int Application::Run()
{
    return 0;
}

#define RUN_WINDOW(WNDCLASS) \
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,\
    _In_opt_ HINSTANCE hPrevInstance,\
    _In_ LPWSTR    lpCmdLine,\
    _In_ int       nCmdShow)\
{\
    Application::Instance().Init(hInstance, hPrevInstance, lpCmdLine, nCmdShow);\
    return Application::Instance().Run(new (WNDCLASS)());\
}