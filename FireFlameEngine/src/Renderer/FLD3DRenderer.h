#pragma once
#include <memory>
#include <wrl.h>
#include <DXGI1_5.h>
#include <d3d12.h>
#include <functional>
#include "FLRenderer.h"
#include "..\FLTypeDefs.h"
#include "..\FrameResource\FLD3DFrameResource.h"
#include "..\Filters\FLD3DFilter.h"

namespace FireFlame {
class StopWatch;
class Window;
class D3DRenderer : public Renderer {
    typedef std::vector<std::shared_ptr<D3DFrameResource>> VecFrameRes;

public:
	D3DRenderer() = default;
	//~D3DRenderer() = default;
	bool Ready() const { return mReady; }
	void SetRenderWindow(std::shared_ptr<Window> wnd) { mRenderWnd = wnd; }
	int  Initialize(API_Feature api);
	void Update(const StopWatch& gt);
	void Render(const StopWatch& gt);

    void ResetCommandList();
    void ExecuteCommand();
	void WaitForGPU();
    void WaitForGPUFrame();
    void WaitForGPUCurrentFrame();

    UINT64 SetFence();
    bool FenceReached(UINT64 fence);

    void AddFilter(const std::string& name, const FilterParam& filter);
    void RemoveFilter(const std::string& name);

    void GrabScreen(const std::wstring& filename);

	void Resize();

	void ToggleMSAA();

    // Get Methods
    ID3D12Device*              GetDevice()             const { return md3dDevice.Get(); }
    DXGI_FORMAT                GetBackBufferFormat()   const { return mBackBufferFormat; }
    DXGI_FORMAT                GetDepthStencilFormat() const { return mDepthStencilFormat; }
    UINT                       GetMSAAMode()           const { return mMSAAMode; }
    bool                       GetMSAAStatus()         const { return mSampleCount > 1; }
    UINT                       GetMSAASampleCount()    const { return mSampleCount; }
    UINT                       GetMSAAQuality()        const { return mMSAAQuality; }
    CRef_MSAADesc_Vec          GetMSAASupported()      const { return mMSAASupported; }
    ID3D12GraphicsCommandList* GetCommandList()        const { return mCommandList.Get(); }
    D3D12_CULL_MODE            GetCullMode()           const { return mCullMode; }
    D3D12_FILL_MODE            GetFillMode()           const { return mFillMode; }
    UINT GetRtvDescriptorSize()                        const { return mRtvDescriptorSize; }
    UINT GetDsvDescriptorSize()                        const { return mDsvDescriptorSize; }
    UINT GetCbvSrvUavDescriptorSize()                  const { return mCbvSrvUavDescriptorSize; }

    VecFrameRes& GetFrameResources()                         { return mFrameResources; }
    D3DFrameResource* GetCurrFrameResource()                 { return mCurrFrameResource; }
    int GetCurrFrameResIndex()                               { return mCurrFrameResourceIndex; }

    // Set Methods
    void SetDefaultClearColor(const float(&color)[4]) 
    { 
        memcpy(mDefaultClearColor, color, sizeof(mDefaultClearColor)); 
    }
    void SetCullMode(D3D12_CULL_MODE mode)       { mCullMode = mode; }
    void SetFillMode(D3D12_FILL_MODE mode)       { mFillMode = mode; }
    void SetCurrentPSO(ID3D12PipelineState* pso) { mCurrPSO = pso; }
    void SetViewport(const stViewport& viewport) {
        mScreenViewport.TopLeftX = viewport.x;
        mScreenViewport.TopLeftY = viewport.y;
        mScreenViewport.Width    = viewport.w;
        mScreenViewport.Height   = viewport.h;
        mScreenViewport.MinDepth = viewport.z0;
        mScreenViewport.MaxDepth = viewport.z1;
    }
    void SetScissorRect(int left, int top, int right, int bottom) {
        mScissorRect.left = left;
        mScissorRect.right = right;
        mScissorRect.top = top;
        mScissorRect.bottom = bottom;
    }

	// register callbacks
	void RegisterDrawFunc(std::function<void(float)> func)   { mDrawFunc = func; }
    void RegisterDrawFunc(std::function<void(ID3D12GraphicsCommandList*)> func){
        mDrawFuncWithCmdList = func;
    }
    void RegisterPreRenderFunc(std::function<void()> func) {
        mPreRenderFunc = func;
    }

	// system probe
	int  LogAdapters(std::wostream& os);
	void LogAdapterDisplays(IDXGIAdapter* adapter, std::wostream& os);
	void LogDisplayModes(IDXGIOutput* output, DXGI_FORMAT format, std::wostream& os);

private:
	bool                   mReady = false;
	std::weak_ptr<Window>  mRenderWnd;

    std::function<void(const StopWatch&)> mMSAARenderer = nullptr;
    void SelectMSAARenderer();
	void RenderWithMSAA(const StopWatch& gt);
	void RenderWithoutMSAA(const StopWatch& gt);

    // post process
    std::unordered_map<std::string, std::unique_ptr<D3DFilter>>         mFilters;

	// callbacks
	std::function<void(float)>                      mDrawFunc   = [](float) {};
    std::function<void(ID3D12GraphicsCommandList*)> mDrawFuncWithCmdList = [](ID3D12GraphicsCommandList*) {};
    std::function<void()>                           mPreRenderFunc = []() {};

	void CreateCommandObjects();
	void CreateSwapChain();
	void CreateRtvAndDsvDescriptorHeaps();

	ID3D12Resource* CurrentBackBuffer() const;
    ID3D12Resource* CurrentFrontBuffer() const;
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const;
	D3D12_CPU_DESCRIPTOR_HANDLE OffscreenRenderTargetView() const;
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const;

    float                mDefaultClearColor[4]{ 0.823529482f, 0.411764741f, 0.117647067f, 1.000000000f };

    ID3D12PipelineState* mCurrPSO = nullptr;

	Microsoft::WRL::ComPtr<IDXGIFactory5>  mdxgiFactory;
	Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain;
	Microsoft::WRL::ComPtr<ID3D12Device>   md3dDevice;

	Microsoft::WRL::ComPtr<ID3D12Fence>    mFence;
	UINT64 mCurrentFence = 0;

	Microsoft::WRL::ComPtr<ID3D12CommandQueue>        mCommandQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator>    mDirectCmdListAlloc;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;

    // frame resources
    VecFrameRes       mFrameResources;
    D3DFrameResource* mCurrFrameResource      = nullptr;
    int               mCurrFrameResourceIndex = 0;

	static const int SwapChainBufferCount = 2;
	int mCurrBackBuffer                   = 0;
    int mCurrFrontBuffer                  = 0;
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

    UINT        mMSAAMode    = 0;
	UINT        mMSAAQuality = 0;
	UINT        mSampleCount = 8;

    D3D12_CULL_MODE mCullMode = D3D12_CULL_MODE_BACK;
    D3D12_FILL_MODE mFillMode = D3D12_FILL_MODE_SOLID;

    std::vector<stMSAADesc> mMSAASupported;
    void GatherMSAAModeSupported();
    void SelectMSAAMode(UINT sampleCount);

	DXGI_FORMAT mBackBufferFormat   = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
};
}