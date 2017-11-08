#include "FLD3DPrimitive.h"
#include "..\..\Renderer\FLD3DRenderer.h"
#include "..\..\ShaderWrapper\FLD3DShaderWrapper.h"
#include "..\..\Engine\FLEngine.h"
#include "..\..\PSOManager\FLD3DPSOManager.h"

namespace FireFlame {
D3DPrimitive::D3DPrimitive() = default;
D3DPrimitive::D3DPrimitive(const stRawMesh& mesh) : mMesh(std::make_unique<D3DMesh>(mesh)){
	mLocal2World = mesh.LocalToWorld;
}

void D3DPrimitive::Draw(D3DRenderer* renderer) {
    ID3D12GraphicsCommandList* cmdList = renderer->GetCommandList();
    auto pso = Engine::GetEngine()->GetPSOManager()->GetPSO
    (
        renderer->GetMSAAMode(), 
        GetMesh()->GetPrimitiveTopologyType(),
        renderer->GetCullMode(),
        renderer->GetFillMode()
    );
    cmdList->SetPipelineState(pso);

    ID3D12DescriptorHeap* CBVHeap = mShader->GetCBVHeap();
    ID3D12DescriptorHeap* descriptorHeaps[] = { CBVHeap };
    cmdList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

    cmdList->SetGraphicsRootSignature(mShader->GetRootSignature());

    std::vector<D3D12_VERTEX_BUFFER_VIEW> vecVBV = GetMesh()->VertexBufferViews();
    cmdList->IASetVertexBuffers(0, (UINT)vecVBV.size(), &vecVBV[0]);
    cmdList->IASetIndexBuffer(&GetMesh()->IndexBufferView());
    cmdList->IASetPrimitiveTopology(GetMesh()->GetPrimitiveTopology());

    cmdList->SetGraphicsRootDescriptorTable(0, CBVHeap->GetGPUDescriptorHandleForHeapStart());

    auto subMeshs = GetMesh()->GetSubMeshs();
    for (const auto& namedSubMesh : subMeshs) {
        const auto& subMesh = namedSubMesh.second;
        cmdList->DrawIndexedInstanced
        (
            subMesh.indexCount, 1,
            subMesh.startIndexLocation,
            subMesh.baseVertexLocation, 0
        );
        //cmdList->DrawInstanced()
    }
}
}