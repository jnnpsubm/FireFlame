#include "FLScene.h"

namespace FireFlame {
Scene::Scene() = default;
void Scene::AddPrimitive(const std::string& name,
						 unsigned int vertexSize,
						 unsigned int vertexCount,
						 unsigned int Vertex_Format,
						 const void* vertices,
						 Index_Format indexFormat,
						 unsigned int indexCount,
						 const void* indices,
						 const Matrix4X4& LocalToWorld) 
{
	D3DPrimitive primitive(name, vertexSize, vertexCount, Vertex_Format, vertices,
		indexFormat, indexCount, indices, LocalToWorld);
}
}