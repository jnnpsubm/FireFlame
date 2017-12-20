#pragma once
#include "FLD3DFilter.h"
#include "..\3rd_utils\d3dx12.h"
#include <vector>
#include <d3d12.h>
#include <wrl/client.h>

namespace FireFlame{
class D3DSobelFilter : public D3DFilter
{
public:
    enum Mode
    {
        modeEdge,
        modeCartoon
    };

public:
    D3DSobelFilter
    (
        ID3D12Device* device, Mode mode,
        UINT width, UINT height, DXGI_FORMAT format
    );
    ID3D12Resource* GetResultResource() override final;
    void OnResize(UINT sampleCount, UINT sampleQuality, UINT newWidth, UINT newHeight) override final;
    void Go
    (
        ID3D12GraphicsCommandList* cmdList, 
        ID3D12Resource* inputResource,
        ID3D12DescriptorHeap* heap,
        D3D12_GPU_DESCRIPTOR_HANDLE descriptor
    ) override final;

private:
    void BuildResources();
    void BuildDescriptors();

    void BuildRootSignature();
    void BuildPSOs();

private:
    ID3D12Device* md3dDevice = nullptr;

    Mode mode = modeEdge;
    UINT mWidth = 0;
    UINT mHeight = 0;
    DXGI_FORMAT mFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDescriptorHeap = nullptr;
    Microsoft::WRL::ComPtr<ID3D12RootSignature>  mRootSignature  = nullptr;
    Microsoft::WRL::ComPtr<ID3D12PipelineState>  mPSOEdge        = nullptr;
    Microsoft::WRL::ComPtr<ID3D12PipelineState>  mPSOCartoon     = nullptr;

    CD3DX12_GPU_DESCRIPTOR_HANDLE          mOutTexUav;
    Microsoft::WRL::ComPtr<ID3D12Resource> mOutTexture = nullptr;
};
} // end FireFlame
