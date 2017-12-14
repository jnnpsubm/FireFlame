#pragma once
#include "FLD3DShaderWrapperBase.h"
#include <map>
#include <unordered_map>
#include <string>
#include <d3d12.h>

namespace FireFlame {
struct ComputeShaderDescription;
class D3DComputeShaderWrapper : public D3DShaderWrapperBase
{
public:
    D3DComputeShaderWrapper(const std::string& name) :D3DShaderWrapperBase(name) {}
    void BuildRootSignature(ID3D12Device* device, const ComputeShaderDescription& desc);
    void BuildShader(const ComputeShaderDescription& desc);

    std::pair<void*, size_t> GetCS(const std::string& macro) const
    {
        auto it = mCSByteCodes.find(macro);
        if (it == mCSByteCodes.end()) return std::make_pair(nullptr, 0);
        return std::make_pair
        (
            reinterpret_cast<void*>(it->second->GetBufferPointer()),
            it->second->GetBufferSize()
        );
    }

private:
    struct RootParam
    {
        UINT paramIndex = 0;
        Microsoft::WRL::ComPtr<ID3D12Resource> res = nullptr;
    };
    std::map<std::string, RootParam> mRootParams;

    std::unordered_map<std::string, BlobPtr> mCSByteCodes;
};
}
