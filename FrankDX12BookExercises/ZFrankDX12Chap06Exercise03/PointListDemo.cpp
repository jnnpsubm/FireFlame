#include "PointListDemo.h"

PointListDemo::PointListDemo(FireFlame::Engine& engine) :Demo(engine) {
    using namespace FireFlame;

    const unsigned int uPointNum = 10000 * 10;
    mPointList.Generate(uPointNum);

    mShaderDesc.name = "Shader1";
    mShaderDesc.constBufferSize.push_back(sizeof(ShaderConsts));
    mShaderDesc.AddVertexInput("POSITION", FireFlame::VERTEX_FORMAT_FLOAT3);
    mShaderDesc.AddVertexInput("COLOR", FireFlame::VERTEX_FORMAT_FLOAT4);
    mShaderDesc.AddShaderStage(L"Shaders\\Shader.hlsl", Shader_Type::VS, "VS", "vs_5_0");
    mShaderDesc.AddShaderStage(L"Shaders\\Shader.hlsl", Shader_Type::PS, "PS", "ps_5_0");

    mMeshDesc.primitiveTopology = Primitive_Topology::PointList;
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

void PointListDemo::Update(float time_elapsed) {
    using namespace DirectX;
    // Convert Spherical to Cartesian coordinates.
    float x = mRadius*sinf(mPhi)*cosf(mTheta);
    float z = mRadius*sinf(mPhi)*sinf(mTheta);
    float y = mRadius*cosf(mPhi);

    // Build the view matrix.
    DirectX::XMVECTOR pos = DirectX::XMVectorSet(x, y, z, 1.0f);
    DirectX::XMVECTOR target = DirectX::XMVectorZero();
    DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(pos, target, up);
    DirectX::XMStoreFloat4x4(&mView, view);

    DirectX::XMMATRIX world = DirectX::XMLoadFloat4x4(&mWorld);
    DirectX::XMMATRIX proj = DirectX::XMLoadFloat4x4(&mProj);
    DirectX::XMMATRIX worldViewProj = world*view*proj;

    // Update the constant buffer with the latest worldViewProj matrix.
    ShaderConsts Constants;
    DirectX::XMStoreFloat4x4(&Constants.WorldViewProj, XMMatrixTranspose(worldViewProj));
    mEngine.GetScene()->UpdateShaderCBData(mShaderDesc.name, 0, Constants);
}