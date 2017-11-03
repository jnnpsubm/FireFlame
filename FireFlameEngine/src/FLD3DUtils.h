#pragma once
#include <wrl.h>
#include <DXGI1_5.h>
#include <d3d12.h>
#include <D3Dcompiler.h>
#include "FLTypeDefs.h"
#include "Exception\FLException.h"

namespace FireFlame {
class D3DUtils {
public:
    static Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(
        ID3D12Device* device, 
        ID3D12GraphicsCommandList* cmdList,
        const void* initData,
        UINT64 byteSize,
        Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer
    );
    static Microsoft::WRL::ComPtr<ID3DBlob> D3DUtils::CompileShader(
        const std::wstring& filename,
        const D3D_SHADER_MACRO* defines,
        const std::string& entrypoint,
        const std::string& target);
    static UINT CalcConstantBufferByteSize(UINT byteSize) {
        // Constant buffers must be a multiple of the minimum hardware
        // allocation size (usually 256 bytes).  So round up to nearest
        // multiple of 256.  We do this by adding 255 and then masking off
        // the lower 2 bytes which store all bits < 256.
        return (byteSize + 255) & ~255;
    }
    static D3D_FEATURE_LEVEL FLAPIFeature2D3DFeatureLevel(API_Feature api) {
        switch (api)
        {
        case FireFlame::API_Feature::API_DX12_1:
            return D3D_FEATURE_LEVEL_12_1;
        case FireFlame::API_Feature::API_DX12_0:
            return D3D_FEATURE_LEVEL_12_0;
        case FireFlame::API_Feature::API_DX11_1_On12:
            return D3D_FEATURE_LEVEL_11_1;
        case FireFlame::API_Feature::API_DX11_0_ON12:
            return D3D_FEATURE_LEVEL_11_0;
        case FireFlame::API_Feature::API_OpenGL42:
            throw std::exception("API not supported now......");
            break;
        case FireFlame::API_Feature::API_OpenGLES:
            throw std::exception("API not supported now......");
            break;
        case FireFlame::API_Feature::API_Vulkan:
            throw std::exception("API not supported now......");
            break;
        case FireFlame::API_Feature::API_Metal:
            throw std::exception("API not supported now......");
            break;
        default:
            throw std::exception("API not supported now......");
            break;
        }
    }
};

inline DXGI_FORMAT FLIndexFormat2DXGIFormat(Index_Format format) {
	switch (format)
	{
	case FireFlame::Index_Format::UINT16:
		return DXGI_FORMAT_R16_UINT;
	case FireFlame::Index_Format::UINT32:
		return DXGI_FORMAT_R32_UINT;
	default:
		return DXGI_FORMAT_R16_UINT;
	}
}
inline DXGI_FORMAT FLVertexFormat2DXGIFormat(unsigned long format) {
    switch (format)
    {
    case VERTEX_FORMAT_POS_FLOAT3:
        return DXGI_FORMAT_R32G32B32_FLOAT;
    case VERTEX_FORMAT_COLOR_FLOAT4:
        return DXGI_FORMAT_R32G32B32A32_FLOAT;
    case VERTEX_FORMAT_TEXCOORD_FLOAT2:
        return DXGI_FORMAT_R32G32_FLOAT;
    default:
        return DXGI_FORMAT_UNKNOWN;
    }
}
inline UINT FLVertexFormatByteSize(unsigned long format) {
    switch (format)
    {
    case VERTEX_FORMAT_POS_FLOAT3:
        return 12;
    case VERTEX_FORMAT_COLOR_FLOAT4:
        return 16;
    case VERTEX_FORMAT_TEXCOORD_FLOAT2:
        return 8;
    default:
        return 0;
    }
}
inline D3D12_PRIMITIVE_TOPOLOGY FLPrimitiveTop2D3DPrimitiveTop(Primitive_Topology top) {
    switch (top)
    {
    case FireFlame::Primitive_Topology::TrangleList:
        return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    default:
        throw std::exception("unknown primitive topology");
        break;
    }
}
inline std::wstring AnsiToWString(const std::string& str)
{
	WCHAR buffer[512];
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
	return std::wstring(buffer);
}

#ifndef ThrowIfFailed
#define ThrowIfFailed(x)                                                    \
{                                                                           \
    HRESULT hr__ = (x);                                                     \
    std::wstring file_name = AnsiToWString(__FILE__);                       \
    if(FAILED(hr__)) { throw Exception(hr__, L#x, file_name, __LINE__); }   \
}
#endif
} // end namespace