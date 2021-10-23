#include "framework.h"
#include "Resource.h"
#include "Application.h"
#include "Window.h"

// �����ڡ������Ϣ�������
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	Window* pWindow = (Window*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	if (pWindow == nullptr)
		return DefWindowProc(hWnd, message, wParam, lParam);
	return pWindow->WndProc(message, wParam, lParam);
}


LRESULT Window::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// �����˵�ѡ��:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(Application::Instance().hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_CREATE:
		OnCreate();
		break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: �ڴ˴����ʹ�� hdc ���κλ�ͼ����...
		OnPaint(hdc);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		OnDestroy();
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void Window::OnPaint(HDC hdc)
{
}

void Window::OnDestroy()
{
}

void Window::OnCreate()
{
}

LPCWSTR Window::GetWindowClassName()
{
	return TEXT("Window");
}

void Window::PopulateClassInfo(WNDCLASSEXW* pwcex)
{
	auto hInstance = Application::Instance().hInstance;
	pwcex->cbSize = sizeof(WNDCLASSEX);
	pwcex->style = CS_HREDRAW | CS_VREDRAW;
	pwcex->lpfnWndProc = ::WndProc;
	pwcex->cbClsExtra = 0;
	pwcex->cbWndExtra = 0;
	pwcex->hInstance = hInstance;
	pwcex->hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GLRT));
	pwcex->hCursor = LoadCursor(nullptr, IDC_ARROW);
	pwcex->hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	pwcex->lpszMenuName = MAKEINTRESOURCEW(IDC_GLRT);
	pwcex->lpszClassName = GetWindowClassName();
	pwcex->hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SMALL));
}
