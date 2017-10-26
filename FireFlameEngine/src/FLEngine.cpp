#include "../header/FLEngine.h"
#include "../src/FLWindow.h"
#include "../src/FLRenderer.h"

namespace FireFlame {
Engine* Engine::theEngine = nullptr;
Engine::Engine(HINSTANCE hinst) {
	assert(theEngine == nullptr);
	theEngine = this;
	renderer = std::make_shared<Renderer>();
	MainWnd  = std::make_shared<Window>(hinst, renderer);
}
Engine::~Engine() {
	if (renderer) renderer->FlushCommandQueue();
}
void Engine::RegisterRendererUpdateFunc(std::function<void(float)> func){
	renderer->RegisterUpdateFunc(func);
}
void Engine::RegisterRendererDrawFunc(std::function<void(float)> func){
	renderer->RegisterDrawFunc(func);
}
int Engine::Run() {
	return MainWnd->Run();
}
int Engine::InitMainWindow(int x, int y, int w, int h) {
	return MainWnd->InitMainWindow(x, y, w, h);
}
}