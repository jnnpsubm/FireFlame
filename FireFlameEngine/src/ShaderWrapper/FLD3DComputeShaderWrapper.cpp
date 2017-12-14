#include "FLD3DComputeShaderWrapper.h"
#include <vector>
#include "..\Engine\FLEngine.h"
#include "..\Renderer\FLD3DRenderer.h"
#include "..\3rd_utils\d3dx12.h"
#include "..\FLTypeDefs.h"
#include "..\FLD3DUtils.h"
#include "..\3rd_utils\spdlog\spdlog.h"
#include "..\PSOManager\FLD3DPSOManager2.h"

namespace FireFlame {
void D3DComputeShaderWrapper::SetCSRootParamData
(
    const std::string& paramName, 
    const ResourceDesc& resDesc, size_t dataLen, std::uint8_t* data
)
{
    auto itRootParam = mRootParams.find(paramName);
    if (itRootParam == mRootParams.end())
    {
        spdlog::get("console")->critical("can not find root param {0} in compute shader {1}", paramName, mName);
        throw std::runtime_error("can not find root param in SetCSRootParamData");
    }

    auto renderer = Engine::GetEngine()->GetRenderer();
    auto device = renderer->GetDevice();
    auto& rootParam = itRootParam->second;
    rootParam.resDimension = resDesc.dimension;
    switch (resDesc.dimension)
    {
    case Resource_Dimension::BUFFER:
    {
        if (rootParam.paramType == ROOT_PARAMETER_TYPE::SRV)
        {
            rootParam.bufferIn.resize(dataLen);
            memcpy(rootParam.bufferIn.data(), data, dataLen);
        }else if (rootParam.paramType == ROOT_PARAMETER_TYPE::UAV)
        {
            rootParam.bufferOutLen = dataLen;
        }
    }break;
    case Resource_Dimension::TEXTURE1D:
    {

    }break;
    case Resource_Dimension::TEXTURE2D:
    {

    }break;
    case Resource_Dimension::TEXTURE3D:
    {

    }break;
    case Resource_Dimension::UNKNOWN:
    {

    }break;
    default:
        throw std::runtime_error("unknown resource dimension");
    }
}

void D3DComputeShaderWrapper::Dispatch(ID3D12GraphicsCommandList* cmdList, const CSTask& taskDesc)
{
    cmdList->SetComputeRootSignature(GetRootSignature());
    UploadRootParamData(cmdList);
    cmdList->SetPipelineState(Engine::GetEngine()->GetPSOManager2()->GetComputePSO(taskDesc.PSOName));

    cmdList->Dispatch(taskDesc.GroupSize.X, taskDesc.GroupSize.Y, taskDesc.GroupSize.Z);
}

void D3DComputeShaderWrapper::UploadRootParamData(ID3D12GraphicsCommandList* cmdList)
{
    auto renderer = Engine::GetEngine()->GetRenderer();
    auto device = renderer->GetDevice();
    for (auto& itParam : mRootParams)
    {
        auto& rootParam = itParam.second;
        switch (rootParam.resDimension)
        {
        case Resource_Dimension::BUFFER:
        {
            if (rootParam.paramType == ROOT_PARAMETER_TYPE::SRV)
            {
                rootParam.resource = D3DUtils::CreateDefaultBuffer
                (
                    device, cmdList, 
                    rootParam.bufferIn.data(), rootParam.bufferIn.size(), 
                    rootParam.uploadResource
                );
                cmdList->SetComputeRootShaderResourceView
                (
                    rootParam.paramIndex, rootParam.resource->GetGPUVirtualAddress()
                );
            }
            else if (rootParam.paramType == ROOT_PARAMETER_TYPE::UAV)
            {
                rootParam.resource = D3DUtils::CreateDefaultBufferUAV
                (
                    device, cmdList, rootParam.bufferOutLen
                );
                cmdList->SetComputeRootUnorderedAccessView
                (
                    rootParam.paramIndex, rootParam.resource->GetGPUVirtualAddress()
                );
            }
        }break;
        case Resource_Dimension::TEXTURE1D:
        {

        }break;
        case Resource_Dimension::TEXTURE2D:
        {

        }break;
        case Resource_Dimension::TEXTURE3D:
        {

        }break;
        case Resource_Dimension::UNKNOWN:
        {

        }break;
        default:
            throw std::runtime_error("unknown resource dimension");
        }
    }
}

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
        mRootParams[rootParam.name] = { (UINT)i,paramType,nullptr,nullptr };
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
        Microsoft::WRL::ComPtr<ID3DBlob> byteCode = CompileShaderStage(shaderStage);
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