#include "LineStripDemo.h"

LineStripDemo::LineStripDemo(FireFlame::Engine& engine) : Demo(engine) {
    using namespace FireFlame;

    const unsigned int uPointNum = 10000 * 10;
    mPointList.Generate(uPointNum);

    mShaderDesc.name = "Shader1";
    mShaderDesc.constBufferSize.push_back(sizeof(ObjectConsts));
    mShaderDesc.AddVertexInput("POSITION", FireFlame::VERTEX_FORMAT_FLOAT3);
    mShaderDesc.AddVertexInput("COLOR", FireFlame::VERTEX_FORMAT_FLOAT4);
    mShaderDesc.AddShaderStage(L"Shaders\\Shader.hlsl", Shader_Type::VS, "VS", "vs_5_0");
    mShaderDesc.AddShaderStage(L"Shaders\\Shader.hlsl", Shader_Type::PS, "PS", "ps_5_0");

    mMeshDesc.primitiveTopology = Primitive_Topology::LineStrip;
    mMeshDesc.indexCount = uPointNum;
    mMeshDesc.indexFormat = Index_Format::UINT16;
    mMeshDesc.indices = mPointList.GetIndexData();
    mMeshDesc.vertexDataCount.push_back((unsigned int)uPointNum);
    mMeshDesc.vertexDataSize.push_back(sizeof(VertexColored));
    mMeshDesc.vertexDataFormat.push_back(VERTEX_FORMAT_FLOAT3 | VERTEX_FORMAT_FLOAT4);
    mMeshDesc.vertexData.push_back(mPointList.GetVertexData());
    mMeshDesc.LocalToWorld = Matrix4X4();
    mMeshDesc.subMeshs.emplace_back("All", uPointNum);
}