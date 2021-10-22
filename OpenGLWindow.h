#pragma once
#include "framework.h"
//#include<gl/GL.h>
#include <glad/glad.h>
#include "Window.h"
class OpenGLWindow :
    public Window
{
    HGLRC hGLRC;
    virtual void PopulateClassInfo(WNDCLASSEXW* pwcex) override;
    virtual void OnCreate() override;
};

