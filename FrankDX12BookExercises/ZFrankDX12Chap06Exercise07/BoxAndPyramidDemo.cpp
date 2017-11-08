#include "BoxAndPyramidDemo.h"

BoxAndPyramidDemo::BoxAndPyramidDemo(FireFlame::Engine& e) :Demo(e) {
    using namespace FireFlame;

    for (size_t i = 0; i < 8; i++)
    {
        mBoxAndPyramid.vertices[i] += Vertex(1.3f, 0.0f, 0.0f);
    }
    for (size_t i = 8; i < 13; i++)
    {
        mBoxAndPyramid.vertices[i] += Vertex(-1.3f, 0.0f, 0.0f);
    }

    mShaderDesc.name = "BoxAndPyramidDemo";
    mShaderDesc.constBufferSize.push_back(sizeof(ObjectConsts));
    mShaderDesc.AddVertexInput("POSITION", FireFlame::VERTEX_FORMAT_FLOAT3);
    mShaderDesc.AddVertexInput("COLOR", FireFlame::VERTEX_FORMAT_FLOAT4);
    mShaderDesc.AddShaderStage(L"Shaders\\BoxAndPymaridDemoShader.hlsl", Shader_Type::VS, "VS", "vs_5_0");
    mShaderDesc.AddShaderStage(L"Shaders\\BoxAndPymaridDemoShader.hlsl", Shader_Type::PS, "PS", "ps_5_0");

    mMeshDesc.primitiveTopology = Primitive_Topology::TriangleList;
    mMeshDesc.indexCount = (unsigned int)mBoxAndPyramid.indices.size();
    mMeshDesc.indexFormat = Index_Format::UINT16;
    mMeshDesc.indices = mBoxAndPyramid.indices.data();
    mMeshDesc.vertexDataCount.push_back((unsigned int)mBoxAndPyramid.vertices.size());
    mMeshDesc.vertexDataSize.push_back(sizeof(VertexColored));
    mMeshDesc.vertexDataFormat.push_back(VERTEX_FORMAT_FLOAT3 | VERTEX_FORMAT_FLOAT4);
    mMeshDesc.vertexData.push_back(mBoxAndPyramid.vertices.data());
    mMeshDesc.LocalToWorld = Matrix4X4();
    mMeshDesc.subMeshs.emplace_back("Box",     (unsigned int)36, 0,  0);
    mMeshDesc.subMeshs.emplace_back("Pyramid", (unsigned int)18, 36, 8);

    //mEngine.SetCullMode(FireFlame::Cull_Mode::None);
}