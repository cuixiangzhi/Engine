#pragma once
#include "../Base/BaseDef.h"

namespace Engine
{
	class Bitmap;
	class Window
	{
	public:
		int mWidth;
		int mHeight;
		HWND mWindow;
		Bitmap* mBitmap;
		MSG mMsg;
	public:
		Window();
		~Window();
	public:
		static LRESULT WndProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam);
		static Window* Create(const char* windowName, int width, int height);
	public:
		int GetWidth() const { return mWidth; }
		int GetHeight() const { return mHeight; }
		void* GetBitmapBuffer();
		void SwapBuffer();
		void SetCursor(int x, int y);
		void SetActive();
		void Update();
		bool IsClosed();
	};
}