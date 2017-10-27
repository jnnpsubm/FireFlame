#pragma once
#include <Windows.h>
#include <memory>
#include <string>
#include <assert.h>
#include "../Timer/FLStopWatch.h"

namespace FireFlame {
class Renderer;
class Window {
public:
	Window(HINSTANCE hInst, std::shared_ptr<Renderer> renderer);
	~Window();

	static Window* GetWindow()     { return theWindow; }
	HINSTANCE AppInst() const      { return mhInst; }
	HWND      MainWnd() const      { return mhMainWnd; }
	float     AspectRatio() const  { return (float)mClientWidth / mClientHeight; }
	int       ClientWidth() const  { return mClientWidth; }
	int       ClientHeight() const { return mClientHeight; }

	int InitMainWindow(int x, int y, int w, int h);
	int Run();

	LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	void CalculateFrameStats();

private:
	static Window* theWindow;
	std::shared_ptr<Renderer> mRenderer;

	// Used to keep track of the delta-time,and game time.
	StopWatch mTimer;

	HINSTANCE    mhInst    = nullptr;
	HWND         mhMainWnd = nullptr;
	std::wstring mMainWndCaption;
	bool         mAppPaused = false;  // is the application paused?
	bool         mMinimized = false;  // is the application minimized?
	bool         mMaximized = false;  // is the application maximized?
	bool         mResizing = false;   // are the resize bars being dragged?

	int mClientWidth  = 800;
	int mClientHeight = 600;
};
}