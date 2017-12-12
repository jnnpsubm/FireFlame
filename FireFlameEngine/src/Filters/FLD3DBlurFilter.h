//***************************************************************************************
// BlurFilter.h by Frank Luna (C) 2011 All Rights Reserved.
//
// Performs a blur operation on the topmost mip level of an input texture.
//***************************************************************************************
#pragma once
#include "FLD3DFilter.h"
#include "..\3rd_utils\d3dx12.h"
#include <vector>
#include <d3d12.h>
#include <wrl/client.h>

namespace FireFlame{
class D3DBlurFilter : public D3DFilter
{
public:
    D3DBlurFilter(ID3D12Device* device, UINT width, UINT height, DXGI_FORMAT format);

    ID3D12Resource* GetResultResource();

    void BuildDescriptors
    (
        CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuDescriptor,
        CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuDescriptor,
        UINT descriptorSize
    );

    void OnResize(UINT newWidth, UINT newHeight) override final;

    // todo : use one pso,transpose and then transpose back
    void Go
    (
        ID3D12GraphicsCommandList* cmdList,
        ID3D12RootSignature* rootSig,
        ID3D12PipelineState* horzBlurPSO,
        ID3D12PipelineState* vertBlurPSO,
        ID3D12Resource* inputResource,
        int blurCount
    ) override final;

private:
    std::vector<float> CalcGaussWeights(float sigma);

    void BuildDescriptors();
    void BuildResources();

private:
    ID3D12Device* md3dDevice = nullptr;

    const int MaxBlurRadius = 5;

    UINT mWidth = 0;
    UINT mHeight = 0;
    DXGI_FORMAT mFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

    CD3DX12_CPU_DESCRIPTOR_HANDLE mBlur0CpuSrv;
    CD3DX12_CPU_DESCRIPTOR_HANDLE mBlur0CpuUav;

    CD3DX12_CPU_DESCRIPTOR_HANDLE mBlur1CpuSrv;
    CD3DX12_CPU_DESCRIPTOR_HANDLE mBlur1CpuUav;

    CD3DX12_GPU_DESCRIPTOR_HANDLE mBlur0GpuSrv;
    CD3DX12_GPU_DESCRIPTOR_HANDLE mBlur0GpuUav;

    CD3DX12_GPU_DESCRIPTOR_HANDLE mBlur1GpuSrv;
    CD3DX12_GPU_DESCRIPTOR_HANDLE mBlur1GpuUav;

    // Two for ping-ponging the textures.
    Microsoft::WRL::ComPtr<ID3D12Resource> mBlurTexture0 = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource> mBlurTexture1 = nullptr;
};
} // end FireFlame
