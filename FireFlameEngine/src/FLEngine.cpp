#include "../header/FLEngine.h"
#include "../src/Window/FLWindow.h"
#include "../src/Renderer/FLRenderer.h"

namespace FireFlame {
Engine* Engine::theEngine = nullptr;
Engine::Engine(HINSTANCE hinst) {
	assert(theEngine == nullptr);
	theEngine = this;
	renderer = std::make_shared<Renderer>();
	MainWnd  = std::make_shared<Window>(hinst, renderer);
	renderer->SetRenderWindow(MainWnd);
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
int Engine::InitRenderer(API_Feature api) {
	return renderer->Initialize(api);
}

// system probe
int Engine::LogVideoAdapters(std::wostream& os) {
	return renderer->LogAdapters(os);
}
}