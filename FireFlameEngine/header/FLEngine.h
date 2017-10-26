#pragma once
#include <memory>
#include <Windows.h>
#include <functional>
#include "..\src\Exception\FLException.h"

namespace FireFlame{
class Window;
class Renderer;
class Engine {
public:
	Engine(HINSTANCE hinst);
	static Engine* GetEngine() { return theEngine; }
	~Engine();

	void RegisterRendererUpdateFunc(std::function<void(float)> func);
	void RegisterRendererDrawFunc(std::function<void(float)> func);

	int InitMainWindow(int x, int y, int w, int h);
	int Run();

private:
	static Engine* theEngine;

	std::shared_ptr<Window>   MainWnd  = nullptr;
	std::shared_ptr<Renderer> renderer = nullptr;
};
}