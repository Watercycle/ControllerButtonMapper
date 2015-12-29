#pragma once

struct HWND__;
typedef HWND__* HWND;

namespace Window
{
	bool IsFullScreen(HWND hwnd);
	wstring GetTopWindowTitle();
	wstring GetTopWindowExecutableName();
	bool TopWindowIsFullscreen();
};