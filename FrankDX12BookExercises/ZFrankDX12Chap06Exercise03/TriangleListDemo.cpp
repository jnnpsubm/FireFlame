#include "TriangleListDemo.h"

TriangleListDemo::TriangleListDemo(FireFlame::Engine& engine) : Demo(engine) {
    using namespace FireFlame;

    const unsigned int uPointNum = 15;
    std::vector<Vertex> vPos;
    vPos.resize(uPointNum);
    vPos[0] = Vertex(-2.0f, -2.0f, 1.0f);
    for (size_t i = 1; i < uPointNum; i++){
        if (i & 1) {
            vPos[i].x = vPos[i - 1].x;
            vPos[i].y = vPos[i - 1].y + 0.3f;
            vPos[i].z = vPos[i - 1].z;
        }
        else {
            vPos[i].x = vPos[i - 1].x + 0.3f;
            vPos[i].y = vPos[i - 1].y;
            vPos[i].z = vPos[i - 1].z;
        }
            
    }
    mPointList.Generate(vPos);

    mShaderDesc.name = "Shader1";
    mShaderDesc.objCBSize = sizeof(ObjectConsts);
    mShaderDesc.AddVertexInput("POSITION", FireFlame::VERTEX_FORMAT_FLOAT3);
    mShaderDesc.AddVertexInput("COLOR", FireFlame::VERTEX_FORMAT_FLOAT4);
    mShaderDesc.AddShaderStage(L"Shaders\\Shader.hlsl", Shader_Type::VS, "VS", "vs_5_0");
    mShaderDesc.AddShaderStage(L"Shaders\\Shader.hlsl", Shader_Type::PS, "PS", "ps_5_0");

    mMeshDesc.primitiveTopology = Primitive_Topology::TriangleList;
    mMeshDesc.indexCount = uPointNum;
    mMeshDesc.indexFormat = Index_Format::UINT16;
    mMeshDesc.indices = mPointList.GetIndexData();
    mMeshDesc.vertexDataCount.push_back((unsigned int)uPointNum);
    mMeshDesc.vertexDataSize.push_back(sizeof(VertexColored));
    mMeshDesc.vertexData.push_back(mPointList.GetVertexData());
    mMeshDesc.subMeshs.emplace_back("All", uPointNum);

    engine.SetCullMode(FireFlame::Cull_Mode::None);
}