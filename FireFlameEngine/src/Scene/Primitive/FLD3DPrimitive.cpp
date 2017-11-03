#include "FLD3DPrimitive.h"
#include "..\..\Renderer\FLRenderer.h"
#include "..\..\ShaderWrapper\FLD3DShaderWrapper.h"

namespace FireFlame {
D3DPrimitive::D3DPrimitive() = default;
D3DPrimitive::D3DPrimitive(const stRawMesh& mesh) : mMesh(std::make_unique<D3DMesh>(mesh)){
	mLocal2World = mesh.LocalToWorld;
}

void D3DPrimitive::Draw(Renderer* renderer) {
    ID3D12GraphicsCommandList* cmdList = renderer->GetCommandList();
    cmdList->SetPipelineState(mShader->GetPSO(renderer->GetMSAAMode()));

    ID3D12DescriptorHeap* CBVHeap = mShader->GetCBVHeap();
    ID3D12DescriptorHeap* descriptorHeaps[] = { CBVHeap };
    cmdList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

    cmdList->SetGraphicsRootSignature(mShader->GetRootSignature());

    cmdList->IASetVertexBuffers(0, 1, &GetMesh()->VertexBufferView());
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
    }
}
}