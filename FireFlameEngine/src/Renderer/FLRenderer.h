#pragma once
#include <memory>
#include <wrl.h>
#include <DXGI1_5.h>
#include <d3d12.h>
#include <functional>
#include "..\FLTypeDefs.h"

namespace FireFlame {
class StopWatch;
class Window;
class Scene;
class Renderer {
public:
	Renderer() = default;
	//~Renderer() = default;
	bool Ready() const { return mReady; }
	void SetRenderWindow(std::shared_ptr<Window> wnd) { mRenderWnd = wnd; }
	int Initialize(API_Feature api);
	void Update(const StopWatch& gt);
	void Render(const StopWatch& gt);
	void FlushCommandQueue();
	void Resize();

	UINT GetSampleCount()const { return mSampleCount; }
	void ToggleMSAA();

	// register callbacks
	void RegisterUpdateFunc(std::function<void(float)> func) { mUpdateFunc = func; }
	void RegisterDrawFunc(std::function<void(float)> func)   { mDrawFunc = func; }

	// system probe
	int  LogAdapters(std::wostream& os);
	void LogAdapterDisplays(IDXGIAdapter* adapter, std::wostream& os);
	void LogDisplayModes(IDXGIOutput* output, DXGI_FORMAT format, std::wostream& os);

private:
	bool                   mReady = false;
	std::weak_ptr<Window>  mRenderWnd;
	std::shared_ptr<Scene> mScene;

	void RenderWithMSAA(const StopWatch& gt);
	void RenderWithoutMSAA(const StopWatch& gt);

	// callbacks
	std::function<void(float)> mUpdateFunc = [](float) {};
	std::function<void(float)> mDrawFunc   = [](float) {};

	void CreateCommandObjects();
	void CreateSwapChain();
	void CreateRtvAndDsvDescriptorHeaps();

	ID3D12Resource* CurrentBackBuffer() const;
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const;
	D3D12_CPU_DESCRIPTOR_HANDLE OffscreenRenderTargetView() const;
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const;

	Microsoft::WRL::ComPtr<IDXGIFactory5>  mdxgiFactory;
	Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain;
	Microsoft::WRL::ComPtr<ID3D12Device>   md3dDevice;

	Microsoft::WRL::ComPtr<ID3D12Fence>    mFence;
	UINT64 mCurrentFence = 0;

	Microsoft::WRL::ComPtr<ID3D12CommandQueue>        mCommandQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator>    mDirectCmdListAlloc;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;

	static const int SwapChainBufferCount = 2;
	int mCurrBackBuffer                   = 0;
	Microsoft::WRL::ComPtr<ID3D12Resource> mSwapChainBuffer[SwapChainBufferCount];
	// for MSAA
	Microsoft::WRL::ComPtr<ID3D12Resource> m_offscreenRenderTarget;
	Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencilBuffer;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDsvHeap;

	D3D12_VIEWPORT mScreenViewport;
	D3D12_RECT     mScissorRect;

	UINT mRtvDescriptorSize       = 0;
	UINT mDsvDescriptorSize       = 0;
	UINT mCbvSrvUavDescriptorSize = 0;

	bool        mMSAAOn = true;
	UINT        mMSAAQuality = 0;
	UINT        mSampleCount = 4;

	DXGI_FORMAT mBackBufferFormat   = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
};
}