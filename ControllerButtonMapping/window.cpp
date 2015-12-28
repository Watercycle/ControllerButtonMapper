#include "stdafx.h"
#include "window.h"
#include <windows.h>
#include <Psapi.h>

Window::Window(unordered_set<wstring> ignoreFullscreenPrograms) 
	: isFullscreen(false), running(true), ignoredFullscreenPrograms(ignoredFullscreenPrograms)
{
}

Window::~Window()
{
	running = false;
	poller.join();
}

void Window::beginPolling(unsigned pollFrequency)
{
	poller = thread([&] {
		while (running)
		{
			this_thread::sleep_for(milliseconds(pollFrequency));
			wstring topWindowExeName = GetTopWindowExecutableName();
			if (ignoredFullscreenPrograms.find(topWindowExeName) == ignoredFullscreenPrograms.end())
			{
				isFullscreen = TopWindowIsFullscreen();
			}
		}
	});
}

bool Window::topWindowIsFullscreen() const
{
	return isFullscreen;
}


bool Window::TopWindowIsFullscreen()
{
	const auto fgWindow = GetForegroundWindow();
	if (IsFullScreen(fgWindow))
	{
		return true;
	}

	return false;
}

wstring Window::GetTopWindowExecutableName()
{
	DWORD topWindowProcesessrId;
	GetWindowThreadProcessId(GetForegroundWindow(), &topWindowProcesessrId);

	wchar_t filename[MAX_PATH] = L"<unknown>";
	HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, topWindowProcesessrId);
	if (processHandle != nullptr) 
	{
		if (GetModuleBaseName(processHandle, nullptr, filename, MAX_PATH) == 0) 
		{
			cerr << "Failed to get module filename." << endl;
		}

		CloseHandle(processHandle);
	}
	else 
	{
		cerr << "Failed to open process." << endl;
	}

	return wstring(filename);
}

wstring Window::GetTopWindowTitle()
{
	wchar_t wnd_title[256];
	HWND hwnd = GetForegroundWindow(); // get handle of currently active window
	GetWindowText(hwnd, wnd_title, sizeof(wnd_title));
	wstring ws(wnd_title);
	return ws;
}

bool Window::IsFullScreen(HWND hwnd)
{
	RECT window;
	GetWindowRect(hwnd, &window);

	const int fullscreenWidth = GetSystemMetrics(SM_CXSCREEN);
	const int fullscreenHeight = GetSystemMetrics(SM_CYSCREEN);

	return window.right - window.left == fullscreenWidth && window.bottom - window.top == fullscreenHeight;
}
