#pragma once
#include <unordered_map>
#include "Primitive\FLD3DPrimitive.h"
#include "Vertex\FLVertex.h"
#include "..\FLTypeDefs.h"

namespace FireFlame {
class Scene {
public:
	Scene();
	void AddPrimitive(const stRawMesh& mesh);
	void PrimitiveAddSubMesh(const stRawMesh::stSubMesh& subMesh);

private:
	// todo : scene manage
	std::unordered_map<std::string, std::unique_ptr<D3DPrimitive>> mPrimitives; 
};
}