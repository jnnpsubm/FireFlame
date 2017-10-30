#pragma once
#include <memory>
#include <wrl.h>
#include <DXGI1_4.h>
#include <d3d12.h>
#include <functional>
#include "..\Timer\FLStepTimer.h"
#include "..\FLTypeDefs.h"

namespace FireFlame{
class Window;
class RendererMS{
public:
	RendererMS();
	~RendererMS();
	void SetRenderWindow(std::shared_ptr<Window> wnd) { m_renderWnd = wnd; }
	// Initialization and management
	void Initialize(API_Feature api);

	// Basic game loop
	void Tick();

	// Messages
	void OnResuming();
	void OnWindowSizeChanged();

private:
	void Update(StepTimer const& timer);
	void Render();

	void Clear();
	void Present();

	void CreateDevice();
	void CreateResources();

	void WaitForGpu() noexcept;
	void MoveToNextFrame();
	void GetAdapter(IDXGIAdapter1** ppAdapter);

	void OnDeviceLost();

	// Application state
	std::shared_ptr<Window>                             m_renderWnd;

	// Direct3D Objects
	D3D_FEATURE_LEVEL                                   m_featureLevel;
	static const UINT                                   c_swapBufferCount = 2;
	UINT                                                m_backBufferIndex;
	UINT                                                m_rtvDescriptorSize;
	Microsoft::WRL::ComPtr<ID3D12Device>                m_d3dDevice;
	Microsoft::WRL::ComPtr<IDXGIFactory4>               m_dxgiFactory;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue>          m_commandQueue;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>        m_rtvDescriptorHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>        m_dsvDescriptorHeap;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator>      m_commandAllocators[c_swapBufferCount];
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>   m_commandList;
	Microsoft::WRL::ComPtr<ID3D12Fence>                 m_fence;
	UINT64                                              m_fenceValues[c_swapBufferCount];
	Microsoft::WRL::Wrappers::Event                     m_fenceEvent;

	// Rendering resources
	Microsoft::WRL::ComPtr<IDXGISwapChain3>             m_swapChain;
	Microsoft::WRL::ComPtr<ID3D12Resource>              m_renderTargets[c_swapBufferCount];
	Microsoft::WRL::ComPtr<ID3D12Resource>              m_depthStencil;

	// Game state
	StepTimer                                           m_timer;
};
}