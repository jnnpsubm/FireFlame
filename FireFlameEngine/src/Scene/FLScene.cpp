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

void Scene::AddPrimitive(const stRawMesh& mesh, const stShaderDescription& shaderDesc) {
    std::shared_ptr<D3DShaderWrapper> shader = nullptr;
    auto it = mShaders.find(shaderDesc.name);
    if (it == mShaders.end()) {
        shader = std::make_shared<D3DShaderWrapper>();
        mShaders[shaderDesc.name] = shader;
    }else {
        shader = mShaders[shaderDesc.name];
    }
    shader->BuildCBVDescriptorHeaps(mRenderer->GetDevice(), shaderDesc.numConstBuffer);
    
	mPrimitives.emplace(mesh.name, std::make_unique<D3DPrimitive>(mesh));
}

void Scene::PrimitiveAddSubMesh(const stRawMesh::stSubMesh& subMesh){
	auto it = mPrimitives.find(subMesh.name);
	if (it != mPrimitives.end()) return;
	mPrimitives[subMesh.name]->GetMesh()->AddSubMesh(subMesh);
}
} // end namespace