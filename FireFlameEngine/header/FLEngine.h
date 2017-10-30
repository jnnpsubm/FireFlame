#pragma once
#include <memory>
#include <Windows.h>
#include <functional>
#include "../src/Exception/FLException.h"
#include "../src/FLTypeDefs.h"
#include "../src/Timer/FLStopWatch.h"

// Link necessary d3d12 libraries.
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

namespace FireFlame{
class Window;
class Renderer;
class Scene;
class Engine {
public:
	Engine(HINSTANCE hinst);
	static Engine* GetEngine() { return theEngine; }
	~Engine();

	void RegisterRendererUpdateFunc(std::function<void(float)> func);
	void RegisterRendererDrawFunc(std::function<void(float)> func);

	int InitMainWindow(int x, int y, int w, int h);
	int InitRenderer(API_Feature api);
	int Run();

	void Pause() { mAppPaused = true; mTimer.Pause(); }
	void Resume() { mAppPaused = false; mTimer.Resume(); }
	void Stop();

	LRESULT OnWindowDestroy();
	LRESULT OnWindowResized();

	// system probe
	int LogVideoAdapters(std::wostream& os);

private:
	static Engine* theEngine;
	StopWatch                 mTimer;
	bool                      mAppPaused = false;  // is the application paused?

	std::shared_ptr<Window>   mMainWnd  = nullptr;
	std::shared_ptr<Renderer> mRenderer = nullptr;
	std::shared_ptr<Scene>    mScene    = nullptr;

	void CalculateFrameStats();
};
}