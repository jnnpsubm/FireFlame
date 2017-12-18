#pragma once
#include "FLD3DShaderWrapperBase.h"
#include "..\FLTypeDefs.h"
#include <map>
#include <unordered_map>
#include <string>
#include <d3d12.h>

namespace FireFlame {
struct CSTask;
class D3DComputeShaderWrapper : public D3DShaderWrapperBase
{
public:
    D3DComputeShaderWrapper(const std::string& name) :D3DShaderWrapperBase(name) {}
    void BuildRootSignature(ID3D12Device* device, const ComputeShaderDescription& desc);
    void BuildShader(const ComputeShaderDescription& desc);

    void SetCSRootParamData
    (
        const std::string& taskName, const std::string& paramName,
        const ResourceDesc& resDesc, size_t dataLen, std::uint8_t* data
    );
    void Dispatch(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, const CSTask& taskDesc);
    void Copyback(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, const CSTask& taskDesc);
    void TaskCallback(const CSTask& taskDesc);
    void ClearTaskRes(const std::string& taskName);

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
    void UploadRootParamData(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, const std::string& taskName);

    struct RootParam
    {
        UINT paramIndex = 0;
        Root_Parameter_Mode paramMode;
        ROOT_PARAMETER_TYPE paramType;
        Resource_Dimension resDimension = Resource_Dimension::UNKNOWN;
        
        struct TaskRes
        {
            Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
            Microsoft::WRL::ComPtr<ID3D12Resource> uploadResource = nullptr;
            Microsoft::WRL::ComPtr<ID3D12Resource> readBackRes = nullptr;
            std::vector<std::uint8_t> bufferIn;
            size_t                    bufferOutLen = 0;
        };
        std::map<std::string, TaskRes> taskResources;
    };
    std::map<std::string, RootParam> mRootParams;

    std::unordered_map<std::string, BlobPtr> mCSByteCodes;
};
}
