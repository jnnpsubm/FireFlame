#include "FLEngine.h"
#include "..\Renderer\FLD3DRenderer.h"
#include "..\PSOManager\FLD3DPSOManager.h"
#include "..\FLD3DUtils.h"

namespace FireFlame {
Engine* Engine::theEngine = nullptr;
Engine::Engine(HINSTANCE hinst) {
	assert(theEngine == nullptr);
	theEngine = this;
	mRenderer = std::make_shared<D3DRenderer>();
	mMainWnd  = std::make_shared<Window>(hinst, *this);
	mRenderer->SetRenderWindow(mMainWnd);
    mScene = std::make_shared<Scene>(mRenderer);
    mPSOManager = std::make_shared<D3DPSOManager>();
}
Engine::~Engine() {
	//if (renderer) renderer->WaitForGPU();
}
void Engine::RegisterUpdateFunc(std::function<void(float)> func){
	mScene->RegisterUpdateFunc(func);
}
void Engine::RegisterRendererDrawFunc(std::function<void(float)> func){
	mRenderer->RegisterDrawFunc(func);
}
int Engine::Run() {
	mTimer.Reset();
	while (true) {
		if (!mMainWnd->HandleMsg()) {
			mTimer.Mark();
			if (!mAppPaused) {
				CalculateFrameStats();
				mScene->Update(mTimer);
				mScene->Render(mTimer);
			}else {
				Sleep(10);
			}
		}
		if (mMainWnd->Quitted()) break;
	}
	return mMainWnd->QuitCode();
}
void Engine::Stop() {
	Pause();
	mRenderer->WaitForGPU();
}
int Engine::InitMainWindow(int x, int y, int w, int h) {
	return mMainWnd->InitMainWindow(x, y, w, h);
}
int Engine::InitRenderer(API_Feature api) {
	return mRenderer->Initialize(api);
}
void Engine::SetCullMode(Cull_Mode mode) {
    // only support D3D right now
    mRenderer->SetCullMode(FLCullMode2D3DCullMode(mode));
}
void Engine::SetFillMode(Fill_Mode mode) {
    // only support D3D right now
    mRenderer->SetFillMode(FLFillMode2D3DFillMode(mode));
}
Cull_Mode Engine::GetCullMode() {
    return D3DCullMode2FLCullMode(mRenderer->GetCullMode());
}
Fill_Mode Engine::GetFillMode() {
    return D3DFillMode2FLFillMode(mRenderer->GetFillMode());
}
void Engine::CalculateFrameStats()
{
	// Code computes the average frames per second, and also the 
	// average time it takes to render one frame.  These stats 
	// are appended to the window caption bar.
	static int   frameCnt = 0;
	static float timeElapsed = 0.0f;
	frameCnt++;
	// Compute averages over one second period.
	if ((mTimer.TotalTime() - timeElapsed) >= 1.0f) {
		float fps = (float)frameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;
		std::wstring fpsStr = std::to_wstring(fps);
		std::wstring mspfStr = std::to_wstring(mspf);
		std::wstring windowText = L"    fps: " + fpsStr + L"   mspf: " + mspfStr +
			L"    SampleCount: " + std::to_wstring(mRenderer->GetMSAASampleCount());
		mMainWnd->AppendWindowCaption(windowText);
		// Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}
// =====================================Windows Msg Handling========================
LRESULT Engine::OnWindowDestroy() {
	Stop();
	return 0;
}
LRESULT Engine::OnWindowResized() {
	Resume();
	if (mRenderer->Ready()) {
		mRenderer->Resize();
	}
    mFuncWindowResizedHandler(mMainWnd->ClientWidth(), mMainWnd->ClientHeight());
	return 0;
}
LRESULT Engine::OnWindowKeyUp(WPARAM wParam, LPARAM lParam) {
	if (mRenderer->Ready() && (int)wParam == VK_F2) {
		mRenderer->ToggleMSAA();
	}
	return 0;
}
// =====================================End Windows Msg Handling====================

// system probe
int Engine::LogVideoAdapters(std::wostream& os) {
	return mRenderer->LogAdapters(os);
}
}