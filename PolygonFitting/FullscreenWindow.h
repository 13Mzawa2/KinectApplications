#pragma once
#include <Windows.h>
#include "OpenCVAdapter.hpp"

class FullscreenWindow
{
public:

	FullscreenWindow()
	{
	}

	~FullscreenWindow()
	{
	}

	void setFullscreen(char *windowName)
	{
		// windowNameを持つウィンドウを検索
		HWND windowHandle = ::FindWindowA(NULL, windowName);

		if (NULL != windowHandle) {

			// ウィンドウスタイル変更（メニューバーなし、最前面）
			SetWindowLongPtr(windowHandle, GWL_STYLE, WS_POPUP);
			SetWindowLongPtr(windowHandle, GWL_EXSTYLE, WS_EX_TOPMOST);

			// 最大化する
			ShowWindow(windowHandle, SW_MAXIMIZE);
			cvSetWindowProperty(windowName, CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);

			// ディスプレイサイズを取得
			int mainDisplayWidth = GetSystemMetrics(SM_CXSCREEN);
			int mainDisplayHeight = GetSystemMetrics(SM_CYSCREEN);

			// クライアント領域をディスプレーに合わせる
			SetWindowPos(windowHandle, NULL,
				0, 0, mainDisplayWidth, mainDisplayWidth,
				SWP_FRAMECHANGED | SWP_NOZORDER);
		}
	}
};

