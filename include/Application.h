#pragma once
#include <framework.h>
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

    //const char* assetDir = "";
    void Init(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow);
    int Run(Window* pMainWindow, bool allocConsole = true);
    template<class T>
    int Run();
};

//std::string GetAssetPath(std::string && path);

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