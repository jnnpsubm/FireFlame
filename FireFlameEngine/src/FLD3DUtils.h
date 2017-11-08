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

inline D3D12_CULL_MODE FLCullMode2D3DCullMode(Cull_Mode mode) {
    switch (mode)
    {
    case FireFlame::Cull_Mode::None:
        return D3D12_CULL_MODE_NONE;
    case FireFlame::Cull_Mode::Front:
        return D3D12_CULL_MODE_FRONT;
    case FireFlame::Cull_Mode::Back:
        return D3D12_CULL_MODE_BACK;
    default:
        throw std::exception("unknown FLCullMode2D3DCullMode");
    }
}
inline Cull_Mode D3DCullMode2FLCullMode(D3D12_CULL_MODE mode) {
    switch (mode)
    {
    case D3D12_CULL_MODE_NONE:
        return FireFlame::Cull_Mode::None;
    case D3D12_CULL_MODE_FRONT:
        return FireFlame::Cull_Mode::Front;
    case D3D12_CULL_MODE_BACK:
        return FireFlame::Cull_Mode::Back;
    default:
        throw std::exception("unknown D3DCullMode2FLCullMode");
    }
}
inline D3D12_FILL_MODE FLFillMode2D3DFillMode(Fill_Mode mode) {
    switch (mode)
    {
    case FireFlame::Fill_Mode::Wireframe:
        return D3D12_FILL_MODE_WIREFRAME;
    case FireFlame::Fill_Mode::Solid:
        return D3D12_FILL_MODE_SOLID;
    default:
        throw std::exception("unknown FLFillMode2D3DFillMode");
    }
}
inline Fill_Mode D3DFillMode2FLFillMode(D3D12_FILL_MODE mode) {
    switch (mode)
    {
    case D3D12_FILL_MODE_WIREFRAME:
        return FireFlame::Fill_Mode::Wireframe;
    case D3D12_FILL_MODE_SOLID:
        return FireFlame::Fill_Mode::Solid;
    default:
        throw std::exception("unknown FLFillMode2D3DFillMode");
    }
}

inline DXGI_FORMAT FLIndexFormat2DXGIFormat(Index_Format format) {
	switch (format)
	{
	case FireFlame::Index_Format::UINT16:
		return DXGI_FORMAT_R16_UINT;
	case FireFlame::Index_Format::UINT32:
		return DXGI_FORMAT_R32_UINT;
	default:
		throw std::exception("unknown FLIndexFormat2DXGIFormat");
	}
}
inline DXGI_FORMAT FLVertexFormat2DXGIFormat(unsigned long format) {
    switch (format)
    {
    case VERTEX_FORMAT_FLOAT1:
        return DXGI_FORMAT_R32_FLOAT;
    case VERTEX_FORMAT_FLOAT2:
        return DXGI_FORMAT_R32G32_FLOAT;
    case VERTEX_FORMAT_FLOAT3:
        return DXGI_FORMAT_R32G32B32_FLOAT;
    case VERTEX_FORMAT_FLOAT4:
        return DXGI_FORMAT_R32G32B32A32_FLOAT;
    case VERTEX_FORMAT_A8B8G8R8_UNORM:
        return DXGI_FORMAT_R8G8B8A8_UNORM;
    case VERTEX_FORMAT_A8R8G8B8_UNORM:
        return DXGI_FORMAT_B8G8R8A8_UNORM;
    default:
        return DXGI_FORMAT_UNKNOWN;
    }
}
inline UINT FLVertexFormatByteSize(unsigned long format) {
    switch (format)
    {
    case VERTEX_FORMAT_A8B8G8R8_UNORM:
    case VERTEX_FORMAT_A8R8G8B8_UNORM:
    case VERTEX_FORMAT_FLOAT1:
        return 4;
    case VERTEX_FORMAT_FLOAT2:
        return 8;
    case VERTEX_FORMAT_FLOAT3:
        return 12;
    case VERTEX_FORMAT_FLOAT4:
        return 16;
    default:
        throw std::exception("unknonw FLVertexFormatByteSize");
    }
}
inline D3D12_PRIMITIVE_TOPOLOGY FLPrimitiveTop2D3DPrimitiveTop(Primitive_Topology top) {
    switch (top)
    {
    case FireFlame::Primitive_Topology::PointList:
        return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
    case FireFlame::Primitive_Topology::LineStrip:
        return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
    case FireFlame::Primitive_Topology::LineList:
        return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
    case FireFlame::Primitive_Topology::TriangleList:
        return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    case FireFlame::Primitive_Topology::TriangleStrip:
        return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
    default:
        throw std::exception("unknown primitive topology");
    }
}
inline D3D12_PRIMITIVE_TOPOLOGY_TYPE D3DPrimitiveType(D3D12_PRIMITIVE_TOPOLOGY top) {
    switch (top)
    {
    case D3D_PRIMITIVE_TOPOLOGY_POINTLIST:
        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
    case D3D_PRIMITIVE_TOPOLOGY_LINELIST:
    case D3D_PRIMITIVE_TOPOLOGY_LINESTRIP:
    case D3D_PRIMITIVE_TOPOLOGY_LINELIST_ADJ:
    case D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ:
        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
    case D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
    case D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
    case D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ:
    case D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ:
        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    case D3D_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST:
    case D3D_PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST:
    case D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST:
    case D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST:
    case D3D_PRIMITIVE_TOPOLOGY_5_CONTROL_POINT_PATCHLIST:
    case D3D_PRIMITIVE_TOPOLOGY_6_CONTROL_POINT_PATCHLIST:
    case D3D_PRIMITIVE_TOPOLOGY_7_CONTROL_POINT_PATCHLIST:
    case D3D_PRIMITIVE_TOPOLOGY_8_CONTROL_POINT_PATCHLIST:
    case D3D_PRIMITIVE_TOPOLOGY_9_CONTROL_POINT_PATCHLIST:
    case D3D_PRIMITIVE_TOPOLOGY_10_CONTROL_POINT_PATCHLIST:
    case D3D_PRIMITIVE_TOPOLOGY_11_CONTROL_POINT_PATCHLIST:
    case D3D_PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST:
    case D3D_PRIMITIVE_TOPOLOGY_13_CONTROL_POINT_PATCHLIST:
    case D3D_PRIMITIVE_TOPOLOGY_14_CONTROL_POINT_PATCHLIST:
    case D3D_PRIMITIVE_TOPOLOGY_15_CONTROL_POINT_PATCHLIST:
    case D3D_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST:
    case D3D_PRIMITIVE_TOPOLOGY_17_CONTROL_POINT_PATCHLIST:
    case D3D_PRIMITIVE_TOPOLOGY_18_CONTROL_POINT_PATCHLIST:
    case D3D_PRIMITIVE_TOPOLOGY_19_CONTROL_POINT_PATCHLIST:
    case D3D_PRIMITIVE_TOPOLOGY_20_CONTROL_POINT_PATCHLIST:
    case D3D_PRIMITIVE_TOPOLOGY_21_CONTROL_POINT_PATCHLIST:
    case D3D_PRIMITIVE_TOPOLOGY_22_CONTROL_POINT_PATCHLIST:
    case D3D_PRIMITIVE_TOPOLOGY_23_CONTROL_POINT_PATCHLIST:
    case D3D_PRIMITIVE_TOPOLOGY_24_CONTROL_POINT_PATCHLIST:
    case D3D_PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST:
    case D3D_PRIMITIVE_TOPOLOGY_26_CONTROL_POINT_PATCHLIST:
    case D3D_PRIMITIVE_TOPOLOGY_27_CONTROL_POINT_PATCHLIST:
    case D3D_PRIMITIVE_TOPOLOGY_28_CONTROL_POINT_PATCHLIST:
    case D3D_PRIMITIVE_TOPOLOGY_29_CONTROL_POINT_PATCHLIST:
    case D3D_PRIMITIVE_TOPOLOGY_30_CONTROL_POINT_PATCHLIST:
    case D3D_PRIMITIVE_TOPOLOGY_31_CONTROL_POINT_PATCHLIST:
    case D3D_PRIMITIVE_TOPOLOGY_32_CONTROL_POINT_PATCHLIST:
        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
    default:
        throw std::exception("unknown D3DPrimitiveType");
    }
}
inline D3D12_PRIMITIVE_TOPOLOGY_TYPE FLPrimitiveType2D3DPrimitiveType(Primitive_Topology_Type type) {
    switch (type)
    {
    case FireFlame::Primitive_Topology_Type::Point:
        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
    case FireFlame::Primitive_Topology_Type::Line:
        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
    case FireFlame::Primitive_Topology_Type::Triangle:
        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    case FireFlame::Primitive_Topology_Type::Patch:
        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
    default:
        throw std::exception("unknown primitive topology type");
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