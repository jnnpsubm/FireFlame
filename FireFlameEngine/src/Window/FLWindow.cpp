#include <WindowsX.h>
#include "FLWindow.h"
#include "../Renderer/FLRenderer.h"
#include <string>

namespace FireFlame {
Window* Window::theWindow = nullptr;
Window::~Window() {
	
}
Window::Window(HINSTANCE hInst, std::shared_ptr<Renderer> renderer) 
	: mhInst(hInst), mRenderer(renderer) {
	assert(theWindow == nullptr);
	theWindow = this;
}
int Window::Run() {
	mTimer.Reset();
	MSG msg = { 0 };
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}else {
			mTimer.Mark();
			if (!mAppPaused){
				CalculateFrameStats();
				mRenderer->Update(mTimer);
				mRenderer->Draw(mTimer);
			}else{
				Sleep(10);
			}
		}
	}
	return (int)msg.wParam;
}
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	// Forward hwnd on because we can get messages (e.g., WM_CREATE)
	// before CreateWindow returns, and thus before mhMainWnd is valid.
	return Window::GetWindow()->MsgProc(hwnd, msg, wParam, lParam);
}
LRESULT Window::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg){
	case WM_ACTIVATE:{
		OnActive(LOWORD(wParam));
	}break;
	case WM_DESTROY:{
		OnDestroy();
	}break;
	case WM_SIZE: {
		OnSize(wParam, lParam);
	}break;
	case WM_ENTERSIZEMOVE: {  // WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
		OnEnterSizeMove();
	}break;
	case WM_EXITSIZEMOVE: {   // WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		OnExitSizeMove();
	}break;
	case WM_KEYUP: {
		OnKeyUp(wParam, lParam);
	}break;
	case WM_GETMINMAXINFO: {
		OnGetMinMaxInfo((MINMAXINFO*)lParam);
	}break;
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	// The WM_MENUCHAR message is sent when a menu is active and the user presses 
	// a key that does not correspond to any mnemonic or accelerator key. 
	case WM_MENUCHAR:
		// Don't beep when we alt-enter.
		return MAKELRESULT(0, MNC_CLOSE);
	}
	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}
int Window::InitMainWindow(int x, int y, int w, int h) {
	mClientWidth = w;
	mClientHeight = h;

	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = mhInst;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"MainWnd";

	if (!RegisterClass(&wc)) {
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}

	// Compute window rectangle dimensions based on requested client area dimensions.
	RECT R = { 0, 0, mClientWidth, mClientHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	mhMainWnd = CreateWindow(L"MainWnd", mCaption.c_str(),
		WS_OVERLAPPEDWINDOW, x, y, width, height, 0, 0, mhInst, 0);
	if (!mhMainWnd) {
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	ShowWindow(mhMainWnd, SW_SHOW);
	UpdateWindow(mhMainWnd);
	return 0;
}
void Window::CalculateFrameStats()
{
	// Code computes the average frames per second, and also the 
	// average time it takes to render one frame.  These stats 
	// are appended to the window caption bar.
	static int   frameCnt = 0;
	static float timeElapsed = 0.0f;
	frameCnt++;

	// Compute averages over one second period.
	if ((mTimer.TotalTime() - timeElapsed) >= 1.0f){
		float fps = (float)frameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;

		std::wstring fpsStr = std::to_wstring(fps);
		std::wstring mspfStr = std::to_wstring(mspf);

		std::wstring windowText = mCaption +
			L"    fps: " + fpsStr +
			L"   mspf: " + mspfStr;
		SetWindowText(mhMainWnd, windowText.c_str());

		// Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}
// ==============================Message Processing==============================
LRESULT Window::OnActive(UINT mode) {
	if (WA_INACTIVE == mode) {
		mAppPaused = true;
		mTimer.Pause();
	}
	else {
		mAppPaused = false;
		mTimer.Resume();
	}
	return 0;
}
LRESULT Window::OnDestroy() {
	mAppPaused = true;
	mRenderer->FlushCommandQueue();
	PostQuitMessage(0);
	return 0;
}
LRESULT Window::OnSize(WPARAM wParam, LPARAM lParam) {
	// Save the new client area dimensions.
	mClientWidth = LOWORD(lParam);
	mClientHeight = HIWORD(lParam);
	if (mRenderer->Ready()){
		if (wParam == SIZE_MINIMIZED){
			mAppPaused = true;
			mMinimized = true;
			mMaximized = false;
		}else if (wParam == SIZE_MAXIMIZED){
			mAppPaused = false;
			mMinimized = false;
			mMaximized = true;
			mRenderer->Resize();
		}else if (wParam == SIZE_RESTORED){
			if (mMinimized){        // Restoring from minimized state?
				mAppPaused = false;
				mMinimized = false;
				mRenderer->Resize();
			}else if (mMaximized){  // Restoring from maximized state?
				mAppPaused = false;
				mMaximized = false;
				mRenderer->Resize();
			}else if (mResizing){
				// If user is dragging the resize bars, we do not resize 
				// the buffers here because as the user continuously 
				// drags the resize bars, a stream of WM_SIZE messages are
				// sent to the window, and it would be pointless (and slow)
				// to resize for each WM_SIZE message received from dragging
				// the resize bars.  So instead, we reset after the user is 
				// done resizing the window and releases the resize bars, which 
				// sends a WM_EXITSIZEMOVE message.
			}else{ // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
				mRenderer->Resize();
			}
		}
	}
	return 0;
}
LRESULT Window::OnEnterSizeMove() {
	mAppPaused = true;
	mResizing = true;
	mTimer.Pause();
	return 0;
}
LRESULT Window::OnExitSizeMove() {
	mAppPaused = false;
	mResizing = false;
	mTimer.Resume();
	mRenderer->Resize();
	return 0;
}
LRESULT Window::OnGetMinMaxInfo(MINMAXINFO* pInfo) {
	pInfo->ptMinTrackSize.x = 200;
	pInfo->ptMinTrackSize.y = 200;
	return 0;
}
LRESULT Window::OnKeyUp(WPARAM wParam, LPARAM lParam) {
	if (wParam == VK_ESCAPE) {
		PostQuitMessage(0);
	}
	return 0;
}
void Window::OnMouseDown(WPARAM btnState, int x, int y) {

}
void Window::OnMouseUp(WPARAM btnState, int x, int y) {

}
void Window::OnMouseMove(WPARAM btnState, int x, int y) {

}
} // end namespace

