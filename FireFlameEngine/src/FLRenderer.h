#pragma once
#include <wrl.h>
#include <DXGI1_5.h>
#include <d3d12.h>
#include <functional>

namespace FireFlame {
class StopWatch;
class Renderer {
public:
	void Update(const StopWatch& gt);
	void Draw(const StopWatch& gt);
	void FlushCommandQueue();

	// register callbacks
	void RegisterUpdateFunc(std::function<void(float)> func) { 
		mUpdateFunc = func; 
	}
	void RegisterDrawFunc(std::function<void(float)> func)   { 
		mDrawFunc = func; 
	}
private:
	// callbacks
	std::function<void(float)> mUpdateFunc = [](float) {};
	std::function<void(float)> mDrawFunc   = [](float) {};

	Microsoft::WRL::ComPtr<IDXGIFactory5>  mdxgiFactory;
	Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain;
	Microsoft::WRL::ComPtr<ID3D12Device>   md3dDevice;

	Microsoft::WRL::ComPtr<ID3D12Fence>    mFence;
	UINT64 mCurrentFence = 0;

	Microsoft::WRL::ComPtr<ID3D12CommandQueue>        mCommandQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator>    mDirectCmdListAlloc;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;

	static const int SwapChainBufferCount = 2;
	int mCurrBackBuffer = 0;
	Microsoft::WRL::ComPtr<ID3D12Resource> mSwapChainBuffer[SwapChainBufferCount];
	Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencilBuffer;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDsvHeap;

	D3D12_VIEWPORT mScreenViewport;
	D3D12_RECT     mScissorRect;

	UINT mRtvDescriptorSize       = 0;
	UINT mDsvDescriptorSize       = 0;
	UINT mCbvSrvUavDescriptorSize = 0;

	DXGI_FORMAT mBackBufferFormat   = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
};
}