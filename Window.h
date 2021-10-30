#pragma once
#include "framework.h"
#include <vector>


enum KEYS
{
	KEY_0 = 0x30,
	//0 key
	KEY_1 = 0x31,
	//1 key
	KEY_2 = 0x32,
	//2 key
	KEY_3 = 0x33,
	//3 key
	KEY_4 = 0x34,
	//4 key
	KEY_5 = 0x35,
	//5 key
	KEY_6 = 0x36,
	//6 key
	KEY_7 = 0x37,
	//7 key
	KEY_8 = 0x38,
	//8 key
	KEY_9 = 0x39,
	//9 key
	//0x3A - 40
	//Undefined
	KEY_A = 0x41,
	//A key
	KEY_B = 0x42,
	//B key
	KEY_C = 0x43,
	//C key
	KEY_D = 0x44,
	//D key
	KEY_E = 0x45,
	//E key
	KEY_F = 0x46,
	//F key
	KEY_G = 0x47,
	//G key
	KEY_H = 0x48,
	//H key
	KEY_I = 0x49,
	//I key
	KEY_J = 0x4A,
	//J key
	KEY_K = 0x4B,
	//K key
	KEY_L = 0x4C,
	//L key
	KEY_M = 0x4D,
	//M key
	KEY_N = 0x4E,
	//N key
	KEY_O = 0x4F,
	//O key
	KEY_P = 0x50,
	//P key
	KEY_Q = 0x51,
	//Q key
	KEY_R = 0x52,
	//R key
	KEY_S = 0x53,
	//S key
	KEY_T = 0x54,
	//T key
	KEY_U = 0x55,
	//U key
	KEY_V = 0x56,
	//V key
	KEY_W = 0x57,
	//W key
	KEY_X = 0x58,
	//X key
	KEY_Y = 0x59,
	//Y key
	KEY_Z = 0x5A,
	//Z key
};

enum KEYACTION
{
	KEYDOWN = 0,
	KEYUP = 1,
};

class Window
{
	friend class Application;
	friend LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	bool ready = false;
	LONG mouseX,mouseY;
protected:
	HDC hdc;
	HGLRC hGLRC;
	
	virtual LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnPaint(HDC hdc);
	virtual void OnDestroy();
	virtual void OnCreate();
	virtual void OnResize(int width, int height);
	virtual void OnIdle();
	virtual void Render();
	virtual void OnMouse(long dx, long dy, long x, long y);
	virtual void OnKeyboard(KEYS key, KEYACTION action);
public:
	virtual LPCWSTR GetWindowClassName();
	virtual void PopulateClassInfo(WNDCLASSEXW* pwcex);
	HWND hWnd = NULL;
	LPCWSTR szTitle = TEXT("GLRT");
};
