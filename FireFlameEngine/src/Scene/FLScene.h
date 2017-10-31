#pragma once
#include <unordered_map>
#include "Primitive\FLD3DPrimitive.h"
#include "Vertex\FLVertex.h"
#include "..\FLTypeDefs.h"

namespace FireFlame {
class Scene {
public:
	Scene();
	void AddPrimitive(const std::string& name,
					  unsigned int vertexSize,
					  unsigned int vertexCount,
					  unsigned int Vertex_Format,
					  const void* vertices,
					  Index_Format indexFormat,
				      unsigned int indexCount,
					  const void* indices,
					  const Matrix4X4& LocalToWorld);

private:
	std::unordered_map<std::string, D3DPrimitive> mPrimitives; // todo : scene manage
};
}