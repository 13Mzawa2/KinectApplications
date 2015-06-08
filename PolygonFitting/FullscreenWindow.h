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
		// windowName�����E�B���h�E������
		HWND windowHandle = ::FindWindowA(NULL, windowName);

		if (NULL != windowHandle) {

			// �E�B���h�E�X�^�C���ύX�i���j���[�o�[�Ȃ��A�őO�ʁj
			SetWindowLongPtr(windowHandle, GWL_STYLE, WS_POPUP);
			SetWindowLongPtr(windowHandle, GWL_EXSTYLE, WS_EX_TOPMOST);

			// �ő剻����
			ShowWindow(windowHandle, SW_MAXIMIZE);
			cvSetWindowProperty(windowName, CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);

			// �f�B�X�v���C�T�C�Y���擾
			int mainDisplayWidth = GetSystemMetrics(SM_CXSCREEN);
			int mainDisplayHeight = GetSystemMetrics(SM_CYSCREEN);

			// �N���C�A���g�̈���f�B�X�v���[�ɍ��킹��
			SetWindowPos(windowHandle, NULL,
				0, 0, mainDisplayWidth, mainDisplayWidth,
				SWP_FRAMECHANGED | SWP_NOZORDER);
		}
	}
};

