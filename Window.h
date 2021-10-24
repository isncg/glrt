#pragma once
#include "framework.h"
class Window
{
	friend class Application;
	friend LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
protected:
	HDC hdc;
	HGLRC hGLRC;
	
	bool ready = false;
	virtual LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnPaint(HDC hdc);
	virtual void OnDestroy();
	virtual void OnCreate();
	virtual void OnIdle();
	virtual void Render();
public:
	virtual LPCWSTR GetWindowClassName();
	virtual void PopulateClassInfo(WNDCLASSEXW* pwcex);
	HWND hWnd = NULL;
	LPCWSTR szTitle = TEXT("GLRT");
};
