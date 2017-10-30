#pragma once
#include <Windows.h>
#include <memory>
#include <string>
#include <assert.h>
#include "../Timer/FLStopWatch.h"

namespace FireFlame {
class Engine;
class Window {
public:
	Window(HINSTANCE hInst, Engine* engine);
	~Window();

	static Window* GetWindow()     { return theWindow; }
	HINSTANCE AppInst() const      { return mhInst; }
	HWND      MainWnd() const      { return mhMainWnd; }
	float     AspectRatio() const  { return (float)mClientWidth / mClientHeight; }
	int       ClientWidth() const  { return mClientWidth; }
	int       ClientHeight() const { return mClientHeight; }

	int InitMainWindow(int x, int y, int w, int h);

	int HandleMsg();
	int Quitted() const { return msg.message == WM_QUIT; }
	int QuitCode() const { return (int)msg.wParam; }

	void SetWindowCaption(const std::wstring& caption);
	void AppendWindowCaption(const std::wstring& appendStr);
	LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	static Window* theWindow;
	Engine*        mEngine;
	
	MSG          msg = { 0 };
	std::wstring mCaption;
	HINSTANCE    mhInst      = nullptr;
	HWND         mhMainWnd   = nullptr;
	bool         mMinimized  = false;  // is the application minimized?
	bool         mMaximized  = false;  // is the application maximized?
	bool         mResizing   = false;  // are the resize bars being dragged?
	bool         mFullScreen = false;

	int          mClientWidth  = 800;
	int          mClientHeight = 600;

	// Message processing
	LRESULT OnKeyUp(WPARAM wParam, LPARAM lParam);
	LRESULT OnActive(UINT mode);
	LRESULT OnDestroy();
	LRESULT OnSize(WPARAM wParam, LPARAM lParam);
	LRESULT OnEnterSizeMove();
	LRESULT OnExitSizeMove();
	LRESULT OnGetMinMaxInfo(MINMAXINFO* pInfo);
	LRESULT OnSysKeyDown(WPARAM wParam, LPARAM lParam);

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);
};
}