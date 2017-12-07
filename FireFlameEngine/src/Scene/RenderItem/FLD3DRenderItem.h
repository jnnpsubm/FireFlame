#pragma once
#include <string>
#include <d3d12.h>

namespace FireFlame {
class D3DPrimitive;
class D3DShaderWrapper;
class D3DShaderWrapper;
struct Material;
struct D3DRenderItem {
    D3DRenderItem() = default;
    ~D3DRenderItem();

    void Render(D3DShaderWrapper* Shader);

    std::string Name;

    int NumFramesDirty = 0;

    char* Data = nullptr;
    size_t DataLen = 0;

    // Index into GPU constant buffer corresponding to the ObjectCB for this render item.
    UINT ObjCBIndex = -1;

    UINT MultiObjCBIndex = -1;

    D3DPrimitive* Primitive = nullptr;

    Material* Mat = nullptr;
    void SetMaterial(Material* mat)
    {
        Mat = mat;
    }

    std::string Shader;
    void SetShader(const std::string& _Shader) {
        Shader = _Shader;
    }

    // Primitive topology.
    D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    std::uint32_t stencilRef = -1;

    // DrawIndexedInstanced parameters.
    UINT IndexCount = 0;
    UINT StartIndexLocation = 0;
    int BaseVertexLocation = 0;

    bool opaque = true;
};
}