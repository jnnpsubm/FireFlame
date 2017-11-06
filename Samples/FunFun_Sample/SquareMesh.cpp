#include "SquareMesh.h"

void SquareMesh::GetMeshDesc(FireFlame::stRawMesh& meshDesc) {
    using namespace FireFlame;
    meshDesc.indexCount = (unsigned int)indices.size();
    meshDesc.indexFormat = FireFlame::Index_Format::UINT16;
    meshDesc.indices = indices.data();

    meshDesc.vertexCount = (unsigned int)vertices.size();
    meshDesc.vertexSize = sizeof(SquareMesh::Vertex);
    meshDesc.vertexFormat = VERTEX_FORMAT_POS_FLOAT3 | VERTEX_FORMAT_TEXCOORD_FLOAT2;
    meshDesc.vertices = vertices.data();
    meshDesc.LocalToWorld = matrixLocal2World;
}

void SquareMesh::GetSubMeshDesc(FireFlame::stRawMesh::stSubMesh& subMesh) {
    subMesh.indexCount = (unsigned int)indices.size();
    subMesh.startIndexLocation = 0;
    subMesh.baseVertexLocation = 0;
}