#pragma once
#include <string>
#include <d3d12.h>

namespace FireFlame {
class D3DMesh;
class D3DShaderWrapper;
class D3DShaderWrapper;
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

    D3DMesh* Mesh            = nullptr;

    std::string Shader;
    void SetShader(const std::string& _Shader) {
        Shader = _Shader;
    }

    // Primitive topology.
    D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    // DrawIndexedInstanced parameters.
    UINT IndexCount = 0;
    UINT StartIndexLocation = 0;
    int BaseVertexLocation = 0;
};
}