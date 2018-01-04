#include "TSIcosahedronApp.h"

void TSIcosahedronApp::PreInitialize() {}

void TSIcosahedronApp::Initialize()
{
    AddShaders();
    AddPSOs();
    AddMaterials();
    AddMeshs();
    AddRenderItems();

    mPasses.push_back("DefaultPass");
    mEngine.GetScene()->AddPass(mPasses[0]);
}

void TSIcosahedronApp::Update(float time_elapsed)
{
    FLEngineApp3::Update(time_elapsed);
}

void TSIcosahedronApp::UpdateMainPassCB(float time_elapsed)
{
    using namespace DirectX;

    // pass constants
    XMMATRIX view = XMLoadFloat4x4(&mView);
    XMMATRIX proj = XMLoadFloat4x4(&mProj);

    XMMATRIX viewProj = XMMatrixMultiply(view, proj);
    XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
    XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
    XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);

    auto& passCB = mPassCBs["main"];
    XMStoreFloat4x4(&passCB.View, XMMatrixTranspose(view));
    XMStoreFloat4x4(&passCB.InvView, XMMatrixTranspose(invView));
    XMStoreFloat4x4(&passCB.Proj, XMMatrixTranspose(proj));
    XMStoreFloat4x4(&passCB.InvProj, XMMatrixTranspose(invProj));
    XMStoreFloat4x4(&passCB.ViewProj, XMMatrixTranspose(viewProj));
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

    passCB.TessLod = mTessLod;
    mEngine.GetScene()->UpdateShaderPassCBData(mShaderDescs["main"].name, sizeof(PassConstants), &passCB);
}

void TSIcosahedronApp::AddShaders()
{
    using namespace FireFlame;

    auto& shaderDesc = mShaderDescs["main"];
    shaderDesc.name = "main";
    shaderDesc.objCBSize = sizeof(ObjectConsts);
    shaderDesc.passCBSize = sizeof(PassConstants);
    shaderDesc.materialCBSize = sizeof(MaterialConstants);
    shaderDesc.ParamDefault();

    //std::wstring strHlslFile = L"Shaders\\main.hlsl";
    std::wstring strHlslFile = L"Shaders\\PNTriangle.hlsl";
    shaderDesc.AddVertexInput("POSITION", FireFlame::VERTEX_FORMAT_FLOAT3);
    shaderDesc.AddShaderStage(strHlslFile, Shader_Type::VS, "VS", "vs_5_0");
    shaderDesc.AddShaderStage(strHlslFile, Shader_Type::HS, "HS", "hs_5_0");
    shaderDesc.AddShaderStage(strHlslFile, Shader_Type::DS, "DS", "ds_5_0");
    shaderDesc.AddShaderStage(strHlslFile, Shader_Type::PS, "PS", "ps_5_0");

    mEngine.GetScene()->AddShader(shaderDesc);
}

void TSIcosahedronApp::AddPSOs()
{
    using namespace FireFlame;

    PSODesc descDefault(mShaderDescs["main"].name);
    descDefault.topologyType = Primitive_Topology_Type::Patch;
    descDefault.cullMode = Cull_Mode::None;
    mEngine.GetScene()->AddPSO("default", descDefault);
}

void TSIcosahedronApp::AddMaterials()
{
    auto& metal = mMaterials["metal"];
    metal.Name = "metal";
    metal.DiffuseAlbedo = FireFlame::Vector4f(0.2f, 0.5f, 0.5f, 1.0f);
    metal.FresnelR0 = FireFlame::Vector3f(0.35f, 0.75f, 0.75f);
    metal.Roughness = 0.725f;
    mEngine.GetScene()->AddMaterial
    (
        metal.Name,
        mShaderDescs["main"].name, "",
        sizeof(MaterialConstants), &metal
    );
}

void TSIcosahedronApp::AddMeshs()
{
    using namespace FireFlame;

    GeometryGenerator geoGen;
    //auto mesh = geoGen.CreateIcosahedron();
    auto mesh = geoGen.CreateOctahedron();
    std::vector<FLVertex> vertices(mesh.Vertices.size());
    for (size_t i = 0; i < mesh.Vertices.size(); ++i)
    {
        auto& p = mesh.Vertices[i].Position;
        vertices[i].Pos = p;
    }
    std::vector<std::uint16_t> indices = mesh.GetIndices16();

    auto& meshDesc = mMeshDescs["quad"];
    meshDesc.name = "quad";
    meshDesc.indexCount = (unsigned int)indices.size();
    meshDesc.indexFormat = Index_Format::UINT16;
    meshDesc.indices = indices.data();

    meshDesc.vertexData.push_back(vertices.data());
    meshDesc.vertexDataCount.push_back((unsigned)vertices.size());
    meshDesc.vertexDataSize.push_back(sizeof(FLVertex));

    // sub meshes
    meshDesc.subMeshs.emplace_back("All", (UINT)indices.size());
    mEngine.GetScene()->AddPrimitive(meshDesc);
}

void TSIcosahedronApp::AddRenderItems()
{
    using namespace DirectX;

    FireFlame::stRenderItemDesc RItem("land", mMeshDescs["quad"].subMeshs[0]);
    RItem.topology = FireFlame::Primitive_Topology::CONTROL_POINT_PATCHLIST_3;
    XMFLOAT4X4 trans[1];
    XMStoreFloat4x4
    (
        &trans[0],
        XMMatrixTranspose(XMMatrixScaling(4.f, 4.f, 4.f))
    );
    RItem.dataLen = sizeof(XMFLOAT4X4)*_countof(trans);
    RItem.data = &trans[0];
    RItem.mat = "metal";
    mRenderItems[RItem.name] = RItem;
    mEngine.GetScene()->AddRenderItem
    (
        mMeshDescs["quad"].name,
        mShaderDescs["main"].name,
        "default",
        0,
        RItem
    );
}

void TSIcosahedronApp::OnKeyUp(WPARAM wParam, LPARAM lParam)
{
    FLEngineApp3::OnKeyUp(wParam, lParam);
    if (wParam == 'L')
    {
        if (mTessLod < 64.f)
        {
            mTessLod += 1.f;
        }
        else
        {
            mTessLod = 1.f;
        }
    }
}