#include "PyramidDemo.h"

PyramidDemo::PyramidDemo(FireFlame::Engine& e) :Demo(e) {
    using namespace FireFlame;

    mShaderDesc.name = "PyramidDemo";
    mShaderDesc.objCBSize = sizeof(ObjectConsts);
    mShaderDesc.AddVertexInput("POSITION", FireFlame::VERTEX_FORMAT_FLOAT3);
    mShaderDesc.AddVertexInput("COLOR", FireFlame::VERTEX_FORMAT_FLOAT4);
    mShaderDesc.AddShaderStage(L"Shaders\\PyramidDemoShader.hlsl", Shader_Type::VS, "VS", "vs_5_0");
    mShaderDesc.AddShaderStage(L"Shaders\\PyramidDemoShader.hlsl", Shader_Type::PS, "PS", "ps_5_0");

    mMeshDesc.primitiveTopology = Primitive_Topology::TriangleList;
    mMeshDesc.indexCount = (unsigned int)mPyramid.indices.size();
    mMeshDesc.indexFormat = Index_Format::UINT16;
    mMeshDesc.indices = mPyramid.indices.data();
    mMeshDesc.vertexDataCount.push_back((unsigned int)mPyramid.vertices.size());
    mMeshDesc.vertexDataSize.push_back(sizeof(VertexColored));
    mMeshDesc.vertexData.push_back(mPyramid.vertices.data());
    mMeshDesc.subMeshs.emplace_back("All", (unsigned int)mPyramid.indices.size());

    //mEngine.SetCullMode(FireFlame::Cull_Mode::None);
}