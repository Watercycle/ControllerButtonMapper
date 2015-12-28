#pragma once
#include <unordered_set>

struct HWND__;
typedef HWND__* HWND;

class Window
{
private:
	unordered_set<wstring> ignoredFullscreenPrograms;
	abool isFullscreen = false;
	abool running = true;
	thread poller;

public:
	Window(unordered_set<wstring> ignoreFullscreenPrograms);
	~Window();

	void beginPolling(unsigned pollFrequency);
	bool topWindowIsFullscreen() const;

	static bool IsFullScreen(HWND hwnd);

	static wstring GetTopWindowTitle();
	static wstring GetTopWindowExecutableName();
	static bool TopWindowIsFullscreen();
};