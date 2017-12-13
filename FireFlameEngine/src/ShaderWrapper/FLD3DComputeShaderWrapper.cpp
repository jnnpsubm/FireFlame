#include "FLD3DComputeShaderWrapper.h"
#include <vector>
#include "..\3rd_utils\d3dx12.h"
#include "..\FLTypeDefs.h"
#include "..\FLD3DUtils.h"
#include "..\3rd_utils\spdlog\spdlog.h"

namespace FireFlame {
void D3DComputeShaderWrapper::BuildRootSignature(ID3D12Device* device, const ComputeShaderDescription& desc)
{
    std::vector<CD3DX12_ROOT_PARAMETER> slotRootParameters(desc.rootParameters.size());
    for (size_t i = 0; i < desc.rootParameters.size(); i++)
    {
        const auto& rootParam = desc.rootParameters[i];
        auto paramType = rootParam.ptype;
        auto visibility = FLShaderVisiblity2D3DShaderVisiblity(rootParam.visibility);
        switch (paramType)
        {
        case FireFlame::ROOT_PARAMETER_TYPE::DESCRIPTOR_TABLE:
            throw std::runtime_error("Do not support root parameter type DESCRIPTOR_TABLE right now");
        case FireFlame::ROOT_PARAMETER_TYPE::_32BIT_CONSTANTS:
            slotRootParameters[i].InitAsConstants(rootParam.datasize, rootParam.baseRegister, rootParam.registerSpace, visibility);
            break;
        case FireFlame::ROOT_PARAMETER_TYPE::CBV:
            slotRootParameters[i].InitAsConstantBufferView(rootParam.baseRegister,rootParam.registerSpace,visibility);
            break;
        case FireFlame::ROOT_PARAMETER_TYPE::SRV:
            slotRootParameters[i].InitAsShaderResourceView(rootParam.baseRegister, rootParam.registerSpace, visibility);
            break;
        case FireFlame::ROOT_PARAMETER_TYPE::UAV:
            slotRootParameters[i].InitAsUnorderedAccessView(rootParam.baseRegister, rootParam.registerSpace, visibility);
            break;
        default:
            throw std::runtime_error("unknown root parameter type...");
        }
        mRootParams[rootParam.name] = { (UINT)i,nullptr };
    }

    auto staticSamplers = GetStaticSamplers();
    // A root signature is an array of root parameters.
    CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc
    (
        (UINT)slotRootParameters.size(), &slotRootParameters[0],
        desc.addDefaultSamplers ? (UINT)staticSamplers.size() : 0,
        desc.addDefaultSamplers ? staticSamplers.data() : nullptr,
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
    );

    Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
    HRESULT hr = D3D12SerializeRootSignature
    (
        &rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
        serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf()
    );

    if (errorBlob != nullptr) {
        ::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
    }
    ThrowIfFailed(hr);

    ThrowIfFailed
    (
        device->CreateRootSignature
        (
            0,
            serializedRootSig->GetBufferPointer(),
            serializedRootSig->GetBufferSize(),
            IID_PPV_ARGS(mRootSignature.ReleaseAndGetAddressOf())
        )
    );
}

void D3DComputeShaderWrapper::BuildShader(const ComputeShaderDescription& desc)
{
    for (const auto& shaderStage : desc.shaderStage) 
    {
        Microsoft::WRL::ComPtr<ID3DBlob> byteCode = nullptr;
        D3D_SHADER_MACRO* defines = nullptr;
        if (!shaderStage.Macros.empty())
        {
            defines = new D3D_SHADER_MACRO[shaderStage.Macros.size() + 1];
            for (size_t i = 0; i < shaderStage.Macros.size(); ++i)
            {
                defines[i].Name = shaderStage.Macros[i].first.c_str();
                defines[i].Definition = shaderStage.Macros[i].second.c_str();
            }
            defines[shaderStage.Macros.size()].Name = NULL;
            defines[shaderStage.Macros.size()].Definition = NULL;
        }
        if (!shaderStage.file.empty())
        {
            byteCode = D3DUtils::CompileShader
            (
                shaderStage.file, defines,
                shaderStage.entry,
                shaderStage.target
            );
        }
        else
        {
            byteCode = D3DUtils::CompileShader
            (
                shaderStage.data, defines,
                shaderStage.entry,
                shaderStage.target
            );
        }
        delete[] defines;

        switch (shaderStage.type) {
        case Shader_Type::CS: {
            mCSByteCodes[shaderStage.Macros2String()] = byteCode;
        }break;
        default:
            throw std::exception("unhandled shader stage type......");
            break;
        }
    }
}

} // end FireFlame