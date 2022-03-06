#pragma once
#include "framework.h"
#include <vector>
#include <utils/stdhelpers.h>
#include <chrono>
#include "Script.h"
enum KEYS
{
	KEY_SHIFT			=0x10,//	SHIFT key
	KEY_CONTROL			=0x11,//	CTRL key
	KEY_MENU			=0x12,//	ALT key
	KEY_PAUSE			=0x13,//	PAUSE key
	KEY_CAPITAL			=0x14,//	CAPS LOCK key
	KEY_KANA			=0x15,//	IME Kana mode
	KEY_HANGUEL			=0x15,//	IME Hanguel mode(maintained for compatibility; use VK_HANGUL)
	KEY_HANGUL			=0x15,//	IME Hangul mode
	KEY_IME_ON			=0x16,//	IME On
	KEY_JUNJA			=0x17,//	IME Junja mode
	KEY_FINAL			=0x18,//	IME final mode
	KEY_HANJA			=0x19,//	IME Hanja mode
	KEY_KANJI			=0x19,//	IME Kanji mode
	KEY_IME_OFF			=0x1A,//	IME Off
	KEY_ESCAPE			=0x1B,//	ESC key
	KEY_CONVERT			=0x1C,//	IME convert
	KEY_NONCONVERT		=0x1D,//	IME nonconvert
	KEY_ACCEPT			=0x1E,//	IME accept
	KEY_MODECHANGE		=0x1F,//	IME mode change request
	KEY_SPACE			=0x20,//	SPACEBAR
	KEY_PRIOR			=0x21,//	PAGE UP key
	KEY_NEXT			=0x22,//	PAGE DOWN key
	KEY_END				=0x23,//	END key
	KEY_HOME			=0x24,//	HOME key
	KEY_LEFT			=0x25,//	LEFT ARROW key
	KEY_UP				=0x26,//	UP ARROW key
	KEY_RIGHT			=0x27,//	RIGHT ARROW key
	KEY_DOWN			=0x28,//	DOWN ARROW key
	KEY_SELECT			=0x29,//	SELECT key
	KEY_PRINT			=0x2A,//	PRINT key
	KEY_EXECUTE_		=0x2B,//	EXECUTE key
	KEY_SNAPSHOT		=0x2C,//	PRINT SCREEN key
	KEY_INSERT			=0x2D,//	INS key
	KEY_DELETE			=0x2E,//	DEL key
	KEY_HELP			=0x2F,//	HELP key
	KEY_0 = 0x30,	//0 key
	KEY_1 = 0x31,	//1 key
	KEY_2 = 0x32,	//2 key
	KEY_3 = 0x33,	//3 key
	KEY_4 = 0x34,	//4 key
	KEY_5 = 0x35,	//5 key
	KEY_6 = 0x36,	//6 key
	KEY_7 = 0x37,	//7 key
	KEY_8 = 0x38,	//8 key
	KEY_9 = 0x39,	//9 key
	//0x3A - 40
	//Undefined
	KEY_A = 0x41,	//A key
	KEY_B = 0x42,	//B key
	KEY_C = 0x43,	//C key
	KEY_D = 0x44,	//D key
	KEY_E = 0x45,	//E key
	KEY_F = 0x46,	//F key
	KEY_G = 0x47,	//G key
	KEY_H = 0x48,	//H key
	KEY_I = 0x49,	//I key
	KEY_J = 0x4A,	//J key
	KEY_K = 0x4B,	//K key
	KEY_L = 0x4C,	//L key
	KEY_M = 0x4D,	//M key
	KEY_N = 0x4E,	//N key
	KEY_O = 0x4F,	//O key
	KEY_P = 0x50,	//P key
	KEY_Q = 0x51,	//Q key
	KEY_R = 0x52,	//R key
	KEY_S = 0x53,	//S key
	KEY_T = 0x54,	//T key
	KEY_U = 0x55,	//U key
	KEY_V = 0x56,	//V key
	KEY_W = 0x57,	//W key
	KEY_X = 0x58,	//X key
	KEY_Y = 0x59,	//Y key
	KEY_Z = 0x5A,	//Z key
};

enum KEYACTION
{
	KEYDOWN = 0,
	KEYUP = 1,
};

#define ASSETDIR(dir) const char* __asset_dir__ = dir
#define ASSETPATH(path) string_format("%s%s", __asset_dir__, path)

class IFrameStatics
{
public:
	virtual double GetFPS() = 0;
	virtual double GetDelta() = 0;
	virtual void FrameUpdate() = 0;
};

class Window: public IScriptable
{
	friend class Application;
#ifdef WIN32
private:
	friend LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	bool ready = false;
	LONG mouseX,mouseY;
	std::stringstream title;
	void UpdateTitle();
protected:
	HDC hdc;
	HGLRC hGLRC;
	virtual LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnPaint(HDC hdc);
public:
	HWND hWnd = NULL;
	LPCWSTR szTitle = TEXT("GLRT");
	virtual LPCWSTR GetWindowClassName();
	virtual void PopulateClassInfo(WNDCLASSEXW* pwcex);
#endif

	IFrameStatics* frameStatics;
protected:
	virtual void GetInitSize(long* width, long* height);
	virtual bool IsEnableVsync();
	virtual void OnDestroy();
	virtual void OnCreate();
	virtual void OnResize(long width, long height);
	virtual void OnIdle();
	virtual void RenderPipline();
	virtual void BeforeRender();
	virtual void Render();
	virtual void PostRender();
	virtual void OnGUI();
	virtual void AfterRender();
	virtual void OnMouseMove(long dx, long dy, long x, long y);
	virtual void OnMouseWheel(int delta);
	virtual void OnKeyboard(KEYS key, KEYACTION action);

public:
	Vector2 GetViewportSize();
	float GetClientAspect();
};
