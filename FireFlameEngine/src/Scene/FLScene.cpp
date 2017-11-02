#include "FLScene.h"
#include "..\Renderer\FLRenderer.h"
#include "..\ShaderWrapper\FLD3DShaderWrapper.h"

namespace FireFlame {
Scene::Scene(std::shared_ptr<Renderer>& renderer) : mRenderer(renderer){}

void Scene::Update(const StopWatch& gt) {
	
}
void Scene::Render(const StopWatch& gt) {
	mRenderer->Render(gt);
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
    shader->BuildCBVDescriptorHeaps(mRenderer->GetDevice(), shaderDesc.constBufferSize.size());
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