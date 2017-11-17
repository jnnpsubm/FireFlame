#include "FLD3DRenderItem.h"
#include "..\..\Engine\FLEngine.h"
#include "..\..\Renderer\FLD3DRenderer.h"
#include "..\..\PSOManager\FLD3DPSOManager.h"
#include "..\..\Material\FLMaterial.h"

namespace FireFlame {
D3DRenderItem::~D3DRenderItem() {
    delete [] Data;
    Data = nullptr;
    DataLen = 0;
}
void D3DRenderItem::Render(D3DShaderWrapper* Shader) {
    auto renderer = Engine::GetEngine()->GetRenderer();
    ID3D12GraphicsCommandList* cmdList = renderer->GetCommandList();
    auto CBVHeap = Shader->GetCBVHeap();

    if (Mat && Shader->GetMaterialRegister() != (UINT)-1)
    {
        int matCbvIndex = Mat->MatCBIndex;
        matCbvIndex += Shader->GetMatCBVOffset();
        matCbvIndex += renderer->GetCurrFrameResIndex() * Shader->GetMaterialCBVMaxCount();
        auto matCbvHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(CBVHeap->GetGPUDescriptorHandleForHeapStart());
        matCbvHandle.Offset(matCbvIndex, renderer->GetCbvSrvUavDescriptorSize());
        cmdList->SetGraphicsRootDescriptorTable(Shader->GetMaterialRegister(), matCbvHandle);
    }

    if (Mesh->VertexBufferInFrameRes())
    {
        auto VB = renderer->GetCurrFrameResource()->VBResources[Mesh->GetName()].get();
        Mesh->SetVertexBuffer(0, VB->Resource());
    }
    std::vector<D3D12_VERTEX_BUFFER_VIEW> vecVBV = Mesh->VertexBufferViews();
    cmdList->IASetVertexBuffers(0, (UINT)vecVBV.size(), &vecVBV[0]);
    cmdList->IASetIndexBuffer(&Mesh->IndexBufferView());
    cmdList->IASetPrimitiveTopology(PrimitiveType);

    // Offset to the CBV in the descriptor heap for this object and for this frame resource.
    UINT cbvIndex = renderer->GetCurrFrameResIndex()*Shader->GetObjCBVMaxCount() + ObjCBIndex;
    auto cbvHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(Shader->GetCBVHeap()->GetGPUDescriptorHandleForHeapStart());
    cbvHandle.Offset(cbvIndex, renderer->GetCbvSrvUavDescriptorSize());
    cmdList->SetGraphicsRootDescriptorTable(0, cbvHandle);

    cmdList->DrawIndexedInstanced
    (
        IndexCount, 1,
        StartIndexLocation,
        BaseVertexLocation, 0
    );
}
}