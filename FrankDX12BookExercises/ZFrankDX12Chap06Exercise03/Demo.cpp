#include "Demo.h"
#include <string>

Demo::Demo(FireFlame::Engine& engine) : mEngine(engine){
    using namespace FireFlame;

    const unsigned int uPointNum = 10000 * 10 * 10;
    mPointList.Generate(uPointNum);

    mShaderDesc.name = "Shader1";
    mShaderDesc.constBufferSize.push_back(sizeof(ShaderConsts));
    mShaderDesc.AddVertexInput("POSITION", FireFlame::VERTEX_FORMAT_FLOAT3);
    mShaderDesc.AddVertexInput("COLOR",    FireFlame::VERTEX_FORMAT_FLOAT4);
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
    mMeshDesc.subMeshs.emplace_back("All",uPointNum);
}
Demo::~Demo(){}

void Demo::OnGameWindowResized(int w, int h) {
    // The window resized, so update the aspect ratio and recompute the projection matrix.
    DirectX::XMMATRIX P = DirectX::XMMatrixPerspectiveFovLH
    (
        0.25f*DirectX::XMVectorGetX(DirectX::g_XMPi),
        (float)w / h, 1.0f, 1000.0f
    );
    DirectX::XMStoreFloat4x4(&mProj, P);
}
void Demo::Update(float time_elapsed) {
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
void Demo::OnMouseDown(WPARAM btnState, int x, int y) {
    mLastMousePos.x = x;
    mLastMousePos.y = y;
    SetCapture(mEngine.GetWindow()->MainWnd());
}
void Demo::OnMouseUp(WPARAM btnState, int x, int y) {
    ReleaseCapture();
}
void Demo::OnMouseMove(WPARAM btnState, int x, int y) {
    if ((btnState & MK_LBUTTON) != 0) {
        // Make each pixel correspond to a quarter of a degree.
        float dx = DirectX::XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
        float dy = DirectX::XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

        // Update angles based on input to orbit camera around box.
        mTheta += dx;
        mPhi += dy;

        // Restrict the angle mPhi.

        mPhi = FireFlame::MathHelper::Clamp(mPhi, 0.1f, DirectX::XMVectorGetX(DirectX::g_XMPi) - 0.1f);
    }
    else if ((btnState & MK_RBUTTON) != 0)
    {
        // Make each pixel correspond to 0.005 unit in the scene.
        float dx = 0.005f*static_cast<float>(x - mLastMousePos.x);
        float dy = 0.005f*static_cast<float>(y - mLastMousePos.y);

        // Update the camera radius based on input.
        mRadius += dx - dy;

        // Restrict the radius.
        mRadius = FireFlame::MathHelper::Clamp(mRadius, 3.0f, 15.0f);
    }

    mLastMousePos.x = x;
    mLastMousePos.y = y;
}
