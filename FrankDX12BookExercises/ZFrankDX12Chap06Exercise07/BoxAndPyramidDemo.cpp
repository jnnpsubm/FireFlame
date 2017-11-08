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
    else if (wParam == 'L') // Chap06Exercise12
    {
        int clientWidth = mEngine.GetWindow()->ClientWidth();
        int clientHeight = mEngine.GetWindow()->ClientHeight();

        FireFlame::stViewport viewport{ 
            0.f,
            0.f,
            (float)clientWidth/2,
            (float)clientHeight,
            0.0f,
            1.0f 
        };
        mEngine.SetViewport(viewport);
    }
    else if (wParam == 'Z')
    {
        int clientWidth = mEngine.GetWindow()->ClientWidth();
        int clientHeight = mEngine.GetWindow()->ClientHeight();

        static FireFlame::stViewport viewport{
            0.f,
            0.f,
            (float)clientWidth,
            (float)clientHeight,
            0.0f,
            1.0f
        };
        viewport.z1 /= 2.f;
        mEngine.SetViewport(viewport);
    }
    else if (wParam == 'S') // Chap06Exercise13
    {
        int clientWidth = mEngine.GetWindow()->ClientWidth();
        int clientHeight = mEngine.GetWindow()->ClientHeight();

        int left = clientWidth / 4;
        int top = clientHeight / 4;
        int right = left + clientWidth / 2;
        int bottom = top + clientHeight / 2;
        mEngine.SetScissorRect(left, top, right, bottom);
    }
}