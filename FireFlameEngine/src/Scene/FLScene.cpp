#include "FLScene.h"
#include "..\Renderer\FLRenderer.h"

namespace FireFlame {
Scene::Scene() = default;

void Scene::Update(Renderer& renderer, const StopWatch& gt) {
	
}
void Scene::Render(Renderer& renderer, const StopWatch& gt) {
	renderer.Render(gt);
}

void Scene::AddPrimitive(const stRawMesh& mesh) {
	mPrimitives.emplace(mesh.name, std::make_unique<D3DPrimitive>(mesh));
}

void Scene::PrimitiveAddSubMesh(const stRawMesh::stSubMesh& subMesh){
	auto it = mPrimitives.find(subMesh.name);
	if (it != mPrimitives.end()) return;
	mPrimitives[subMesh.name]->GetMesh()->AddSubMesh(subMesh);
}
}