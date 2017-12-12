#pragma once
#include <d3d12.h>

namespace FireFlame {
class D3DFilter
{
public:
    virtual void OnResize(UINT newWidth, UINT newHeight) = 0;
    virtual void Go
    (
        ID3D12GraphicsCommandList* cmdList,
        ID3D12RootSignature* rootSig,
        ID3D12PipelineState* horzBlurPSO,
        ID3D12PipelineState* vertBlurPSO,
        ID3D12Resource* inputResource,
        int blurCount
    ) = 0;
};
} // end FireFlame
