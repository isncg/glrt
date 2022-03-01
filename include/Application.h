#pragma once
#include <framework.h>
#include <list>
#include "Window.h"
#include "../utils/singleton.h"
#include "../include/Script.h"
class Application: public Singleton<Application>, public IScriptable
{
    SINGLETON_CTOR(Application)
public:

    HINSTANCE hInstance =0;
    HINSTANCE hPrevInstance =0;
    LPWSTR    lpCmdLine =0 ;
    int       nCmdShow = 0;

    std::list<Window*> window_list;
    Window* pCurrentWindow;
    int ShowWindow(Window* pWindow);
    template<typename T>
    int ShowWindow();
    //const char* assetDir = "";
    void Init(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow);
    int Run(Window* pMainWindow = nullptr);
};

//std::string GetAssetPath(std::string && path);

template<typename T>
inline int Application::ShowWindow()
{
    return ShowWindow(new T);
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

//#define RUN_WINDOWEX(WNDCLASS, ASSETDIR) \
//int APIENTRY wWinMain(_In_ HINSTANCE hInstance,\
//    _In_opt_ HINSTANCE hPrevInstance,\
//    _In_ LPWSTR    lpCmdLine,\
//    _In_ int       nCmdShow)\
//{\
//    Application::Instance().Init(hInstance, hPrevInstance, lpCmdLine, nCmdShow);\
//    Application::Instance().assetDir = ASSETDIR;\
//    return Application::Instance().Run(new (WNDCLASS)());\
//}