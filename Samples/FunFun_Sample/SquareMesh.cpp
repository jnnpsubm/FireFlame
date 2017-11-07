#include "SquareMesh.h"

void SquareMesh::GetMeshDesc(FireFlame::stRawMesh& meshDesc) {
    using namespace FireFlame;
    meshDesc.indexCount = (unsigned int)indices.size();
    meshDesc.indexFormat = FireFlame::Index_Format::UINT16;
    meshDesc.indices = indices.data();

    meshDesc.vertexDataCount.push_back((unsigned int)vertices.size());
    meshDesc.vertexDataSize.push_back(sizeof(SquareMesh::Vertex));
    meshDesc.vertexDataFormat.push_back(VERTEX_FORMAT_FLOAT3 | VERTEX_FORMAT_FLOAT2);
    meshDesc.vertexData.push_back(vertices.data());
    meshDesc.LocalToWorld = matrixLocal2World;
}

void SquareMesh::GetSubMeshDesc(FireFlame::stRawMesh::stSubMesh& subMesh) {
    subMesh.indexCount = (unsigned int)indices.size();
    subMesh.startIndexLocation = 0;
    subMesh.baseVertexLocation = 0;
}