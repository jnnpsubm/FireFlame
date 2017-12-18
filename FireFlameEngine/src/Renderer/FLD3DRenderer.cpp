#include <string>
#include <vector>
#include <DirectXColors.h>
#include "FLD3DRenderer.h"
#include "..\Engine\FLEngine.h"
#include "..\Window\FLWindow.h"
#include "..\FLD3DUtils.h"
#include "..\Exception\FLException.h"
#include "..\Timer\FLStopWatch.h"
#include "..\3rd_utils\spdlog\spdlog.h"
#include "..\3rd_utils\d3dx12.h"
#include "..\3rd_utils\ScreenGrab\ScreenGrab12.h"
#include "..\Filters\FLD3DBlurFilter.h"

namespace FireFlame {
//D3DRenderer::~D3DRenderer() {
//	//OutputDebugString(L"~D3DRenderer executed...\n");
//}
void D3DRenderer::Update(const StopWatch& gt) {
	
}

void D3DRenderer::CreateThisThreadCmdList
(
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmdList
)
{
    ThrowIfFailed
    (
        md3dDevice->CreateCommandList
        (
            0,
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            mDirectCmdListAlloc.Get(), // Associated command allocator
            nullptr,                   // Initial PipelineStateObject
            IID_PPV_ARGS(cmdList.GetAddressOf())
        )
    );
    cmdList->Close();
}

void D3DRenderer::Render(const StopWatch& gt) {
    mPreRenderFunc();
	
    auto cmdListAlloc = mCurrFrameResource->CmdListAlloc;
	ThrowIfFailed(cmdListAlloc->Reset());
	// A command list can be reset after it has been added to the command queue via ExecuteCommandList.
	// Reusing the command list reuses memory.
	ThrowIfFailed(mCommandList->Reset(cmdListAlloc.Get(), mCurrPSO));
	// Indicate a state transition on the resource usage.
	// Transition the render target into the correct state to allow for drawing into it.
	
    mMSAARenderer(gt);

	// Done recording commands.
	ThrowIfFailed(mCommandList->Close());

	// Add the command list to the queue for execution.
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// swap the back and front buffers
	ThrowIfFailed(mSwapChain->Present(0, 0));
    mCurrFrontBuffer = mCurrBackBuffer;
	mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;

    {
        std::lock_guard<std::mutex> lock(mFenceMutex);
        mCurrFrameResource->Fence = ++mCurrentFence;
        mCommandQueue->Signal(mFence.Get(), mCurrFrameResource->Fence);
    }
}
void D3DRenderer::SelectMSAARenderer() {
    mMSAARenderer = mSampleCount > 1 ?
        std::bind(&D3DRenderer::RenderWithMSAA, this, std::placeholders::_1) :
        std::bind(&D3DRenderer::RenderWithoutMSAA, this, std::placeholders::_1);
}
void D3DRenderer::RenderWithMSAA(const StopWatch& gt) {
	D3D12_RESOURCE_BARRIER barrier2RT = CD3DX12_RESOURCE_BARRIER::Transition(
		mOffscreenRenderTarget.Get(),
		D3D12_RESOURCE_STATE_RESOLVE_SOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
	mCommandList->ResourceBarrier(1, &barrier2RT);

	// Specify the buffers we are going to render to.
	mCommandList->OMSetRenderTargets(1, &OffscreenRenderTargetView(), true, &DepthStencilView());
	// Clear the back buffer and depth buffer.
	mCommandList->ClearRenderTargetView(OffscreenRenderTargetView(), mDefaultClearColor, 0, nullptr);
	mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	// Set the viewport and scissor rect.  This needs to be reset whenever the command list is reset.
	mCommandList->RSSetViewports(1, &mScreenViewport);
	mCommandList->RSSetScissorRects(1, &mScissorRect);

	// todo : render something here
	mDrawFunc(gt.DeltaTime()); // Engine users can draw here.
    mDrawFuncWithCmdList(mCommandList.Get());

	D3D12_RESOURCE_BARRIER barriers[2] =
	{
		CD3DX12_RESOURCE_BARRIER::Transition(mOffscreenRenderTarget.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_RESOLVE_SOURCE),
		CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RESOLVE_DEST)
	};
	mCommandList->ResourceBarrier(2, barriers);

	mCommandList->ResolveSubresource(CurrentBackBuffer(), 0,
		mOffscreenRenderTarget.Get(), 0, mBackBufferFormat);

    // post process
    mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
        D3D12_RESOURCE_STATE_RESOLVE_DEST, D3D12_RESOURCE_STATE_COPY_SOURCE));
    for (auto& itFilter : mFilters)
    {
        auto filter = itFilter.second.get();
        filter->Go(mCommandList.Get(), CurrentBackBuffer());
        // Prepare to copy blurred output to the back buffer.
        mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
            D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_COPY_DEST));
        mCommandList->CopyResource(CurrentBackBuffer(), filter->GetResultResource());
        // Transition to original state.
        mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
            D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_COPY_SOURCE));
    }

	D3D12_RESOURCE_BARRIER barrier2Present =
		CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
            D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_PRESENT);
	mCommandList->ResourceBarrier(1, &barrier2Present);
}
void D3DRenderer::RenderWithoutMSAA(const StopWatch& gt) {
	// Indicate a state transition on the resource usage.
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	// Specify the buffers we are going to render to.
	mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());
	// Clear the back buffer and depth buffer.
	mCommandList->ClearRenderTargetView(CurrentBackBufferView(), mDefaultClearColor, 0, nullptr);
	mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	// Set the viewport and scissor rect.  This needs to be reset whenever the command list is reset.
	mCommandList->RSSetViewports(1, &mScreenViewport);
	mCommandList->RSSetScissorRects(1, &mScissorRect);

	// todo : render something here
	mDrawFunc(gt.DeltaTime()); // Engine users can draw here.
    mDrawFuncWithCmdList(mCommandList.Get());

    // post process
    mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
        D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_SOURCE));
    for (auto& itFilter : mFilters)
    {
        auto filter = itFilter.second.get();
        filter->Go(mCommandList.Get(), CurrentBackBuffer());
        // Prepare to copy blurred output to the back buffer.
        mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
            D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_COPY_DEST));
        mCommandList->CopyResource(CurrentBackBuffer(), filter->GetResultResource());
        // Transition to original state.
        mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
            D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_COPY_SOURCE));
    }

    // Indicate a state transition on the resource usage.
    mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
        D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_PRESENT));
}
void D3DRenderer::ToggleMSAA() {
    if ((mMSAAMode + 1) < mMSAASupported.size() && ++mMSAAMode) {
        
    }
    else {
        mMSAAMode = 0;
    }
    mSampleCount = mMSAASupported[mMSAAMode].sampleCount;
    mMSAAQuality = mMSAASupported[mMSAAMode].qualityLevels;
    SelectMSAARenderer();
	Resize();
}
void D3DRenderer::ResetCommandList() {
    // Reset the command list to prep for initialization commands.
    ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));
}
void D3DRenderer::ExecuteCommand() {
    // Execute the initialization commands.
    ThrowIfFailed(mCommandList->Close());
    ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
    mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
}
void D3DRenderer::ExecuteCommand(ID3D12GraphicsCommandList* cmdList)
{
    ThrowIfFailed(cmdList->Close());
    ID3D12CommandList* cmdsLists[] = { cmdList };
    mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
}
void D3DRenderer::WaitForGPU() {
    UINT64 fence = 0;
    {
        std::lock_guard<std::mutex> lock(mFenceMutex);
        fence = mCurrentFence++;
        ThrowIfFailed(mCommandQueue->Signal(mFence.Get(), fence));
    }
	
	// Wait until the GPU has completed commands up to this fence point.
	if (mFence->GetCompletedValue() < fence){
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);

		// Fire event when GPU hits current fence.  
		ThrowIfFailed(mFence->SetEventOnCompletion(fence, eventHandle));

		// Wait until the GPU hits current fence event is fired.
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}
}
void D3DRenderer::WaitForGPUFrame() {
    // Cycle through the circular frame resource array.
    mCurrFrameResourceIndex = (mCurrFrameResourceIndex + 1) % Engine::NumFrameResources();
    mCurrFrameResource = mFrameResources[mCurrFrameResourceIndex].get();

    // Has the GPU finished processing the commands of the current frame resource?
    // If not, wait until the GPU has completed commands up to this fence point.
    if (mCurrFrameResource->Fence != 0 && mFence->GetCompletedValue() < mCurrFrameResource->Fence)
    {
#ifdef _DEBUG
        /*std::wstring info(L"Wait For GPU finishing current Frame......");
        info += std::to_wstring(mCurrFrameResource->Fence);
        info += L"\n";
        OutputDebugString(info.c_str());*/
#endif
        /*spdlog::get("console")->info
        (
            "WaitForGPUFrame:Wait For GPU finishing Frame {0:d}......",
            mCurrFrameResource->Fence
        );*/

        HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
        ThrowIfFailed(mFence->SetEventOnCompletion(mCurrFrameResource->Fence, eventHandle));
        WaitForSingleObject(eventHandle, INFINITE);
        CloseHandle(eventHandle);
    }
}

void D3DRenderer::WaitForGPUCurrentFrame()
{
    mCurrFrameResource = mFrameResources[mCurrFrameResourceIndex].get();

    // Has the GPU finished processing the commands of the current frame resource?
    // If not, wait until the GPU has completed commands up to this fence point.
    if (mCurrFrameResource->Fence != 0 && mFence->GetCompletedValue() < mCurrFrameResource->Fence)
    {
        spdlog::get("console")->debug
        (
            "WaitForGPUCurrentFrame:Wait For GPU finishing current Frame{0:d}......",
            mCurrFrameResource->Fence
        );

        HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
        ThrowIfFailed(mFence->SetEventOnCompletion(mCurrFrameResource->Fence, eventHandle));
        WaitForSingleObject(eventHandle, INFINITE);
        CloseHandle(eventHandle);
    }
}

UINT64 D3DRenderer::SetFence()
{
    std::lock_guard<std::mutex> lock(mFenceMutex);
    UINT64 fence = ++mCurrentFence;
    mCommandQueue->Signal(mFence.Get(), fence);
#ifdef _DEBUG
    //spdlog::get("console")->info("Set Fence at {0:d}", fence);
#endif
    return fence;
}

bool D3DRenderer::FenceReached(UINT64 fence)
{
#ifdef _DEBUG
    //spdlog::get("console")->info("Current completed value {0:d}", mFence->GetCompletedValue());
#endif
    return (mFence->GetCompletedValue() >= fence);
}

void D3DRenderer::AddFilter(const std::string& name, const FilterParam& filter)
{
    assert(!mRenderWnd.expired());
    assert(md3dDevice);
    assert(mCommandList);
    auto renderWindow = mRenderWnd.lock();
    if (!renderWindow) return;

    auto it = mFilters.find(name);
    if (it != mFilters.end())
    {
        spdlog::get("console")->warn("Filter {0} already exist......", name);
    }

    auto type = filter.type;
    switch (type)
    {
    case FireFlame::FilterType::Blur:
    {
        auto d3dfilter = std::make_unique<D3DBlurFilter>
        (
            md3dDevice.Get(),
            renderWindow->ClientWidth(), renderWindow->ClientHeight(), mBackBufferFormat,
            filter.blurCount, filter.sigma
        );
        d3dfilter->BuildResources(mSampleCount, mMSAAQuality - 1);
        d3dfilter->BuildDescriptors(mCbvSrvUavDescriptorSize);
        mFilters.emplace(name, std::move(d3dfilter));
    }break;
    case FireFlame::FilterType::Sobel:
        break;
    case FireFlame::FilterType::HDR:
        break;
    case FireFlame::FilterType::Max:
        break;
    default:
        break;
    }
}

void D3DRenderer::RemoveFilter(const std::string& name)
{
    auto it = mFilters.find(name);
    if (it == mFilters.end())
    {
        spdlog::get("console")->warn("can not find filter {0} to remove......", name);
        return;
    }

    WaitForGPU();
    mFilters.erase(it);
}

#include <wincodec.h>
void D3DRenderer::GrabScreen(const std::wstring& filename)
{
    WaitForGPUCurrentFrame();
    //DirectX::SaveDDSTextureToFile(mCommandQueue.Get(), CurrentFrontBuffer(), filename.c_str(),
        //D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_PRESENT);
    DirectX::SaveWICTextureToFile
    (
        mCommandQueue.Get(), CurrentFrontBuffer(), 
        GUID_ContainerFormatBmp, filename.c_str(),
        D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_PRESENT
    );
    spdlog::get("console")->info("screen shot saved.........");
}

int D3DRenderer::Initialize(API_Feature api) {
	assert(!mRenderWnd.expired());
	auto renderWindow = mRenderWnd.lock();

    D3D_FEATURE_LEVEL featureLevel = D3DUtils::FLAPIFeature2D3DFeatureLevel(api);

#if defined(DEBUG) || defined(_DEBUG) 
	// Enable the D3D12 debug layer.
	{
		Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
		debugController->EnableDebugLayer();
	}
#endif
	ThrowIfFailed(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&mdxgiFactory)));
	// Try to create hardware device.
	HRESULT hardwareResult = D3D12CreateDevice(
		nullptr,             // default adapter
        featureLevel,
		IID_PPV_ARGS(&md3dDevice));

    //hardwareResult = E_FAIL;
	// Fallback to WARP device.
	if (FAILED(hardwareResult)){
		OutputDebugString(L"======Fallback to WARP device.======\n");
		Microsoft::WRL::ComPtr<IDXGIAdapter> pWarpAdapter;
		ThrowIfFailed(mdxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));
		ThrowIfFailed(D3D12CreateDevice(
			pWarpAdapter.Get(),
            featureLevel,
			IID_PPV_ARGS(&md3dDevice)));
	}

	ThrowIfFailed(md3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence)));
	mRtvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	mDsvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	mCbvSrvUavDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// Check MSAA support......
    GatherMSAAModeSupported();
    SelectMSAAMode(mSampleCount);

	CreateCommandObjects();
	CreateSwapChain();
	CreateRtvAndDsvDescriptorHeaps();

    SelectMSAARenderer();
	Resize();
	mReady = true;
	return 0;
}
void D3DRenderer::GatherMSAAModeSupported() {
    // Check 4X MSAA quality support for our back buffer format.
    // All Direct3D 11 capable devices support 4X MSAA for all render 
    // target formats, so we only need to check quality support.
    D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
    msQualityLevels.Format = mBackBufferFormat;
    msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
    for (UINT i = 1; i <= 64; i <<= 1){ // todo : step size? should be pow of 2?
        msQualityLevels.SampleCount = i;
        msQualityLevels.NumQualityLevels = 0;
        HRESULT hr = md3dDevice->CheckFeatureSupport
        (
            D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
            &msQualityLevels,
            sizeof(msQualityLevels)
        );
        if (msQualityLevels.NumQualityLevels)
            mMSAASupported.emplace_back(i, msQualityLevels.NumQualityLevels);
    }
}
void D3DRenderer::SelectMSAAMode(UINT sampleCount) {
    mMSAAMode = 0;
    for (size_t i = 0; i < mMSAASupported.size(); ++i) {
        if (mMSAASupported[i].sampleCount == mSampleCount) {
            mMSAAMode = (UINT)i;
            break;
        }
    }
    mSampleCount = mMSAASupported[mMSAAMode].sampleCount;
    mMSAAQuality = mMSAASupported[mMSAAMode].qualityLevels;
}
void D3DRenderer::Resize(){
	assert(!mRenderWnd.expired());
	assert(md3dDevice);
	assert(mSwapChain);
	assert(mDirectCmdListAlloc);

	auto renderWindow = mRenderWnd.lock();
	if (!renderWindow) return;

	// Flush before changing any resources.
	WaitForGPU();

	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));

	// Release the previous resources we will be recreating.
	for (int i = 0; i < SwapChainBufferCount; ++i)
		mSwapChainBuffer[i].Reset();
	mDepthStencilBuffer.Reset();

	// Resize the swap chain.
	ThrowIfFailed(mSwapChain->ResizeBuffers(
		SwapChainBufferCount,
		renderWindow->ClientWidth(), renderWindow->ClientHeight(),
		mBackBufferFormat,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

	mCurrBackBuffer = 0;
    mCurrFrontBuffer = 0;

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(mRtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (UINT i = 0; i < SwapChainBufferCount; i++){
		ThrowIfFailed(mSwapChain->GetBuffer(i, IID_PPV_ARGS(&mSwapChainBuffer[i])));
		md3dDevice->CreateRenderTargetView(mSwapChainBuffer[i].Get(), nullptr, rtvHeapHandle);
		rtvHeapHandle.Offset(1, mRtvDescriptorSize);
	}
	// Create offscreen RT and view
	D3D12_RESOURCE_DESC msaaRTDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		mBackBufferFormat,
		renderWindow->ClientWidth(), renderWindow->ClientHeight(),
		1,            // This render target view has only one texture.
		1,            // Use a single mipmap level
		mSampleCount, // <--- Use MSAA 
		mMSAAQuality-1
	);
	msaaRTDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	D3D12_CLEAR_VALUE msaaOptimizedClearValue = {};
	msaaOptimizedClearValue.Format = mBackBufferFormat;
	memcpy(msaaOptimizedClearValue.Color, mDefaultClearColor, sizeof(float) * 4);

	ThrowIfFailed(md3dDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&msaaRTDesc,
		D3D12_RESOURCE_STATE_RESOLVE_SOURCE,
		&msaaOptimizedClearValue,
		IID_PPV_ARGS(mOffscreenRenderTarget.ReleaseAndGetAddressOf())
	));

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvDescriptor(
		mRtvHeap->GetCPUDescriptorHandleForHeapStart(),
		SwapChainBufferCount, mRtvDescriptorSize);
	md3dDevice->CreateRenderTargetView(mOffscreenRenderTarget.Get(), nullptr, rtvDescriptor);

	// Create the depth/stencil buffer and view.
	D3D12_RESOURCE_DESC depthStencilDesc;
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.Width = renderWindow->ClientWidth();
	depthStencilDesc.Height = renderWindow->ClientHeight();
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.MipLevels = 1;

	// Correction 11/12/2016: SSAO chapter requires an SRV to the depth buffer to read from 
	// the depth buffer.  Therefore, because we need to create two views to the same resource:
	//   1. SRV format: DXGI_FORMAT_R24_UNORM_X8_TYPELESS
	//   2. DSV Format: DXGI_FORMAT_D24_UNORM_S8_UINT
	// we need to create the depth buffer resource with a typeless format.  
	depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	depthStencilDesc.SampleDesc.Count = mSampleCount;
	depthStencilDesc.SampleDesc.Quality = mSampleCount > 1 ? mMSAAQuality - 1 : 0;
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE optClear;
	optClear.Format = mDepthStencilFormat;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;
	ThrowIfFailed(md3dDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&depthStencilDesc,
		D3D12_RESOURCE_STATE_COMMON,
		&optClear,
		IID_PPV_ARGS(mDepthStencilBuffer.GetAddressOf())));

	// Create descriptor to mip level 0 of entire resource using the format of the resource.
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension = mSampleCount>1?D3D12_DSV_DIMENSION_TEXTURE2DMS:D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = mDepthStencilFormat;
	dsvDesc.Texture2D.MipSlice = 0;
	md3dDevice->CreateDepthStencilView(mDepthStencilBuffer.Get(), &dsvDesc, DepthStencilView());

	// Transition the resource from its initial state to be used as a depth buffer.
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mDepthStencilBuffer.Get(),
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));

	// Execute the resize commands.
	ThrowIfFailed(mCommandList->Close());
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// Wait until resize is complete.
	WaitForGPU();

	// Update the viewport transform to cover the client area.
	mScreenViewport.TopLeftX = 0;
	mScreenViewport.TopLeftY = 0;
	mScreenViewport.Width = static_cast<float>(renderWindow->ClientWidth());
	mScreenViewport.Height = static_cast<float>(renderWindow->ClientHeight());
	mScreenViewport.MinDepth = 0.0f;
	mScreenViewport.MaxDepth = 1.0f;

	mScissorRect = { 0, 0, renderWindow->ClientWidth(), renderWindow->ClientHeight() };

    for (auto& itFilter : mFilters)
    {
        auto filter = itFilter.second.get();
        filter->OnResize(mSampleCount, mMSAAQuality - 1, renderWindow->ClientWidth(), renderWindow->ClientHeight());
    }
}
void D3DRenderer::CreateRtvAndDsvDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = SwapChainBufferCount+1; // add 1 for MSAA offscreen rt
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(mRtvHeap.GetAddressOf())));

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(mDsvHeap.GetAddressOf())));
}
void D3DRenderer::CreateSwapChain()
{
	assert(!mRenderWnd.expired());

	auto renderWindow = mRenderWnd.lock();
	if (!renderWindow) return;
	// Release the previous swapchain we will be recreating.
	mSwapChain.Reset();

	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = renderWindow->ClientWidth();
	sd.BufferDesc.Height = renderWindow->ClientHeight();
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = mBackBufferFormat;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = SwapChainBufferCount;
	sd.OutputWindow = renderWindow->MainWnd();
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// Note: Swap chain uses queue to perform flush.
	ThrowIfFailed(mdxgiFactory->CreateSwapChain(
		mCommandQueue.Get(),
		&sd,
		mSwapChain.GetAddressOf()));

	// fullscreen mode not supported now.
	ThrowIfFailed(mdxgiFactory->MakeWindowAssociation(
		renderWindow->MainWnd(), DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_WINDOW_CHANGES));
}
void D3DRenderer::CreateCommandObjects()
{
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	ThrowIfFailed(md3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQueue)));

	ThrowIfFailed(md3dDevice->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(mDirectCmdListAlloc.GetAddressOf())));

    // create frame resources of Command Allocator    
    for (int i = 0; i < Engine::NumFrameResources(); ++i) {
        auto frameRes = std::make_shared<D3DFrameResource>(md3dDevice.Get());
        mFrameResources.emplace_back(std::move(frameRes));
    }
    mCurrFrameResourceIndex = 0;
    mCurrFrameResource = mFrameResources[mCurrFrameResourceIndex].get();

	ThrowIfFailed(md3dDevice->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		mDirectCmdListAlloc.Get(), // Associated command allocator
		nullptr,                   // Initial PipelineStateObject
		IID_PPV_ARGS(mCommandList.GetAddressOf())));

	// Start off in a closed state.  This is because the first time we refer 
	// to the command list we will Reset it, and it needs to be closed before
	// calling Reset.
	mCommandList->Close();

    ThrowIfFailed(md3dDevice->CreateCommandList(
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        mDirectCmdListAlloc.Get(), // Associated command allocator
        nullptr,                   // Initial PipelineStateObject
        IID_PPV_ARGS(mComputeCmdList.GetAddressOf())));
    mComputeCmdList->Close();
}
ID3D12Resource* D3DRenderer::CurrentBackBuffer() const {
	return mSwapChainBuffer[mCurrBackBuffer].Get();
}
ID3D12Resource* D3DRenderer::CurrentFrontBuffer() const {
    return mSwapChainBuffer[mCurrFrontBuffer].Get();
}
D3D12_CPU_DESCRIPTOR_HANDLE D3DRenderer::CurrentBackBufferView() const {
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(
		mRtvHeap->GetCPUDescriptorHandleForHeapStart(),
		mCurrBackBuffer,
		mRtvDescriptorSize);
}
D3D12_CPU_DESCRIPTOR_HANDLE D3DRenderer::OffscreenRenderTargetView() const {
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(
		mRtvHeap->GetCPUDescriptorHandleForHeapStart(),
		SwapChainBufferCount,
		mRtvDescriptorSize);
}
D3D12_CPU_DESCRIPTOR_HANDLE D3DRenderer::DepthStencilView() const {
	return mDsvHeap->GetCPUDescriptorHandleForHeapStart();
}

// system probe
int D3DRenderer::LogAdapters(std::wostream& os) {
	using Microsoft::WRL::ComPtr;
	UINT i = 0;
	ComPtr<IDXGIAdapter> adapter = nullptr;
	while (mdxgiFactory->EnumAdapters(i, adapter.ReleaseAndGetAddressOf()) != DXGI_ERROR_NOT_FOUND){
		DXGI_ADAPTER_DESC desc;
		adapter->GetDesc(&desc);
		os << L"***Adapter: " << desc.Description 
			<< " VideoMem:" << (float)desc.DedicatedVideoMemory/1024.f/1024.f << L"M" << std::endl;
		LogAdapterDisplays(adapter.Get(), os);
		++i;
	}
	return 0;
}
void D3DRenderer::LogAdapterDisplays(IDXGIAdapter* adapter, std::wostream& os){
	UINT i = 0;
	Microsoft::WRL::ComPtr<IDXGIOutput> display = nullptr;
	while (adapter->EnumOutputs(i, display.ReleaseAndGetAddressOf()) != DXGI_ERROR_NOT_FOUND){
		DXGI_OUTPUT_DESC desc;
		display->GetDesc(&desc);
		os << L"\t***Display: " << desc.DeviceName << std::endl;
		LogDisplayModes(display.Get(), mBackBufferFormat, os);
		++i;
	}
}
void D3DRenderer::LogDisplayModes(IDXGIOutput* output, DXGI_FORMAT format, std::wostream& os){
	using std::to_wstring;
	UINT count = 0;
	UINT flags = 0;
	// Call with nullptr to get list count.
	output->GetDisplayModeList(format, flags, &count, nullptr);
	std::vector<DXGI_MODE_DESC> modeList(count);
	output->GetDisplayModeList(format, flags, &count, &modeList[0]);
	for (auto& x : modeList){
		UINT n = x.RefreshRate.Numerator;
		UINT d = x.RefreshRate.Denominator;
		os << L"\t\tWidth = " << to_wstring(x.Width) << L" " 
		   << L"\t\tHeight = " << to_wstring(x.Height) << L" "
		   << L"\t\tRefresh = " << to_wstring(n) << L"/" << to_wstring(d) << std::endl;
	}
}
} // end namespace