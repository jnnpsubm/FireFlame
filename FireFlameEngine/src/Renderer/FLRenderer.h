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
class Renderer {
public:
	Renderer() = default;
	//~Renderer() = default;
	bool Ready() const { return mReady; }
	void SetRenderWindow(std::shared_ptr<Window> wnd) { mRenderWnd = wnd; }
	int Initialize(API_Feature api);
	void Update(const StopWatch& gt);
	void Render(const StopWatch& gt);

    void ResetCommandList();
    void ExecuteCommand();
	void WaitForGPU();

	void Resize();

	void CheckMSAASupport();
	UINT GetSampleCount()const { return mSampleCount; }
	void ToggleMSAA();

    // Get Methods
    ID3D12Device*              GetDevice() const             { return md3dDevice.Get(); }
    DXGI_FORMAT                GetBackBufferFormat() const   { return mBackBufferFormat; }
    DXGI_FORMAT                GetDepthStencilFormat() const { return mDepthStencilFormat; }
    bool                       GetMSAAStatus() const         { return mMSAAOn; }
    UINT                       GetMSAASampleCount() const    { return mSampleCount; }
    UINT                       GetMSAAQuality() const        { return mMSAAQuality; }
    ID3D12GraphicsCommandList* GetCommandList() const        { return mCommandList.Get(); }

	// register callbacks
	void RegisterDrawFunc(std::function<void(float)> func)   { mDrawFunc = func; }
    void RegisterDrawFunc(std::function<void(ID3D12GraphicsCommandList*)> func){
        mDrawFuncWithCmdList = func;
    }

	// system probe
	int  LogAdapters(std::wostream& os);
	void LogAdapterDisplays(IDXGIAdapter* adapter, std::wostream& os);
	void LogDisplayModes(IDXGIOutput* output, DXGI_FORMAT format, std::wostream& os);

private:
	bool                   mReady = false;
	std::weak_ptr<Window>  mRenderWnd;

	void RenderWithMSAA(const StopWatch& gt);
	void RenderWithoutMSAA(const StopWatch& gt);

	// callbacks
	std::function<void(float)> mDrawFunc   = [](float) {};
    std::function<void(ID3D12GraphicsCommandList*)> mDrawFuncWithCmdList;

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
	Microsoft::WRL::ComPtr<ID3D12Resource> mOffscreenRenderTarget; // for MSAA
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