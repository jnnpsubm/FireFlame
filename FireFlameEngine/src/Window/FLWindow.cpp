#include <WindowsX.h>
#include <string>
#include "FLWindow.h"
#include "..\Engine\FLEngine.h"

namespace FireFlame {
Window* Window::theWindow = nullptr;
Window::~Window() {
	
}
Window::Window(HINSTANCE hInst, Engine& engine) 
	: mhInst(hInst), mEngine(engine) {
	assert(theWindow == nullptr);
	theWindow = this;
}
int Window::HandleMsg() {
	if (::PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		return TRUE;
	}
	return FALSE;
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
	case WM_SYSKEYDOWN:
		OnSysKeyDown(wParam, lParam);
		break;
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
void Window::SetWindowCaption(const std::wstring& caption) {
	SetWindowText(mhMainWnd, caption.c_str());
}
void Window::AppendWindowCaption(const std::wstring& appendStr) {
	std::wstring windowText = mCaption + appendStr;
	SetWindowText(mhMainWnd, windowText.c_str());
}
// ==============================Message Processing==============================
LRESULT Window::OnActive(UINT mode) {
	if (mode == WA_INACTIVE){
		mEngine.Pause();
	}else{
		mEngine.Resume();
	}
	return 0;
}
LRESULT Window::OnDestroy() {
	mEngine.OnWindowDestroy();
	PostQuitMessage(0);
	return 0;
}
LRESULT Window::OnSize(WPARAM wParam, LPARAM lParam) {
	// Save the new client area dimensions.
	mClientWidth = LOWORD(lParam);
	mClientHeight = HIWORD(lParam);
	if (wParam == SIZE_MINIMIZED) {
		mEngine.Pause();
		mMinimized = true;
		mMaximized = false;
	}
	else if (wParam == SIZE_MAXIMIZED) {
		mMinimized = false;
		mMaximized = true;
		mEngine.OnWindowResized();
	}
	else if (wParam == SIZE_RESTORED) {
		if (mMinimized) {        // Restoring from minimized state?
			mMinimized = false;
			mEngine.OnWindowResized();
		}
		else if (mMaximized) {  // Restoring from maximized state?
			mMaximized = false;
			mEngine.OnWindowResized();
		}
		else if (mResizing) {
			// If user is dragging the resize bars, we do not resize 
			// the buffers here because as the user continuously 
			// drags the resize bars, a stream of WM_SIZE messages are
			// sent to the window, and it would be pointless (and slow)
			// to resize for each WM_SIZE message received from dragging
			// the resize bars.  So instead, we reset after the user is 
			// done resizing the window and releases the resize bars, which 
			// sends a WM_EXITSIZEMOVE message.
		}
		else { // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
			mEngine.OnWindowResized();
		}
	}
	return 0;
}
LRESULT Window::OnEnterSizeMove() {
	mResizing = true;
	mEngine.Pause();
	return 0;
}
LRESULT Window::OnExitSizeMove() {
	mResizing = false;
	mEngine.OnWindowResized();
	return 0;
}
LRESULT Window::OnGetMinMaxInfo(MINMAXINFO* pInfo) {
	pInfo->ptMinTrackSize.x = 200;
	pInfo->ptMinTrackSize.y = 200;
	return 0;
}
LRESULT Window::OnKeyUp(WPARAM wParam, LPARAM lParam) {
	if (wParam == VK_ESCAPE) {
		mEngine.Stop();
		PostQuitMessage(0);
	}
	return mEngine.OnWindowKeyUp(wParam, lParam);
}
LRESULT Window::OnSysKeyDown(WPARAM wParam, LPARAM lParam) {
	if (wParam == VK_RETURN && (lParam & 0x60000000) == 0x20000000){
		// Implements the classic ALT+ENTER fullscreen toggle
		if (mFullScreen){
			SetWindowLongPtr(mhMainWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
			SetWindowLongPtr(mhMainWnd, GWL_EXSTYLE, 0);
			ShowWindow(mhMainWnd, SW_SHOWNORMAL);
			SetWindowPos(mhMainWnd, HWND_TOP, 0, 0, mClientWidth, mClientHeight, 
				         SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
		}else{
			SetWindowLongPtr(mhMainWnd, GWL_STYLE, 0);
			SetWindowLongPtr(mhMainWnd, GWL_EXSTYLE, WS_EX_TOPMOST);
			SetWindowPos(mhMainWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
			ShowWindow(mhMainWnd, SW_SHOWMAXIMIZED);
		}
		mFullScreen = !mFullScreen;
	}
	return 0;
}
void Window::OnMouseDown(WPARAM btnState, int x, int y) {
    mMouseDownCB(btnState, x, y);
}
void Window::OnMouseUp(WPARAM btnState, int x, int y) {
    mMouseUpCB(btnState, x, y);
}
void Window::OnMouseMove(WPARAM btnState, int x, int y) {
    mMouseMoveCB(btnState, x, y);
}
} // end namespace

