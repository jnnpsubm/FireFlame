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
class D3DBilateralBlurFilter : public D3DFilter
{
public:
    D3DBilateralBlurFilter
    (
        ID3D12Device* device, 
        UINT width, UINT height, DXGI_FORMAT format,
        int blurCount = 4, float sigma = 2.5f
    );
    void BuildResources(UINT sampleCount, UINT sampleQuality);
    void BuildDescriptors(UINT descriptorSize);
    ID3D12Resource* GetResultResource() override final;
    void OnResize(UINT sampleCount, UINT sampleQuality, UINT newWidth, UINT newHeight) override final;
    // todo : use one pso,transpose and then transpose back
    void Go
    (
        ID3D12GraphicsCommandList* cmdList, 
        ID3D12Resource* inputResource,
        ID3D12DescriptorHeap* heap,
        D3D12_GPU_DESCRIPTOR_HANDLE descriptor
    ) override final;

private:
    std::vector<float> CalcGaussWeights();

    void BuildRootSignature();
    void BuildPSOs();
    void BuildDescriptors();

private:
    ID3D12Device* md3dDevice = nullptr;

    const int mMaxBlurRadius = 10;
    const int mBlurCount = 4;
    const float mSigma = 2.5f;

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

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDescriptorHeap = nullptr;
    Microsoft::WRL::ComPtr<ID3D12RootSignature>  mRootSignature = nullptr;
    Microsoft::WRL::ComPtr<ID3D12PipelineState>  mPSOHorz = nullptr;
    Microsoft::WRL::ComPtr<ID3D12PipelineState>  mPSOVert = nullptr;

    // Two for ping-ponging the textures.
    Microsoft::WRL::ComPtr<ID3D12Resource> mBlurTexture0 = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource> mBlurTexture1 = nullptr;
};
} // end FireFlame
