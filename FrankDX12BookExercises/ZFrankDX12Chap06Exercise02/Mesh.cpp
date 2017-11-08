#include "Mesh.h"

void Mesh::GetMeshDesc(FireFlame::stRawMesh& meshDesc) {
    using namespace FireFlame;
    meshDesc.indexCount = (unsigned int)indices.size();
    meshDesc.indexFormat = FireFlame::Index_Format::UINT16;
    meshDesc.indices = indices.data();

    meshDesc.vertexDataCount.push_back((unsigned int)vertexPos.size());
    meshDesc.vertexDataSize.push_back(sizeof(Mesh::VPosData));
    meshDesc.vertexData.push_back(vertexPos.data());

    meshDesc.vertexDataCount.push_back((unsigned int)vertexColor.size());
    meshDesc.vertexDataSize.push_back(sizeof(Mesh::VColorData));
    meshDesc.vertexData.push_back(vertexColor.data());
}

void Mesh::GetSubMeshDesc(FireFlame::stRawMesh::stSubMesh& subMesh) {
    subMesh.indexCount = (unsigned int)indices.size();
    subMesh.startIndexLocation = 0;
    subMesh.baseVertexLocation = 0;
}