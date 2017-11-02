#include "FLScene.h"
#include "..\Renderer\FLRenderer.h"
#include "..\ShaderWrapper\FLD3DShaderWrapper.h"
#include "..\Timer\FLStopWatch.h"

namespace FireFlame {
Scene::Scene(std::shared_ptr<Renderer>& renderer) : mRenderer(renderer){}

void Scene::Update(const StopWatch& gt) {
    mUpdateFunc(gt.DeltaTime());
}
void Scene::Render(const StopWatch& gt) {
	mRenderer->Render(gt);
}
int Scene::GetReady() {
    mRenderer->ResetCommandList();
    // make mesh resident to GPU memory
    for (auto& namedPrimitive : mPrimitives){
        auto& primitive = namedPrimitive.second;
        primitive->GetMesh()->MakeResident2GPU(mRenderer->GetDevice(), mRenderer->GetCommandList());
    }
    mRenderer->ExecuteCommand();
    // Wait until initialization is complete.
    mRenderer->WaitForGPU();
    return 0;
}
void Scene::AddShader(const stShaderDescription& shaderDesc) {
    std::shared_ptr<D3DShaderWrapper> shader = nullptr;
    auto it = mShaders.find(shaderDesc.name);
    if (it == mShaders.end()) {
        shader = std::make_shared<D3DShaderWrapper>();
        mShaders[shaderDesc.name] = shader;
    }
    else {
        shader = mShaders[shaderDesc.name];
    }
    shader->BuildCBVDescriptorHeaps(mRenderer->GetDevice(), (UINT)shaderDesc.constBufferSize.size());
    shader->BuildConstantBuffers(mRenderer->GetDevice(), shaderDesc.constBufferSize[0]);
    shader->BuildRootSignature(mRenderer->GetDevice());
    shader->BuildShadersAndInputLayout(shaderDesc);
    shader->BuildPSO
    (
        mRenderer->GetDevice(),             mRenderer->GetBackBufferFormat(),
        mRenderer->GetDepthStencilFormat(), mRenderer->GetMSAAStatus(),
        mRenderer->GetSampleCount(),        mRenderer->GetMSAAQuality()
    );
}
void Scene::AddPrimitive(const stRawMesh& mesh) {
	mPrimitives.emplace(mesh.name, std::make_unique<D3DPrimitive>(mesh));
}

void Scene::PrimitiveAddSubMesh(const std::string& name, const stRawMesh::stSubMesh& subMesh){
	auto it = mPrimitives.find(name);
	if (it == mPrimitives.end()) return;
    D3DMesh* mesh = mPrimitives[name]->GetMesh();
    mesh->AddSubMesh(subMesh);
}
} // end namespace