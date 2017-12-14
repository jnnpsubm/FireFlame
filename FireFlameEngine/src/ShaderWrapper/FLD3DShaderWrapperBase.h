#pragma once
#include <string>
#include <array>
#include <wrl\client.h>
#include "..\3rd_utils\d3dx12.h"

namespace FireFlame {
struct stShaderStage;
class D3DShaderWrapperBase
{
public:
    D3DShaderWrapperBase(const std::string& name) : mName(name) {}

    ID3D12RootSignature*  GetRootSignature()    const { return mRootSignature.Get(); }

protected:
    std::string mName;

    Microsoft::WRL::ComPtr<ID3D12RootSignature>    mRootSignature = nullptr;

    std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();

protected:
    typedef Microsoft::WRL::ComPtr<ID3DBlob> BlobPtr;

    BlobPtr CompileShaderStage(const stShaderStage& shaderStage);

private:

};
} // end FireFlame