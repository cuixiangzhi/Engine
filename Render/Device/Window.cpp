#include "Window.h"
#include "../Logic.h"

namespace Engine
{
	class Bitmap
	{
	public:
		HDC mWindowHDC;
		HDC mDeviceHDC;
		HBITMAP mDeviceBM;
		HGDIOBJ mLastBMOBJ;
		LPVOID mBitmapBuffer;
	};

	Window::Window() 
	{ 
		mWidth = 0;
		mHeight = 0;
		mWindow = NULL; 
		mBitmap = NULL;
	}

	Window::~Window() 
	{ 
		if (mBitmap != NULL)
		{
			if (mBitmap->mDeviceBM != NULL) DeleteObject(mBitmap->mDeviceBM);
			if (mBitmap->mWindowHDC != NULL) ReleaseDC(mWindow, mBitmap->mWindowHDC);
			if (mBitmap->mDeviceHDC != NULL)
			{
				if (mBitmap->mLastBMOBJ != NULL) SelectObject(mBitmap->mDeviceHDC, mBitmap->mLastBMOBJ);
				DeleteDC(mBitmap->mDeviceHDC);
			}
			delete mBitmap;
			mBitmap = NULL;
		}
		if (mWindow != NULL)
		{
			CloseWindow(mWindow);
			mWindow = NULL;
		}
	}

	LRESULT Window::WndProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		if (msg == WM_KEYDOWN && Logic::ProcessEvent(wparam)) return 0;
		return DefWindowProc(wnd, msg, wparam, lparam);
	}

	Window* Window::Create(const char* windowName, int width, int height)
	{
		Window* window = new Window();
		window->mWidth = width;
		window->mHeight = height;
		window->mBitmap = new Bitmap();
		//窗口类注册
		WNDCLASS wc;
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = (WNDPROC)WndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = GetModuleHandle(NULL);
		wc.hIcon = NULL;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = NULL;
		wc.lpszMenuName = NULL;
		wc.lpszClassName = "ENGINE";
		if (!RegisterClass(&wc)) goto CREATE_FAILED;
		//窗口创建
		window->mWindow = CreateWindow("ENGINE", windowName, WS_TILEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL, wc.hInstance, NULL);
		if (!window->mWindow) goto CREATE_FAILED;
		//位图创建
		Bitmap* bitmap = window->mBitmap;
		//
		bitmap->mWindowHDC = GetDC(window->mWindow);
		bitmap->mDeviceHDC = CreateCompatibleDC(bitmap->mWindowHDC);
		//位图创建
		BITMAPINFO bmInfo;
		BITMAPINFOHEADER& bmHeader = bmInfo.bmiHeader;
		bmHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmHeader.biWidth = window->mWidth;
		bmHeader.biHeight = window->mHeight;
		bmHeader.biPlanes = 1;
		bmHeader.biBitCount = 32;
		bmHeader.biCompression = BI_RGB;
		bmHeader.biSizeImage = window->mWidth * window->mHeight * 4;
		bmHeader.biXPelsPerMeter = 0;
		bmHeader.biYPelsPerMeter = 0;
		bmHeader.biClrUsed = 0;
		bmHeader.biClrImportant = 0;
		bitmap->mDeviceBM = CreateDIBSection(bitmap->mDeviceHDC, &bmInfo, DIB_RGB_COLORS, &bitmap->mBitmapBuffer, 0, 0);
		if (!bitmap->mDeviceBM || !bitmap->mBitmapBuffer) goto CREATE_FAILED;
		//位图选入设备
		bitmap->mLastBMOBJ = SelectObject(bitmap->mDeviceHDC, bitmap->mDeviceBM);
		//创建成功
		return window;
	CREATE_FAILED:
		delete window;
		return NULL;
	}

	void Window::SetActive()
	{
		RECT rect = { 0, 0, mWidth, mHeight };
		AdjustWindowRect(&rect, GetWindowLong(mWindow, GWL_STYLE), 0);
		int windoww = rect.right - rect.left;
		int windowh = rect.bottom - rect.top;
		int screenw = GetSystemMetrics(SM_CXSCREEN);
		int screenh = GetSystemMetrics(SM_CYSCREEN);
		int centerx = (screenw - windoww) / 2;
		int centery = (screenh - windowh) / 2;
		SetWindowPos(mWindow, NULL, centerx, centery, windoww, windowh, SWP_NOCOPYBITS | SWP_NOZORDER | SWP_SHOWWINDOW);
		ShowWindow(mWindow, SW_NORMAL);
		UpdateWindow(mWindow);
	}

	void Window::SetCursor(int x, int y)
	{
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { x, y });
	}

	void Window::SwapBuffer()
	{
		BitBlt(mBitmap->mWindowHDC, 0, 0, mWidth, mHeight, mBitmap->mDeviceHDC, 0, 0, SRCCOPY);
	}

	void* Window::GetBitmapBuffer()
	{
		return mBitmap->mBitmapBuffer;
	}

	void Window::Update()
	{
		if (PeekMessage(&mMsg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&mMsg);
			DispatchMessage(&mMsg);
		}
	}

	bool Window::IsClosed()
	{
		return mMsg.message == WM_QUIT;
	}
}