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
    static Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBufferUAV(
        ID3D12Device* device,
        ID3D12GraphicsCommandList* cmdList,
        UINT64 byteSize
    );
    static Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBufferUAV(
        ID3D12Device* device,
        ID3D12GraphicsCommandList* cmdList,
        const void* initData,
        UINT64 byteSize,
        Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer
    );
    static Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultTexture2D(
        ID3D12Device* device,
        ID3D12GraphicsCommandList* cmdList,
        const void* initData, DXGI_FORMAT format,
        UINT64 width, UINT height,
        Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer
    );
    static Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(
        const std::wstring& filename,
        const D3D_SHADER_MACRO* defines,
        const std::string& entrypoint,
        const std::string& target);
    static Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(
        const std::string& shaderdata,
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

    static size_t BitsPerPixel(DXGI_FORMAT fmt);

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

inline size_t D3DUtils::BitsPerPixel(DXGI_FORMAT fmt)
{
    switch (static_cast<int>(fmt))
    {
    case DXGI_FORMAT_R32G32B32A32_TYPELESS:
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
    case DXGI_FORMAT_R32G32B32A32_UINT:
    case DXGI_FORMAT_R32G32B32A32_SINT:
        return 128;

    case DXGI_FORMAT_R32G32B32_TYPELESS:
    case DXGI_FORMAT_R32G32B32_FLOAT:
    case DXGI_FORMAT_R32G32B32_UINT:
    case DXGI_FORMAT_R32G32B32_SINT:
        return 96;

    case DXGI_FORMAT_R16G16B16A16_TYPELESS:
    case DXGI_FORMAT_R16G16B16A16_FLOAT:
    case DXGI_FORMAT_R16G16B16A16_UNORM:
    case DXGI_FORMAT_R16G16B16A16_UINT:
    case DXGI_FORMAT_R16G16B16A16_SNORM:
    case DXGI_FORMAT_R16G16B16A16_SINT:
    case DXGI_FORMAT_R32G32_TYPELESS:
    case DXGI_FORMAT_R32G32_FLOAT:
    case DXGI_FORMAT_R32G32_UINT:
    case DXGI_FORMAT_R32G32_SINT:
    case DXGI_FORMAT_R32G8X24_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
    case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
    case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
    case DXGI_FORMAT_Y416:
    case DXGI_FORMAT_Y210:
    case DXGI_FORMAT_Y216:
        return 64;

    case DXGI_FORMAT_R10G10B10A2_TYPELESS:
    case DXGI_FORMAT_R10G10B10A2_UNORM:
    case DXGI_FORMAT_R10G10B10A2_UINT:
    case DXGI_FORMAT_R11G11B10_FLOAT:
    case DXGI_FORMAT_R8G8B8A8_TYPELESS:
    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
    case DXGI_FORMAT_R8G8B8A8_UINT:
    case DXGI_FORMAT_R8G8B8A8_SNORM:
    case DXGI_FORMAT_R8G8B8A8_SINT:
    case DXGI_FORMAT_R16G16_TYPELESS:
    case DXGI_FORMAT_R16G16_FLOAT:
    case DXGI_FORMAT_R16G16_UNORM:
    case DXGI_FORMAT_R16G16_UINT:
    case DXGI_FORMAT_R16G16_SNORM:
    case DXGI_FORMAT_R16G16_SINT:
    case DXGI_FORMAT_R32_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT:
    case DXGI_FORMAT_R32_FLOAT:
    case DXGI_FORMAT_R32_UINT:
    case DXGI_FORMAT_R32_SINT:
    case DXGI_FORMAT_R24G8_TYPELESS:
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
    case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
    case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
    case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
    case DXGI_FORMAT_R8G8_B8G8_UNORM:
    case DXGI_FORMAT_G8R8_G8B8_UNORM:
    case DXGI_FORMAT_B8G8R8A8_UNORM:
    case DXGI_FORMAT_B8G8R8X8_UNORM:
    case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
    case DXGI_FORMAT_B8G8R8A8_TYPELESS:
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8X8_TYPELESS:
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
    case DXGI_FORMAT_AYUV:
    case DXGI_FORMAT_Y410:
    case DXGI_FORMAT_YUY2:
    //case XBOX_DXGI_FORMAT_R10G10B10_7E3_A2_FLOAT:
    //case XBOX_DXGI_FORMAT_R10G10B10_6E4_A2_FLOAT:
    //case XBOX_DXGI_FORMAT_R10G10B10_SNORM_A2_UNORM:
        return 32;

    case DXGI_FORMAT_P010:
    case DXGI_FORMAT_P016:
    //case XBOX_DXGI_FORMAT_D16_UNORM_S8_UINT:
    //case XBOX_DXGI_FORMAT_R16_UNORM_X8_TYPELESS:
    //case XBOX_DXGI_FORMAT_X16_TYPELESS_G8_UINT:
    //case WIN10_DXGI_FORMAT_V408:
        return 24;

    case DXGI_FORMAT_R8G8_TYPELESS:
    case DXGI_FORMAT_R8G8_UNORM:
    case DXGI_FORMAT_R8G8_UINT:
    case DXGI_FORMAT_R8G8_SNORM:
    case DXGI_FORMAT_R8G8_SINT:
    case DXGI_FORMAT_R16_TYPELESS:
    case DXGI_FORMAT_R16_FLOAT:
    case DXGI_FORMAT_D16_UNORM:
    case DXGI_FORMAT_R16_UNORM:
    case DXGI_FORMAT_R16_UINT:
    case DXGI_FORMAT_R16_SNORM:
    case DXGI_FORMAT_R16_SINT:
    case DXGI_FORMAT_B5G6R5_UNORM:
    case DXGI_FORMAT_B5G5R5A1_UNORM:
    case DXGI_FORMAT_A8P8:
    case DXGI_FORMAT_B4G4R4A4_UNORM:
    //case WIN10_DXGI_FORMAT_P208:
    //case WIN10_DXGI_FORMAT_V208:
        return 16;

    case DXGI_FORMAT_NV12:
    case DXGI_FORMAT_420_OPAQUE:
    case DXGI_FORMAT_NV11:
        return 12;

    case DXGI_FORMAT_R8_TYPELESS:
    case DXGI_FORMAT_R8_UNORM:
    case DXGI_FORMAT_R8_UINT:
    case DXGI_FORMAT_R8_SNORM:
    case DXGI_FORMAT_R8_SINT:
    case DXGI_FORMAT_A8_UNORM:
    case DXGI_FORMAT_AI44:
    case DXGI_FORMAT_IA44:
    case DXGI_FORMAT_P8:
    //case XBOX_DXGI_FORMAT_R4G4_UNORM:
        return 8;

    case DXGI_FORMAT_R1_UNORM:
        return 1;

    case DXGI_FORMAT_BC1_TYPELESS:
    case DXGI_FORMAT_BC1_UNORM:
    case DXGI_FORMAT_BC1_UNORM_SRGB:
    case DXGI_FORMAT_BC4_TYPELESS:
    case DXGI_FORMAT_BC4_UNORM:
    case DXGI_FORMAT_BC4_SNORM:
        return 4;

    case DXGI_FORMAT_BC2_TYPELESS:
    case DXGI_FORMAT_BC2_UNORM:
    case DXGI_FORMAT_BC2_UNORM_SRGB:
    case DXGI_FORMAT_BC3_TYPELESS:
    case DXGI_FORMAT_BC3_UNORM:
    case DXGI_FORMAT_BC3_UNORM_SRGB:
    case DXGI_FORMAT_BC5_TYPELESS:
    case DXGI_FORMAT_BC5_UNORM:
    case DXGI_FORMAT_BC5_SNORM:
    case DXGI_FORMAT_BC6H_TYPELESS:
    case DXGI_FORMAT_BC6H_UF16:
    case DXGI_FORMAT_BC6H_SF16:
    case DXGI_FORMAT_BC7_TYPELESS:
    case DXGI_FORMAT_BC7_UNORM:
    case DXGI_FORMAT_BC7_UNORM_SRGB:
        return 8;

    default:
        return 0;
    }
}

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
        throw std::exception("unknown FLVertexFormatByteSize");
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
    case FireFlame::Primitive_Topology::CONTROL_POINT_PATCHLIST_1:
        return D3D_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST;
    case FireFlame::Primitive_Topology::CONTROL_POINT_PATCHLIST_2:
        return D3D_PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST;
    case FireFlame::Primitive_Topology::CONTROL_POINT_PATCHLIST_3:
        return D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;
    case FireFlame::Primitive_Topology::CONTROL_POINT_PATCHLIST_4:
        return D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST;
    case FireFlame::Primitive_Topology::CONTROL_POINT_PATCHLIST_5:
        return D3D_PRIMITIVE_TOPOLOGY_5_CONTROL_POINT_PATCHLIST;
    case FireFlame::Primitive_Topology::CONTROL_POINT_PATCHLIST_6:
        return D3D_PRIMITIVE_TOPOLOGY_6_CONTROL_POINT_PATCHLIST;
    case FireFlame::Primitive_Topology::CONTROL_POINT_PATCHLIST_7:
        return D3D_PRIMITIVE_TOPOLOGY_7_CONTROL_POINT_PATCHLIST;
    case FireFlame::Primitive_Topology::CONTROL_POINT_PATCHLIST_8:
        return D3D_PRIMITIVE_TOPOLOGY_8_CONTROL_POINT_PATCHLIST;
    case FireFlame::Primitive_Topology::CONTROL_POINT_PATCHLIST_9:
        return D3D_PRIMITIVE_TOPOLOGY_9_CONTROL_POINT_PATCHLIST;
    case FireFlame::Primitive_Topology::CONTROL_POINT_PATCHLIST_10:
        return D3D_PRIMITIVE_TOPOLOGY_10_CONTROL_POINT_PATCHLIST;
    case FireFlame::Primitive_Topology::CONTROL_POINT_PATCHLIST_11:
        return D3D_PRIMITIVE_TOPOLOGY_11_CONTROL_POINT_PATCHLIST;
    case FireFlame::Primitive_Topology::CONTROL_POINT_PATCHLIST_12:
        return D3D_PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST;
    case FireFlame::Primitive_Topology::CONTROL_POINT_PATCHLIST_13:
        return D3D_PRIMITIVE_TOPOLOGY_13_CONTROL_POINT_PATCHLIST;
    case FireFlame::Primitive_Topology::CONTROL_POINT_PATCHLIST_14:
        return D3D_PRIMITIVE_TOPOLOGY_14_CONTROL_POINT_PATCHLIST;
    case FireFlame::Primitive_Topology::CONTROL_POINT_PATCHLIST_15:
        return D3D_PRIMITIVE_TOPOLOGY_15_CONTROL_POINT_PATCHLIST;
    case FireFlame::Primitive_Topology::CONTROL_POINT_PATCHLIST_16:
        return D3D_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST;
    case FireFlame::Primitive_Topology::CONTROL_POINT_PATCHLIST_17:
        return D3D_PRIMITIVE_TOPOLOGY_17_CONTROL_POINT_PATCHLIST;
    case FireFlame::Primitive_Topology::CONTROL_POINT_PATCHLIST_18:
        return D3D_PRIMITIVE_TOPOLOGY_18_CONTROL_POINT_PATCHLIST;
    case FireFlame::Primitive_Topology::CONTROL_POINT_PATCHLIST_19:
        return D3D_PRIMITIVE_TOPOLOGY_19_CONTROL_POINT_PATCHLIST;
    case FireFlame::Primitive_Topology::CONTROL_POINT_PATCHLIST_20:
        return D3D_PRIMITIVE_TOPOLOGY_20_CONTROL_POINT_PATCHLIST;
    case FireFlame::Primitive_Topology::CONTROL_POINT_PATCHLIST_21:
        return D3D_PRIMITIVE_TOPOLOGY_21_CONTROL_POINT_PATCHLIST;
    case FireFlame::Primitive_Topology::CONTROL_POINT_PATCHLIST_22:
        return D3D_PRIMITIVE_TOPOLOGY_22_CONTROL_POINT_PATCHLIST;
    case FireFlame::Primitive_Topology::CONTROL_POINT_PATCHLIST_23:
        return D3D_PRIMITIVE_TOPOLOGY_23_CONTROL_POINT_PATCHLIST;
    case FireFlame::Primitive_Topology::CONTROL_POINT_PATCHLIST_24:
        return D3D_PRIMITIVE_TOPOLOGY_24_CONTROL_POINT_PATCHLIST;
    case FireFlame::Primitive_Topology::CONTROL_POINT_PATCHLIST_25:
        return D3D_PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST;
    case FireFlame::Primitive_Topology::CONTROL_POINT_PATCHLIST_26:
        return D3D_PRIMITIVE_TOPOLOGY_26_CONTROL_POINT_PATCHLIST;
    case FireFlame::Primitive_Topology::CONTROL_POINT_PATCHLIST_27:
        return D3D_PRIMITIVE_TOPOLOGY_27_CONTROL_POINT_PATCHLIST;
    case FireFlame::Primitive_Topology::CONTROL_POINT_PATCHLIST_28:
        return D3D_PRIMITIVE_TOPOLOGY_28_CONTROL_POINT_PATCHLIST;
    case FireFlame::Primitive_Topology::CONTROL_POINT_PATCHLIST_29:
        return D3D_PRIMITIVE_TOPOLOGY_29_CONTROL_POINT_PATCHLIST;
    case FireFlame::Primitive_Topology::CONTROL_POINT_PATCHLIST_30:
        return D3D_PRIMITIVE_TOPOLOGY_30_CONTROL_POINT_PATCHLIST;
    case FireFlame::Primitive_Topology::CONTROL_POINT_PATCHLIST_31:
        return D3D_PRIMITIVE_TOPOLOGY_31_CONTROL_POINT_PATCHLIST;
    case FireFlame::Primitive_Topology::CONTROL_POINT_PATCHLIST_32:
        return D3D_PRIMITIVE_TOPOLOGY_32_CONTROL_POINT_PATCHLIST;
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
    case FireFlame::Primitive_Topology_Type::Undefined:
        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
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

inline std::uint8_t FLColorWriteMask2D3DColorWriteMask(std::uint8_t flMask) {
    switch (flMask)
    {
    case (std::uint8_t)FireFlame::COLOR_WRITE_ENABLE::RED:
        return D3D12_COLOR_WRITE_ENABLE_RED;
    case (std::uint8_t)FireFlame::COLOR_WRITE_ENABLE::GREEN:
        return D3D12_COLOR_WRITE_ENABLE_GREEN;
    case (std::uint8_t)FireFlame::COLOR_WRITE_ENABLE::BLUE:
        return D3D12_COLOR_WRITE_ENABLE_BLUE;
    case (std::uint8_t)FireFlame::COLOR_WRITE_ENABLE::ALPHA:
        return D3D12_COLOR_WRITE_ENABLE_ALPHA;
    case (std::uint8_t)FireFlame::COLOR_WRITE_ENABLE::ALL:
        return D3D12_COLOR_WRITE_ENABLE_ALL;
    default:
        return 0;
    }
}

inline D3D12_DEPTH_WRITE_MASK FLDepthWriteMask2D3DDepthWriteMask(std::uint8_t mask) {
    switch (mask)
    {
    case 0:
        return D3D12_DEPTH_WRITE_MASK_ZERO;
    case 1:
        return D3D12_DEPTH_WRITE_MASK_ALL;
    default:
        return D3D12_DEPTH_WRITE_MASK_ALL;
    }
}

inline D3D12_STENCIL_OP FLStencilOp2D3DStencilOp(STENCIL_OP op)
{
    switch (op)
    {
    case FireFlame::STENCIL_OP::KEEP:
        return D3D12_STENCIL_OP_KEEP;
    case FireFlame::STENCIL_OP::ZERO:
        return D3D12_STENCIL_OP_ZERO;
    case FireFlame::STENCIL_OP::REPLACE:
        return D3D12_STENCIL_OP_REPLACE;
    case FireFlame::STENCIL_OP::INCR_SAT:
        return D3D12_STENCIL_OP_INCR_SAT;
    case FireFlame::STENCIL_OP::DECR_SAT:
        return D3D12_STENCIL_OP_DECR_SAT;
    case FireFlame::STENCIL_OP::INVERT:
        return D3D12_STENCIL_OP_INVERT;
    case FireFlame::STENCIL_OP::INCR:
        return D3D12_STENCIL_OP_INCR;
    case FireFlame::STENCIL_OP::DECR:
        return D3D12_STENCIL_OP_DECR;
    default:
        throw std::exception("unknown op in FLStencilOp2D3DStencilOp");
    }
}

inline D3D12_BLEND_OP FLBlendOp2D3DBlendOp(BLEND_OP op)
{
    switch (op)
    {
    case FireFlame::BLEND_OP::ADD:
        return D3D12_BLEND_OP_ADD;
    case FireFlame::BLEND_OP::SUBTRACT:
        return D3D12_BLEND_OP_SUBTRACT;
    case FireFlame::BLEND_OP::REV_SUBTRACT:
        return D3D12_BLEND_OP_REV_SUBTRACT;
    case FireFlame::BLEND_OP::MIN:
        return D3D12_BLEND_OP_MIN;
    case FireFlame::BLEND_OP::MAX:
        return D3D12_BLEND_OP_MAX;
    default:
        throw std::exception("unknown op in FLBlendOp2D3DBlendOp");
    }
}

inline D3D12_BLEND FLBlend2D3DBlend(BLEND blend)
{
    switch (blend)
    {
    case FireFlame::BLEND::ZERO:
        return D3D12_BLEND_ZERO;
    case FireFlame::BLEND::ONE:
        return D3D12_BLEND_ONE;
    case FireFlame::BLEND::SRC_COLOR:
        return D3D12_BLEND_SRC_COLOR;
    case FireFlame::BLEND::INV_SRC_COLOR:
        return D3D12_BLEND_INV_SRC_COLOR;
    case FireFlame::BLEND::SRC_ALPHA:
        return D3D12_BLEND_SRC_ALPHA;
    case FireFlame::BLEND::INV_SRC_ALPHA:
        return D3D12_BLEND_INV_SRC_ALPHA;
    case FireFlame::BLEND::DEST_ALPHA:
        return D3D12_BLEND_DEST_ALPHA;
    case FireFlame::BLEND::INV_DEST_ALPHA:
        return D3D12_BLEND_INV_DEST_ALPHA;
    case FireFlame::BLEND::DEST_COLOR:
        return D3D12_BLEND_DEST_COLOR;
    case FireFlame::BLEND::INV_DEST_COLOR:
        return D3D12_BLEND_INV_DEST_COLOR;
    case FireFlame::BLEND::SRC_ALPHA_SAT:
        return D3D12_BLEND_SRC_ALPHA_SAT;
    case FireFlame::BLEND::BLEND_FACTOR:
        return D3D12_BLEND_BLEND_FACTOR;
    case FireFlame::BLEND::INV_BLEND_FACTOR:
        return D3D12_BLEND_INV_BLEND_FACTOR;
    case FireFlame::BLEND::SRC1_COLOR:
        return D3D12_BLEND_SRC1_COLOR;
    case FireFlame::BLEND::INV_SRC1_COLOR:
        return D3D12_BLEND_INV_SRC1_COLOR;
    case FireFlame::BLEND::SRC1_ALPHA:
        return D3D12_BLEND_SRC1_ALPHA;
    case FireFlame::BLEND::INV_SRC1_ALPHA:
        return D3D12_BLEND_INV_SRC1_ALPHA;
    default:
        throw std::exception("unknown op in FLBlend2D3DBlend");
    }
}

inline D3D12_COMPARISON_FUNC FLCompareFunc2D3DCompareFunc(COMPARISON_FUNC func)
{
    switch (func)
    {
    case FireFlame::COMPARISON_FUNC::NEVER:
        return D3D12_COMPARISON_FUNC_NEVER;
    case FireFlame::COMPARISON_FUNC::LESS:
        return D3D12_COMPARISON_FUNC_LESS;
    case FireFlame::COMPARISON_FUNC::EQUAL:
        return D3D12_COMPARISON_FUNC_EQUAL;
    case FireFlame::COMPARISON_FUNC::LESS_EQUAL:
        return D3D12_COMPARISON_FUNC_LESS_EQUAL;
    case FireFlame::COMPARISON_FUNC::GREATER:
        return D3D12_COMPARISON_FUNC_GREATER;
    case FireFlame::COMPARISON_FUNC::NOT_EQUAL:
        return D3D12_COMPARISON_FUNC_NOT_EQUAL;
    case FireFlame::COMPARISON_FUNC::GREATER_EQUAL:
        return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
    case FireFlame::COMPARISON_FUNC::ALWAYS:
        return D3D12_COMPARISON_FUNC_ALWAYS;
    default:
        throw std::exception("unknown func in FLCompareFunc2D3DCompareFunc");
    }
}

inline D3D12_SRV_DIMENSION FLSRVDim2D3DSRVDim(SRV_DIMENSION dim)
{
    switch (dim)
    {
    case FireFlame::SRV_DIMENSION::UNKNOWN:
        return D3D12_SRV_DIMENSION_UNKNOWN;
    case FireFlame::SRV_DIMENSION::BUFFER:
        return D3D12_SRV_DIMENSION_BUFFER;
    case FireFlame::SRV_DIMENSION::TEXTURE1D:
        return D3D12_SRV_DIMENSION_TEXTURE1D;
    case FireFlame::SRV_DIMENSION::TEXTURE1DARRAY:
        return D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
    case FireFlame::SRV_DIMENSION::TEXTURE2D:
        return D3D12_SRV_DIMENSION_TEXTURE2D;
    case FireFlame::SRV_DIMENSION::TEXTURE2DARRAY:
        return D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
    case FireFlame::SRV_DIMENSION::TEXTURE2DMS:
        return D3D12_SRV_DIMENSION_TEXTURE2DMS;
    case FireFlame::SRV_DIMENSION::TEXTURE2DMSARRAY:
        return D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY;
    case FireFlame::SRV_DIMENSION::TEXTURE3D:
        return D3D12_SRV_DIMENSION_TEXTURE3D;
    case FireFlame::SRV_DIMENSION::TEXTURECUBE:
        return D3D12_SRV_DIMENSION_TEXTURECUBE;
    case FireFlame::SRV_DIMENSION::TEXTURECUBEARRAY:
        return D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
    default:
        throw std::exception("unknown SRV dimension in FLSRVDim2D3DSRVDim");
    }
}

inline D3D12_DESCRIPTOR_RANGE_TYPE FLDesRangeType2D3DDesRangeType(DESCRIPTOR_RANGE_TYPE type)
{
    switch (type)
    {
    case FireFlame::DESCRIPTOR_RANGE_TYPE::SRV:
        return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    case FireFlame::DESCRIPTOR_RANGE_TYPE::UAV:
        return D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
    case FireFlame::DESCRIPTOR_RANGE_TYPE::CBV:
        return D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
    case FireFlame::DESCRIPTOR_RANGE_TYPE::SAMPLER:
        return D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
    default:
        throw std::exception("unknown DESCRIPTOR_RANGE_TYPE in FLDesRangeType2D3DDesRangeType");
    }
}

inline D3D12_SHADER_VISIBILITY FLShaderVisiblity2D3DShaderVisiblity(SHADER_VISIBILITY visibility)
{
    switch (visibility)
    {
    case FireFlame::SHADER_VISIBILITY::VISIBILITY_ALL:
        return D3D12_SHADER_VISIBILITY_ALL;
    case FireFlame::SHADER_VISIBILITY::VISIBILITY_VERTEX:
        return D3D12_SHADER_VISIBILITY_VERTEX;
    case FireFlame::SHADER_VISIBILITY::VISIBILITY_HULL:
        return D3D12_SHADER_VISIBILITY_HULL;
    case FireFlame::SHADER_VISIBILITY::VISIBILITY_DOMAIN:
        return D3D12_SHADER_VISIBILITY_DOMAIN;
    case FireFlame::SHADER_VISIBILITY::VISIBILITY_GEOMETRY:
        return D3D12_SHADER_VISIBILITY_GEOMETRY;
    case FireFlame::SHADER_VISIBILITY::VISIBILITY_PIXEL:
        return D3D12_SHADER_VISIBILITY_PIXEL;
    default:
        throw std::exception("unknown SHADER_VISIBILITY in FLShaderVisiblity2D3DShaderVisiblity");
    }
}

inline std::wstring AnsiToWString(const std::string& str)
{
	WCHAR buffer[2048];
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 2048);
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