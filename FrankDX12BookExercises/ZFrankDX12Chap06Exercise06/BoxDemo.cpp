#include "BoxDemo.h"

BoxDemo::BoxDemo(FireFlame::Engine& e) :Demo(e, "Box1") {
    using namespace FireFlame;

    mShaderDesc.name = "BoxDemo";
    mShaderDesc.objCBSize = sizeof(ObjectConsts);
    mShaderDesc.AddVertexInput("POSITION", FireFlame::VERTEX_FORMAT_FLOAT3);
    mShaderDesc.AddVertexInput("COLOR", FireFlame::VERTEX_FORMAT_FLOAT4);
    mShaderDesc.AddShaderStage(L"Shaders\\BoxDemoShader.hlsl", Shader_Type::VS, "VS", "vs_5_0");
    mShaderDesc.AddShaderStage(L"Shaders\\BoxDemoShader.hlsl", Shader_Type::PS, "PS", "ps_5_0");

    mMeshDesc.name = "Box";
    mMeshDesc.primitiveTopology = Primitive_Topology::TriangleList;
    mMeshDesc.indexCount = (unsigned int)mBox.indices.size();
    mMeshDesc.indexFormat = Index_Format::UINT16;
    mMeshDesc.indices = mBox.indices.data();
    mMeshDesc.vertexDataCount.push_back((unsigned int)mBox.vertices.size());
    mMeshDesc.vertexDataSize.push_back(sizeof(VertexColored));
    mMeshDesc.vertexData.push_back(mBox.vertices.data());
    mMeshDesc.subMeshs.emplace_back("All", (unsigned int)mBox.indices.size());

    mRenderItems[0].subMesh = mMeshDesc.subMeshs[0];

    //mEngine.SetCullMode(FireFlame::Cull_Mode::None);
}
void BoxDemo::Update(float time_elapsed) {
    using namespace DirectX;

    DirectX::XMMATRIX wvp;
    BuildUpWVP(wvp);

    BoxDemoObjectConsts ObjConstants;
    ObjConstants.PulseColor = DirectX::XMFLOAT4(0.823529482f, 0.411764741f, 0.117647067f, 1.000000000f);
    ObjConstants.TotalTime = mEngine.TotalTime();
    // Update the constant buffer with the latest worldViewProj matrix.
    DirectX::XMStoreFloat4x4(&ObjConstants.WorldViewProj, XMMatrixTranspose(wvp));
    //mEngine.GetScene()->UpdateShaderCBData(mShaderDesc.name, 0, ObjConstants);
    mEngine.GetScene()->UpdateRenderItemCBData(mRenderItems[0].name, sizeof(BoxDemoObjectConsts), &ObjConstants);
}