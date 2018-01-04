#include "BezierPatchApp.h"

void BezierPatchApp::PreInitialize() {}

void BezierPatchApp::Initialize()
{
    AddShaders();
    AddPSOs();
    AddMaterials();
    AddMeshs();
    AddRenderItems();

    mPasses.push_back("DefaultPass");
    mEngine.GetScene()->AddPass(mPasses[0]);
}

void BezierPatchApp::Update(float time_elapsed)
{
    FLEngineApp3::Update(time_elapsed);
}

void BezierPatchApp::UpdateMainPassCB(float time_elapsed)
{
    FLEngineApp3::UpdateMainPassCB(time_elapsed);
}

void BezierPatchApp::AddShaders()
{
    using namespace FireFlame;

    auto& shaderDesc = mShaderDescs["main"];
    shaderDesc.name = "main";
    shaderDesc.objCBSize = sizeof(ObjectConsts);
    shaderDesc.passCBSize = sizeof(PassConstants);
    shaderDesc.materialCBSize = sizeof(MaterialConstants);
    shaderDesc.ParamDefault();

    shaderDesc.AddVertexInput("POSITION", FireFlame::VERTEX_FORMAT_FLOAT3);
    shaderDesc.AddShaderStage(L"Shaders\\main.hlsl", Shader_Type::VS, "VS", "vs_5_0");
    shaderDesc.AddShaderStage(L"Shaders\\main.hlsl", Shader_Type::HS, "HS", "hs_5_0");
    shaderDesc.AddShaderStage(L"Shaders\\main.hlsl", Shader_Type::DS, "DS", "ds_5_0");
    shaderDesc.AddShaderStage(L"Shaders\\main.hlsl", Shader_Type::PS, "PS", "ps_5_0");

    mEngine.GetScene()->AddShader(shaderDesc);
}

void BezierPatchApp::AddPSOs()
{
    using namespace FireFlame;

    PSODesc descDefault(mShaderDescs["main"].name);
    descDefault.topologyType = Primitive_Topology_Type::Patch;
    mEngine.GetScene()->AddPSO("default", descDefault);
}

void BezierPatchApp::AddMaterials()
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

void BezierPatchApp::AddMeshs()
{
    using namespace FireFlame;

    std::array<FLVertex, 16> vertices =
    {
        // Row 0
        FLVertex(-10.0f, -10.0f, +15.0f),
        FLVertex(-5.0f,  0.0f, +15.0f),
        FLVertex(+5.0f,  0.0f, +15.0f),
        FLVertex(+10.0f, 0.0f, +15.0f),

        // Row 1
        FLVertex(-15.0f, 0.0f, +5.0f),
        FLVertex(-5.0f,  0.0f, +5.0f),
        FLVertex(+5.0f,  20.0f, +5.0f),
        FLVertex(+15.0f, 0.0f, +5.0f),

        // Row 2
        FLVertex(-15.0f, 0.0f, -5.0f),
        FLVertex(-5.0f,  0.0f, -5.0f),
        FLVertex(+5.0f,  0.0f, -5.0f),
        FLVertex(+15.0f, 0.0f, -5.0f),

        // Row 3
        FLVertex(-10.0f, 10.0f, -15.0f),
        FLVertex(-5.0f,  0.0f, -15.0f),
        FLVertex(+5.0f,  0.0f, -15.0f),
        FLVertex(+25.0f, 10.0f, -15.0f)
    };

    std::array<std::int16_t, 16> indices =
    {
        0, 1, 2, 3,
        4, 5, 6, 7,
        8, 9, 10, 11,
        12, 13, 14, 15
    };

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

void BezierPatchApp::AddRenderItems()
{
    using namespace DirectX;

    FireFlame::stRenderItemDesc RItem("land", mMeshDescs["quad"].subMeshs[0]);
    RItem.topology = FireFlame::Primitive_Topology::CONTROL_POINT_PATCHLIST_16;
    XMFLOAT4X4 trans[1];
    XMStoreFloat4x4
    (
        &trans[0],
        XMMatrixTranspose(XMMatrixIdentity())
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

void BezierPatchApp::OnKeyUp(WPARAM wParam, LPARAM lParam)
{
    FLEngineApp3::OnKeyUp(wParam, lParam);
}