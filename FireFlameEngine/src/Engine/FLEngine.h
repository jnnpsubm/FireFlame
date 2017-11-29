#pragma once
#include <memory>
#include <Windows.h>
#include <functional>
#include "..\Exception\FLException.h"
#include "..\FLTypeDefs.h"
#include "..\Timer\FLStopWatch.h"
#include "..\Scene\FLScene.h"
#include "..\Window\FLWindow.h"
#include "..\Renderer\FLRenderer.h"

// Link necessary d3d12 libraries.
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

namespace FireFlame{
class D3DRenderer;
class D3DPSOManager;
class Engine {
public:
	Engine(HINSTANCE hinst);
	static Engine* GetEngine() { return theEngine; }
	~Engine();

	std::shared_ptr<Scene>         GetScene()      const { return mScene; }
    std::shared_ptr<Window>        GetWindow()     const { return mMainWnd; }
    std::shared_ptr<D3DPSOManager> GetPSOManager() const { return mPSOManager; }
    std::shared_ptr<D3DRenderer>   GetRenderer()   const { return mRenderer; }

    float TotalTime() const { return mTimer.TotalTime(); }
    float DeltaTime() const { return mTimer.DeltaTime(); }

    // Get Methods
    Cull_Mode GetCullMode();
    Fill_Mode GetFillMode();

    static int NumFrameResources() { return mNumFrameResources; }

    // Set methods
    void SetDefaultClearColor(const float(&color)[4]);
    void SetCullMode(Cull_Mode mode);
    void SetFillMode(Fill_Mode mode);
    void SetViewport(const stViewport& viewport);
    void SetScissorRect(int left, int top, int right, int bottom);
    void SetControllerInputMode(ControllerInputMode mode) { mControllerInputMode = mode; }

    // register callbacks
	void RegisterUpdateFunc(std::function<void(float)> func);
	void RegisterRendererDrawFunc(std::function<void(float)> func);
    void RegisterWindowResizedHandler(std::function<void(int w, int h)> func) {
        mFuncWindowResizedHandler = func;
    }

	int InitMainWindow(int x, int y, int w, int h);
	int InitRenderer(API_Feature api);
	int Run();

	void Pause() { mAppPaused = true; mTimer.Pause(); }
	void Resume() { mAppPaused = false; mTimer.Resume(); }
	void Stop();

	LRESULT OnWindowDestroy();
	LRESULT OnWindowResized();
	LRESULT OnWindowKeyUp(WPARAM wParam, LPARAM lParam);

	// system probe
	int LogVideoAdapters(std::wostream& os);

private:
	static Engine*            theEngine;
    ControllerInputMode       mControllerInputMode = ControllerInputMode::Feed;
	StopWatch                 mTimer;
	bool                      mAppPaused = false;  // is the application paused?

    static const int          mNumFrameResources = 3;

	std::shared_ptr<Window>        mMainWnd    = nullptr;
	std::shared_ptr<D3DRenderer>   mRenderer   = nullptr;
	std::shared_ptr<Scene>         mScene      = nullptr;
    std::shared_ptr<D3DPSOManager> mPSOManager = nullptr;

    // callbacks
    std::function<void(int w, int h)> mFuncWindowResizedHandler = [](int w, int h) {};

	void CalculateFrameStats();
};
}