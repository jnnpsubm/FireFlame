#include "FLD3DPrimitive.h"

namespace FireFlame {
D3DPrimitive::D3DPrimitive() = default;
D3DPrimitive::D3DPrimitive(const std::string& name,
						   unsigned int vertexSize,
						   unsigned int vertexCount,
						   unsigned int Vertex_Format,
						   const void* vertices,
						   Index_Format indexFormat,
						   unsigned int indexCount,
						   const void* indices,
						   const Matrix4X4& LocaltoWorld)
	: mMesh()
{
	mLocal2World = LocaltoWorld;
}
}