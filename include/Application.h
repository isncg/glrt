#pragma once
#include <framework.h>
#include <list>
#include "Window.h"
#include "../utils/singleton.h"
#include "../include/Script.h"
#include "../utils/stdhelpers.h"
#include "../include/Camera.h"

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
class GLApp;
class GLApp_PlatformContext
{
public:
    GLApp* pApp;
};

class GLApp: IScriptable
{
protected:
    GLApp_PlatformContext* platform_context = nullptr;
public:
    GLApp(GLApp_PlatformContext* context = nullptr);
    virtual void Start();
    virtual void Update();
    virtual void GetInitSize(long* width, long* height);
    //virtual void OnCreate();
    virtual void OnDestroy();
    virtual void OnResize(long width, long height);
    virtual void OnMouseMove(long dx, long dy, long x, long y);
    virtual void OnMouseWheel(int delta);
    virtual void OnKeyboard(KEYS key, KEYACTION action);
    virtual void OnImGUI();
#ifdef WIN32
    virtual LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
#endif
public:
    Vector2 GetClientSize();
    float GetClientAspect();
    void Run();
};

class Viewer3D : public GLApp
{
    ASSETDIR("viewer3d/assets/");
    struct Context;
    Context* pctx_viewer3d;
    struct CameraStartupParam
    {
        CameraProjectionParam projection;
        Vector3 position;
        Vector3 direction;
        float moveSpeed;
    };
    void InitHorizonGrid();
    void InitAxis();
protected:
    virtual void SetCameraStartupParam(CameraStartupParam& param);
    virtual void RenderScene();
public:
    virtual void Start() override;
    virtual void Update() override;
    virtual void OnMouseMove(long dx, long dy, long x, long y) override;
    virtual void OnKeyboard(KEYS key, KEYACTION action) override;
#ifdef WIN32
    virtual LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam) override;
    void DrawHorizonGrid(float size);
    void DrawAxis();
#endif
};

class Game : public GLApp
{

};