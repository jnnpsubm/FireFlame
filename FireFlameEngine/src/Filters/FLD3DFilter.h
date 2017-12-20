#pragma once
#include <d3d12.h>

namespace FireFlame {
class D3DFilter
{
public:
    virtual ~D3DFilter() = default;

    virtual void Go
    (
        ID3D12GraphicsCommandList* cmdList, 
        ID3D12Resource* inputResource, 
        ID3D12DescriptorHeap* heap,
        D3D12_GPU_DESCRIPTOR_HANDLE descriptor
    ) = 0;
    virtual ID3D12Resource* GetResultResource() = 0;

    virtual void OnResize(UINT sampleCount, UINT sampleQuality, UINT newWidth, UINT newHeight) = 0;
};
} // end FireFlame
