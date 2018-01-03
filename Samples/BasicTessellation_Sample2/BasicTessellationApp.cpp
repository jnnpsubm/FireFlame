#include "BasicTessellationApp.h"

void BasicTessellationApp::PreInitialize() {}

void BasicTessellationApp::Initialize()
{
    AddShaders();
    AddPSOs();
    AddMeshs();
    AddRenderItems();

    mPasses.push_back("DefaultPass");
    mEngine.GetScene()->AddPass(mPasses[0]);
}

void BasicTessellationApp::Update(float time_elapsed)
{
    FLEngineApp3::Update(time_elapsed);
}

void BasicTessellationApp::UpdateMainPassCB(float time_elapsed)
{
    FLEngineApp3::UpdateMainPassCB(time_elapsed);
}

void BasicTessellationApp::AddShaders()
{
    using namespace FireFlame;

    auto& shaderDesc = mShaderDescs["main"];
    shaderDesc.name = "main";
    shaderDesc.objCBSize = sizeof(ObjectConsts);
    shaderDesc.passCBSize = sizeof(PassConstantsBase);
    shaderDesc.materialCBSize = sizeof(MaterialConstants);
    shaderDesc.ParamDefault();

    shaderDesc.AddVertexInput("POSITION", FireFlame::VERTEX_FORMAT_FLOAT3);
    shaderDesc.AddShaderStage(L"Shaders\\Tessellation.hlsl", Shader_Type::VS, "VS", "vs_5_0");
    shaderDesc.AddShaderStage(L"Shaders\\Tessellation.hlsl", Shader_Type::HS, "HS", "hs_5_0");
    shaderDesc.AddShaderStage(L"Shaders\\Tessellation.hlsl", Shader_Type::DS, "DS", "ds_5_0");
    shaderDesc.AddShaderStage(L"Shaders\\Tessellation.hlsl", Shader_Type::PS, "PS", "ps_5_0");

    mEngine.GetScene()->AddShader(shaderDesc);
}

void BasicTessellationApp::AddPSOs()
{
    using namespace FireFlame;

    PSODesc descImage(mShaderDescs["image"].name);
    mEngine.GetScene()->AddPSO("pso_image", descImage);
}

void BasicTessellationApp::AddMeshs()
{
    using namespace FireFlame;

    //GeometryGenerator geoGen;
    //GeometryGenerator::MeshData grid = geoGen.CreateGrid(320.f, 320.f, 512, 512);

    //std::vector<FLVertexNormalTex> vertices(grid.Vertices.size());
    //for (size_t i = 0; i < grid.Vertices.size(); ++i)
    //{
    //    vertices[i].Pos = grid.Vertices[i].Position;
    //    vertices[i].Normal = grid.Vertices[i].Normal;
    //    vertices[i].Tex = grid.Vertices[i].TexC;
    //}
    //std::vector<std::uint32_t> indices = grid.Indices32;

    //auto& meshDesc = mMeshDescs["grid"];
    //meshDesc.name = "grid";
    //meshDesc.indexCount = (unsigned int)indices.size();
    //meshDesc.indexFormat = Index_Format::UINT32;
    //meshDesc.indices = indices.data();

    //meshDesc.vertexData.push_back(vertices.data());
    //meshDesc.vertexDataCount.push_back((unsigned)vertices.size());
    //meshDesc.vertexDataSize.push_back(sizeof(FLVertexNormalTex));

    //// sub meshes
    //meshDesc.subMeshs.emplace_back("All", (UINT)indices.size());
    //mEngine.GetScene()->AddPrimitive(meshDesc);
}

void BasicTessellationApp::AddRenderItems()
{
    using namespace DirectX;

    /*FireFlame::stRenderItemDesc RItem3("land", mMeshDescs["land"].subMeshs[0]);
    XMFLOAT4X4 trans[2];
    XMStoreFloat4x4
    (
        &trans[0],
        XMMatrixTranspose(XMMatrixIdentity())
    );
    XMStoreFloat4x4
    (
        &trans[1],
        XMMatrixTranspose(XMMatrixScaling(5.0f, 5.0f, 1.0f))
    );
    RItem3.dataLen = sizeof(XMFLOAT4X4)*_countof(trans);
    RItem3.data = &trans[0];
    RItem3.mat = "grass";
    mRenderItems[RItem3.name] = RItem3;
    mEngine.GetScene()->AddRenderItem
    (
        mMeshDescs["land"].name,
        mShaderDescs["main"].name,
        "ps_fogged",
        0,
        RItem3
    );*/
}

void BasicTessellationApp::OnKeyUp(WPARAM wParam, LPARAM lParam)
{
    FLEngineApp3::OnKeyUp(wParam, lParam);
}