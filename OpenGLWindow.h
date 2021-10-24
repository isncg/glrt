#pragma once
#include "framework.h"
//#include<gl/GL.h>
#include <glad/glad.h>
#include "Window.h"
#include "Renderer.h"
class OpenGLWindow :
    public Window
{
    HDC hdc;
    HGLRC hGLRC;

    Shader shader;
    Mesh mesh;
    MeshRenderer renderer;
    bool ready = false;

    virtual void PopulateClassInfo(WNDCLASSEXW* pwcex) override;
    virtual void OnCreate() override;
    virtual void OnDestroy() override;
    virtual void OnIdle() override;
    virtual void Render();
};

