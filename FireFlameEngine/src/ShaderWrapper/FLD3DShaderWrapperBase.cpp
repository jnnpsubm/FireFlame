#include "FLD3DShaderWrapperBase.h"
#include "..\3rd_utils\d3dx12.h"
#include "..\FLD3DUtils.h"

namespace FireFlame {
D3DShaderWrapperBase::BlobPtr D3DShaderWrapperBase::CompileShaderStage(const stShaderStage& shaderStage)
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

    return byteCode;
}
std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> D3DShaderWrapperBase::GetStaticSamplers()
{
    // Applications usually only need a handful of samplers.  So just define them all up front
    // and keep them available as part of the root signature.  

    const CD3DX12_STATIC_SAMPLER_DESC pointWrap(
        0, // shaderRegister
        D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
        D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

    const CD3DX12_STATIC_SAMPLER_DESC pointClamp(
        1, // shaderRegister
        D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

    const CD3DX12_STATIC_SAMPLER_DESC linearWrap(
        2, // shaderRegister
        D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
        D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

    const CD3DX12_STATIC_SAMPLER_DESC linearClamp(
        3, // shaderRegister
        D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

    const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(
        4, // shaderRegister
        D3D12_FILTER_ANISOTROPIC, // filter
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressW
        0.0f,                             // mipLODBias
        8);                               // maxAnisotropy

    const CD3DX12_STATIC_SAMPLER_DESC anisotropicClamp(
        5, // shaderRegister
        D3D12_FILTER_ANISOTROPIC, // filter
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressW
        0.0f,                              // mipLODBias
        8);                                // maxAnisotropy

    return {
        pointWrap, pointClamp,
        linearWrap, linearClamp,
        anisotropicWrap, anisotropicClamp };
}
} // end FireFlame