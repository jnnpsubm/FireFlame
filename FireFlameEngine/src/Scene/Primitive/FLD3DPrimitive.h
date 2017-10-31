#pragma once
#include "../Shape/FLD3DMesh.h"

namespace FireFlame {
class D3DPrimitive {
public:
	D3DPrimitive();
	D3DPrimitive(const std::string& name,
				 unsigned int vertexSize,
				 unsigned int vertexCount,
				 unsigned int Vertex_Format,
				 const void* vertices,
				 Index_Format indexFormat,
				 unsigned int indexCount,
				 const void* indices,
				 const Matrix4X4& LocaltoWorld);

private:
	D3DMesh mMesh;
	DirectX::XMFLOAT4X4 mLocal2World;
};
}