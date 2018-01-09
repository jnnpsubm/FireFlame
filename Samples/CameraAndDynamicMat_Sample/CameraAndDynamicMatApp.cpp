#include "CameraAndDynamicMatApp.h"
#include <fstream>
#include <assert.h>

void CameraAndDynamicMatApp::PreInitialize() {}

void CameraAndDynamicMatApp::Initialize()
{
    AddShaders();
    AddPSOs();
    AddTextures();
    AddMaterials();
    AddMeshs();
    AddRenderItems();

    mPasses.push_back("DefaultPass");
    mEngine.GetScene()->AddPass(mPasses[0]);
}

void CameraAndDynamicMatApp::Update(float time_elapsed)
{
    FLEngineApp4::Update(time_elapsed);
}

void CameraAndDynamicMatApp::UpdateMainPassCB(float time_elapsed)
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

    mEngine.GetScene()->UpdateShaderPassCBData(mShaderDescs["main"].name, sizeof(PassConstantsLight), &passCB);
}

void CameraAndDynamicMatApp::AddShaders()
{
    using namespace FireFlame;

    auto& shaderDesc = mShaderDescs["main"];
    shaderDesc.name = "main";
    shaderDesc.objCBSize = sizeof(ObjectConsts);
    shaderDesc.passCBSize = sizeof(PassConstantsLight);
    shaderDesc.objParamIndex = 0;
    shaderDesc.passParamIndex = 1;
    shaderDesc.texSRVDescriptorTableSize = 4;
    shaderDesc.useDynamicMat = true;

    std::wstring strHlslFile = L"Shaders\\Main.hlsl";
    shaderDesc.AddVertexInput("POSITION", FireFlame::VERTEX_FORMAT_FLOAT3);
    shaderDesc.AddVertexInput("NORMAL", FireFlame::VERTEX_FORMAT_FLOAT3);
    shaderDesc.AddVertexInput("TEXCOORD", FireFlame::VERTEX_FORMAT_FLOAT2);
    shaderDesc.AddShaderStage(strHlslFile, Shader_Type::VS, "VS", "vs_5_1");
    shaderDesc.AddShaderStage(strHlslFile, Shader_Type::PS, "PS", "ps_5_1");

    mEngine.GetScene()->AddShader(shaderDesc);
}

void CameraAndDynamicMatApp::AddPSOs()
{
    using namespace FireFlame;

    PSODesc descDefault(mShaderDescs["main"].name);
    mEngine.GetScene()->AddPSO("default", descDefault);
}

void CameraAndDynamicMatApp::AddTextures()
{
    mEngine.GetScene()->AddTexture
    (
        "bricksTex",
        L"../../Resources/Textures/bricks.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "stoneTex",
        L"../../Resources/Textures/stone.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "tileTex",
        L"../../Resources/Textures/tile.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "crateTex",
        L"../../Resources/Textures/WoodCrate01.dds"
    );
    mEngine.GetScene()->AddTextureGroup
    (
        "main",
        { 
            FireFlame::TEX("bricksTex"),
            FireFlame::TEX("stoneTex"),
            FireFlame::TEX("tileTex"),
            FireFlame::TEX("crateTex")
        }
    );
}

void CameraAndDynamicMatApp::AddMaterials()
{
    auto& metal = mMaterials["metal"];
    metal.Name = "metal";
    metal.DiffuseAlbedo = FireFlame::Vector4f(0.2f, 0.5f, 0.5f, 1.0f);
    metal.FresnelR0 = FireFlame::Vector3f(0.75f, 0.75f, 0.75f);
    metal.Roughness = 0.725f;
    mEngine.GetScene()->AddMaterial
    (
        metal.Name,
        mShaderDescs["main"].name, "",
        sizeof(MaterialConstants), &metal
    );
}

void CameraAndDynamicMatApp::AddMeshs()
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

void CameraAndDynamicMatApp::AddRenderItems()
{
    using namespace DirectX;

    FireFlame::stRenderItemDesc RItem("model1", mMeshDescs["model1"].subMeshs[0]);
    RItem.mat = "model_mat";
    XMFLOAT4X4 trans[2];
    XMStoreFloat4x4
    (
        &trans[0],
        XMMatrixTranspose(XMMatrixTranslation(-6.0f, 0.0f, 0.0f))
    );
    XMStoreFloat4x4
    (
        &trans[1],
        XMMatrixTranspose(XMMatrixScaling(1.0f, 1.0f, 1.0f))
    );
    RItem.dataLen = sizeof(XMFLOAT4X4)*_countof(trans);
    RItem.data = &trans[0];
    mRenderItems[RItem.name] = RItem;
    mEngine.GetScene()->AddRenderItem
    (
        mMeshDescs["model1"].name,
        mShaderDescs["model_shader"].name,
        "model_pso",
        0,
        RItem
    );
}

void CameraAndDynamicMatApp::OnKeyUp(WPARAM wParam, LPARAM lParam)
{
    FLEngineApp4::OnKeyUp(wParam, lParam);
}