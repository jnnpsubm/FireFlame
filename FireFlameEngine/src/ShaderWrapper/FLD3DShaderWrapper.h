#pragma once
#include <vector>
#include <wrl.h>
#include <d3d12.h>

namespace FireFlame {
struct stShaderDescription;
class D3DShaderWrapper {
public:
    D3DShaderWrapper() = default;
    void BuildCBVDescriptorHeaps(ID3D12Device* device, UINT numDescriptors);
    HRESULT BuildShadersAndInputLayout(const stShaderDescription& shaderDesc);

private:
    std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

    Microsoft::WRL::ComPtr<ID3DBlob> mvsByteCode = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> mpsByteCode = nullptr;

    Microsoft::WRL::ComPtr<ID3D12RootSignature>  mRootSignature = nullptr;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mCbvHeap       = nullptr;
};

} // end namespace