#pragma once
#include <vector>
#include <memory>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include "..\Matrix\FLMatrix4X4.h"

namespace FireFlame {
struct stShaderDescription;
struct ShaderConstants {
    DirectX::XMFLOAT4X4 WorldViewProj = Matrix4X4();
};
class D3DShaderWrapper {
public:
    D3DShaderWrapper() = default;
    void BuildConstantBuffers(ID3D12Device* device);
    void BuildCBVDescriptorHeaps(ID3D12Device* device, UINT numDescriptors);
    void BuildShadersAndInputLayout(const stShaderDescription& shaderDesc, UINT vertexFormat);

private:
    std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

    Microsoft::WRL::ComPtr<ID3DBlob> mVSByteCode = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> mPSByteCode = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> mTSByteCode = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> mGSByteCode = nullptr;

    //std::unique_ptr<UploadBuffer<ShaderConstants>> mShaderCB      = nullptr;
    Microsoft::WRL::ComPtr<ID3D12RootSignature>    mRootSignature = nullptr;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>   mCbvHeap       = nullptr;

    void BuildInputLayout(const stShaderDescription& shaderDesc, UINT vertexFormat);
};

} // end namespace