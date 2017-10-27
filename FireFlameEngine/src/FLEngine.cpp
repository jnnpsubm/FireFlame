#include "../header/FLEngine.h"
#include "../src/Window/FLWindow.h"
#include "../src/Renderer/FLRenderer.h"

namespace FireFlame {
Engine* Engine::theEngine = nullptr;
Engine::Engine(HINSTANCE hinst) {
	assert(theEngine == nullptr);
	theEngine = this;
	mRenderer = std::make_shared<Renderer>();
	mMainWnd  = std::make_shared<Window>(hinst, mRenderer);
	mRenderer->SetRenderWindow(mMainWnd);
}
Engine::~Engine() {
	//if (renderer) renderer->FlushCommandQueue();
}
void Engine::RegisterRendererUpdateFunc(std::function<void(float)> func){
	mRenderer->RegisterUpdateFunc(func);
}
void Engine::RegisterRendererDrawFunc(std::function<void(float)> func){
	mRenderer->RegisterDrawFunc(func);
}
int Engine::Run() {
	return mMainWnd->Run();
}
int Engine::InitMainWindow(int x, int y, int w, int h) {
	return mMainWnd->InitMainWindow(x, y, w, h);
}
int Engine::InitRenderer(API_Feature api) {
	return mRenderer->Initialize(api);
}

// system probe
int Engine::LogVideoAdapters(std::wostream& os) {
	return mRenderer->LogAdapters(os);
}
}