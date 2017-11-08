#include "BoxAndPyramidDemo.h"
#include <DirectXPackedVector.h>

BoxAndPyramidDemo::BoxAndPyramidDemo(FireFlame::Engine& e) :Demo(e) {
    using namespace FireFlame;

    for (size_t i = 0; i < 8; i++)
    {
        mBoxAndPyramid.vertices[i] += Vertex(1.3f, 0.0f, 0.0f);
        mBoxAndPyramid.verticesPacked[i] += Vertex(1.3f, 0.0f, 0.0f);
    }
    for (size_t i = 8; i < 13; i++)
    {
        mBoxAndPyramid.vertices[i] += Vertex(-1.3f, 0.0f, 0.0f);
        mBoxAndPyramid.verticesPacked[i] += Vertex(-1.3f, 0.0f, 0.0f);
    }

    mShaderDesc.name = "BoxAndPyramidDemo";
    mShaderDesc.constBufferSize.push_back(sizeof(ObjectConsts));
    mShaderDesc.AddVertexInput("POSITION", FireFlame::VERTEX_FORMAT_FLOAT3);
    //mShaderDesc.AddVertexInput("COLOR", FireFlame::VERTEX_FORMAT_FLOAT4);
    mShaderDesc.AddVertexInput("COLOR", FireFlame::VERTEX_FORMAT_A8R8G8B8_UNORM);
    mShaderDesc.AddShaderStage(L"Shaders\\BoxAndPymaridDemoShader.hlsl", Shader_Type::VS, "VS", "vs_5_0");
    mShaderDesc.AddShaderStage(L"Shaders\\BoxAndPymaridDemoShader.hlsl", Shader_Type::PS, "PS", "ps_5_0");

    mMeshDesc.primitiveTopology = Primitive_Topology::TriangleList;
    mMeshDesc.indexCount = (unsigned int)mBoxAndPyramid.indices.size();
    mMeshDesc.indexFormat = Index_Format::UINT16;
    mMeshDesc.indices = mBoxAndPyramid.indices.data();

    //mMeshDesc.vertexDataCount.push_back((unsigned int)mBoxAndPyramid.vertices.size());
    //mMeshDesc.vertexDataSize.push_back(sizeof(VertexColored));
    //mMeshDesc.vertexData.push_back(mBoxAndPyramid.vertices.data());
    mMeshDesc.vertexDataCount.push_back((unsigned int)mBoxAndPyramid.verticesPacked.size());
    mMeshDesc.vertexDataSize.push_back(sizeof(VertexPackedColored));
    mMeshDesc.vertexData.push_back(mBoxAndPyramid.verticesPacked.data());

    mMeshDesc.subMeshs.emplace_back("Box",     (unsigned int)36, 0,  0);
    mMeshDesc.subMeshs.emplace_back("Pyramid", (unsigned int)18, 36, 8);

    //mEngine.SetCullMode(FireFlame::Cull_Mode::None);
}

void BoxAndPyramidDemo::OnKeyUp(WPARAM wParam, LPARAM lParam) {
    if (wParam == 'W') {
        if (mEngine.GetFillMode() == FireFlame::Fill_Mode::Solid){
            mEngine.SetFillMode(FireFlame::Fill_Mode::Wireframe);
        }
        else {
            mEngine.SetFillMode(FireFlame::Fill_Mode::Solid);
        }
    }
    else if (wParam == 'N')
    {
        mEngine.SetCullMode(FireFlame::Cull_Mode::None);
    }
    else if (wParam == 'F')
    {
        mEngine.SetCullMode(FireFlame::Cull_Mode::Front);
    }
    else if (wParam == 'B')
    {
        mEngine.SetCullMode(FireFlame::Cull_Mode::Back);
    }
}