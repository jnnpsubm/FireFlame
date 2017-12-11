#include "GSIcosahedronApp.h"

void GSIcosahedronApp::PreInitialize()
{
    mEngine.SetDefaultClearColor({ 0.7f, 0.7f, 0.7f, 1.0f });
}

void GSIcosahedronApp::Initialize()
{
    AddShaders();
    AddPSOs();
    AddMaterials();
    AddMeshs();
    AddRenderItems();

    mPasses.push_back("DefaultPass");
    mEngine.GetScene()->AddPass(mPasses[0]);
}

void GSIcosahedronApp::Update(float time_elapsed)
{
    OnKeyboardInput(time_elapsed);
    UpdateCamera(time_elapsed);
    UpdateMainPassCB(time_elapsed);
}

void GSIcosahedronApp::UpdateMainPassCB(float time_elapsed)
{
    using namespace DirectX;

    // for icosahedron
    auto& passCB = mPassCBs["icosahedron"];
    XMMATRIX view = XMLoadFloat4x4(&mView);
    XMMATRIX proj = XMLoadFloat4x4(&mProj);
    XMMATRIX viewProj = XMMatrixMultiply(view, proj);
    XMStoreFloat4x4(&passCB.View, XMMatrixTranspose(view));
    XMStoreFloat4x4(&passCB.Proj, XMMatrixTranspose(proj));
    XMStoreFloat4x4(&passCB.ViewProj, XMMatrixTranspose(viewProj));
    
    XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
    XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
    XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);
    XMStoreFloat4x4(&passCB.InvView, XMMatrixTranspose(invView));
    XMStoreFloat4x4(&passCB.InvProj, XMMatrixTranspose(invProj));
    XMStoreFloat4x4(&passCB.InvViewProj, XMMatrixTranspose(invViewProj));
    passCB.EyePosW = mEyePos;
    float clientWidth = (float)mEngine.GetWindow()->ClientWidth();
    float clientHeight = (float)mEngine.GetWindow()->ClientHeight();
    passCB.RenderTargetSize = XMFLOAT2(clientWidth, clientHeight);
    passCB.InvRenderTargetSize = XMFLOAT2(1.0f / clientWidth, 1.0f / clientHeight);
    passCB.NearZ = 1.0f;
    passCB.FarZ = 1000.0f;
    passCB.TotalTime = mEngine.TotalTime();
    passCB.DeltaTime = mEngine.DeltaTime();

    passCB.AmbientLight = { 0.25f, 0.25f, 0.35f, 1.0f };
    passCB.Lights[0].Direction = { 0.57735f, -0.57735f, -0.57735f };
    passCB.Lights[0].Strength = { 0.6f, 0.6f, 0.6f };
    passCB.Lights[1].Direction = { -0.57735f, -0.57735f, 0.57735f };
    passCB.Lights[1].Strength = { 0.3f, 0.3f, 0.3f };
    passCB.Lights[2].Direction = { 0.0f, -0.707f, -0.707f };
    passCB.Lights[2].Strength = { 0.15f, 0.15f, 0.15f };
    mEngine.GetScene()->UpdateShaderPassCBData(mShaderDescs["icosahedron"].name, sizeof(PassConstantsLight), &passCB);
}

void GSIcosahedronApp::AddShaders()
{
    AddShaderIcosahedron();
}

void GSIcosahedronApp::AddShaderIcosahedron()
{
    using namespace FireFlame;

    auto& shaderDesc = mShaderDescs["icosahedron"];
    shaderDesc.name = "icosahedron";
    shaderDesc.objCBSize = sizeof(ObjectConsts);
    shaderDesc.materialCBSize = sizeof(MaterialConstants);
    shaderDesc.passCBSize = sizeof(PassConstantsLight);
    shaderDesc.objParamIndex = 1;
    shaderDesc.matParamIndex = 2;
    shaderDesc.passParamIndex = 3;
    shaderDesc.AddVertexInput("POSITION", FireFlame::VERTEX_FORMAT_FLOAT3);
    shaderDesc.AddShaderStage(L"Shaders\\Icosahedron.hlsl", Shader_Type::VS, "VS", "vs_5_0");
    shaderDesc.AddShaderStage(L"Shaders\\Icosahedron.hlsl", Shader_Type::GS, "GS", "gs_5_0");
    shaderDesc.AddShaderStage(L"Shaders\\Icosahedron.hlsl", Shader_Type::PS, "PS", "ps_5_0");
    mEngine.GetScene()->AddShader(shaderDesc);
}

void GSIcosahedronApp::AddPSOs()
{
    using namespace FireFlame;

    PSODesc descIcosahedron(mShaderDescs["icosahedron"].name);
    descIcosahedron.cullMode = Cull_Mode::None;
    mEngine.GetScene()->AddPSO("icosahedron_default", descIcosahedron);
}

void GSIcosahedronApp::AddMaterials()
{
    auto& matCylinder = mMaterials["icosahedron"];
    matCylinder.Name = "icosahedron";
    matCylinder.DiffuseAlbedo = FireFlame::Vector4f(0.5f, 0.4f, 0.3f, 1.0f);
    matCylinder.FresnelR0 = FireFlame::Vector3f(0.8f, 0.8f, 0.8f);
    matCylinder.Roughness = 0.125f;
    mEngine.GetScene()->AddMaterial
    (
        matCylinder.Name,
        mShaderDescs["icosahedron"].name,
        sizeof(MaterialConstants), &matCylinder
    );
}

void GSIcosahedronApp::AddMeshs()
{
    AddMeshIcosahedron();
}

void GSIcosahedronApp::AddMeshIcosahedron()
{
    using namespace FireFlame;
    
    const float X = 0.525731f;
    const float Z = 0.850651f;

    std::array<FLVertex, 12> vertices = {
        FLVertex(-X, 0.0f, Z),  FLVertex(X, 0.0f, Z),
        FLVertex(-X, 0.0f, -Z), FLVertex(X, 0.0f, -Z),
        FLVertex(0.0f, Z, X),   FLVertex(0.0f, Z, -X),
        FLVertex(0.0f, -Z, X),  FLVertex(0.0f, -Z, -X),
        FLVertex(Z, X, 0.0f),   FLVertex(-Z, X, 0.0f),
        FLVertex(Z, -X, 0.0f),  FLVertex(-Z, -X, 0.0f)
    };

    std::array<std::uint16_t, 60> indices = {
        1,4,0,  4,9,0,  4,5,9,  8,5,4,  1,8,4,
        1,10,8, 10,3,8, 8,3,5,  3,2,5,  3,7,2,
        3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
        10,1,6, 11,0,9, 2,11,9, 5,2,9,  11,2,7
    };

    auto& mesh = mMeshDescs["icosahedron"];
    mesh.name = "icosahedron";
    mesh.primitiveTopology = Primitive_Topology::TriangleList;
    mesh.indexFormat = Index_Format::UINT16;
    mesh.indexCount = (unsigned)indices.size();
    mesh.indices = indices.data();
    mesh.vertexDataSize.emplace_back((unsigned)sizeof(FLVertex));
    mesh.vertexDataCount.emplace_back((unsigned)vertices.size());
    mesh.vertexData.emplace_back(vertices.data());
    mesh.subMeshs.emplace_back("all", (unsigned)indices.size());

    mEngine.GetScene()->AddPrimitive(mesh);
}

void GSIcosahedronApp::AddRenderItems()
{
    AddRenderItemsIcosahedron();
}

void GSIcosahedronApp::AddRenderItemsIcosahedron()
{
    using namespace DirectX;

    FireFlame::stRenderItemDesc RItem("icosahedron01", mMeshDescs["icosahedron"].subMeshs[0]);
    RItem.mat = "icosahedron";
    XMFLOAT4X4 trans[2];
    XMStoreFloat4x4
    (
        &trans[0],
        XMMatrixIdentity()
    );
    XMStoreFloat4x4
    (
        &trans[1],
        XMMatrixIdentity()
    );
    RItem.dataLen = sizeof(XMFLOAT4X4)*_countof(trans);
    RItem.data = &trans[0];
    mEngine.GetScene()->AddRenderItem
    (
        mMeshDescs["icosahedron"].name,
        mShaderDescs["icosahedron"].name,
        "icosahedron_default",
        RItem
    );
}
