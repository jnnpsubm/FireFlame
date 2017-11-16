#include "PointListDemo.h"

PointListDemo::PointListDemo(FireFlame::Engine& engine) :Demo(engine, "PointList") {
    using namespace FireFlame;

    const unsigned int uPointNum = 10000*10;
    mPointList.Generate(uPointNum);

    mShaderDesc.name = "Shader1";
    mShaderDesc.objCBSize = sizeof(ObjectConsts);
    mShaderDesc.passCBSize = 1;
    mShaderDesc.AddVertexInput("POSITION", FireFlame::VERTEX_FORMAT_FLOAT3);
    mShaderDesc.AddVertexInput("COLOR", FireFlame::VERTEX_FORMAT_FLOAT4);
    mShaderDesc.AddShaderStage(L"Shaders\\Shader.hlsl", Shader_Type::VS, "VS", "vs_5_0");
    mShaderDesc.AddShaderStage(L"Shaders\\Shader.hlsl", Shader_Type::PS, "PS", "ps_5_0");

    mMeshDesc.name = "PointList";
    mMeshDesc.primitiveTopology = Primitive_Topology::PointList;
    mMeshDesc.indexCount = uPointNum;
    mMeshDesc.indexFormat = Index_Format::UINT16;
    mMeshDesc.indices = mPointList.GetIndexData();
    mMeshDesc.vertexDataCount.push_back((unsigned int)uPointNum);
    mMeshDesc.vertexDataSize.push_back(sizeof(VertexColored));
    mMeshDesc.vertexData.push_back(mPointList.GetVertexData());
    mMeshDesc.subMeshs.emplace_back("All", uPointNum);

    mRenderItems[0].subMesh = mMeshDesc.subMeshs[0];
    mRenderItems[0].topology = FireFlame::Primitive_Topology::PointList;
}

//void PointListDemo::Update(float time_elapsed) {
//    
//}