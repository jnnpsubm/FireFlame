#include "FLScene.h"

namespace FireFlame {
Scene::Scene() = default;
void Scene::AddPrimitive(const stRawMesh& mesh) {
	mPrimitives.emplace(mesh.name, std::make_unique<D3DPrimitive>(mesh));
}

void Scene::PrimitiveAddSubMesh(const stRawMesh::stSubMesh& subMesh){
	auto it = mPrimitives.find(subMesh.name);
	if (it != mPrimitives.end()) return;
	mPrimitives[subMesh.name]->GetMesh();

}
}