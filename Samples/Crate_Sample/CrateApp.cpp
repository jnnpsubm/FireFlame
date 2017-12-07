#include "CrateApp.h"

void CrateApp::Initialize() 
{
    AddShaders();
    AddTextures();
    AddMaterials();
    AddGeoMeshs();
    AddRenderItems();

    mPasses.push_back("DefaultPass");
    mEngine.GetScene()->AddPass(mPasses[0]);
}

void CrateApp::UpdateMainPassCB(float time_elapsed)
{
    using namespace FireFlame;
    float totalTime = mEngine.TotalTime();

    mMainPassCB.AmbientLight = { 0.25f, 0.25f, 0.35f, 1.0f };
    mMainPassCB.Lights[0].Direction = { 0.57735f, -0.57735f, 0.57735f };
    mMainPassCB.Lights[0].Strength = { 0.6f, 0.6f, 0.6f };
    mMainPassCB.Lights[1].Direction = { -0.57735f, -0.57735f, 0.57735f };
    mMainPassCB.Lights[1].Strength = { 0.3f, 0.3f, 0.3f };
    mMainPassCB.Lights[2].Direction = { 0.0f, -0.707f, -0.707f };
    mMainPassCB.Lights[2].Strength = { 0.15f, 0.15f, 0.15f };
}

void CrateApp::AddShaders()
{
    using namespace FireFlame;
    mShaderDesc.name = "CrateApp";
    mShaderDesc.objCBSize = sizeof(ObjectConsts);
    mShaderDesc.passCBSize = sizeof(PassConstants);
    mShaderDesc.materialCBSize = sizeof(MaterialConstants);
    mShaderDesc.ParamDefault();
    mShaderDesc.AddVertexInput("POSITION", FireFlame::VERTEX_FORMAT_FLOAT3);
    mShaderDesc.AddVertexInput("NORMAL", FireFlame::VERTEX_FORMAT_FLOAT3);
    mShaderDesc.AddVertexInput("TEXCOORD", FireFlame::VERTEX_FORMAT_FLOAT2);
    mShaderDesc.AddShaderStage(L"Shaders\\CrateApp.hlsl", Shader_Type::VS, "VS", "vs_5_0");
    mShaderDesc.AddShaderStage(L"Shaders\\CrateApp.hlsl", Shader_Type::PS, "PS", "ps_5_0");

    mEngine.GetScene()->AddShader(mShaderDesc);
}

void CrateApp::AddTextures()
{
    mEngine.GetScene()->AddTexture
    (
        "woodCrateTex", 
        L"..\\..\\Resources\\Textures\\WoodCrate01.dds"
    );
}

void CrateApp::AddMaterials()
{
    auto& woodCrate = mMaterials["woodCrate"];
    woodCrate.name = "woodCrate";
    //woodCrate->DiffuseSrvHeapIndex = 0;
    woodCrate.DiffuseAlbedo = FireFlame::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    woodCrate.FresnelR0 = FireFlame::Vector3f(0.01f, 0.01f, 0.01f);
    woodCrate.Roughness = 0.25f;
    mEngine.GetScene()->AddMaterial
    (
        "woodCrate", 
        mShaderDesc.name, "woodCrateTex",
        sizeof(MaterialConstants), &woodCrate
    );
}

void CrateApp::AddGeoMeshs()
{
    using namespace FireFlame;
    GeometryGenerator geoGen;
    GeometryGenerator::MeshData box = geoGen.CreateBox(1.f, 1.f, 1.f, 3);

    std::vector<FireFlame::FLVertexNormalTex> vertices(box.Vertices.size());
    for (size_t i = 0; i < box.Vertices.size(); ++i)
    {
        vertices[i].Pos = box.Vertices[i].Position;
        vertices[i].Normal = box.Vertices[i].Normal;
        vertices[i].Tex = box.Vertices[i].TexC;
    }

    std::vector<std::uint32_t> indices = box.Indices32;

    mMeshDesc.emplace_back();
    mMeshDesc.back().name = "Crate";
    mMeshDesc.back().primitiveTopology = Primitive_Topology::TriangleList;
    mMeshDesc.back().indexCount = (unsigned int)indices.size();
    mMeshDesc.back().indexFormat = Index_Format::UINT32;
    mMeshDesc.back().indices = indices.data();

    mMeshDesc.back().vertexDataCount.push_back((unsigned int)vertices.size());
    mMeshDesc.back().vertexDataSize.push_back(sizeof(FLVertexNormalTex));
    mMeshDesc.back().vertexData.push_back(vertices.data());

    // sub meshes
    mMeshDesc.back().subMeshs.emplace_back("All", (UINT)box.Indices32.size());
    
    mEngine.GetScene()->AddPrimitive(mMeshDesc.back());
}

void CrateApp::AddRenderItems() 
{
    using namespace DirectX;

    FireFlame::stRenderItemDesc RItem("Box1", mMeshDesc[0].subMeshs[0]);
    RItem.mat = "woodCrate";
    XMFLOAT4X4 trans[2];
    XMStoreFloat4x4
    (
        &trans[0],
        XMMatrixTranspose(XMMatrixIdentity())
    );
    XMStoreFloat4x4
    (
        &trans[1],
        XMMatrixTranspose(XMMatrixIdentity())
    );
    RItem.dataLen = sizeof(XMFLOAT4X4)*_countof(trans);
    RItem.data = &trans[0];
    mRenderItems.emplace_back(RItem);
    mEngine.GetScene()->AddRenderItem
    (
        mMeshDesc[0].name,
        mShaderDesc.name,
        RItem
    );
}

void CrateApp::OnKeyUp(WPARAM wParam, LPARAM lParam) 
{
    auto& dragon = mMaterials["dragon"];
    /*dragon.name = "dragon";
    dragon.DiffuseAlbedo = { 0.1f, 0.1f, 0.1f, 1.0f };
    dragon.FresnelR0 = { 0.9f,0.9f,0.1f };
    dragon.Roughness = 0.0f;*/
    if (wParam == '1') 
    {
        dragon.DiffuseAlbedo.x += 0.05f;
        if (dragon.DiffuseAlbedo.x > 1.0f)
        {
            dragon.DiffuseAlbedo.x = 1.0f;
        }
        mEngine.GetScene()->UpdateMaterialCBData(dragon.name, sizeof(MaterialConstants), &dragon);
    }
    else if (wParam == '2')
    {
        dragon.DiffuseAlbedo.x -= 0.05f;
        if (dragon.DiffuseAlbedo.x < 0.0f)
        {
            dragon.DiffuseAlbedo.x = 0.0f;
        }
        mEngine.GetScene()->UpdateMaterialCBData(dragon.name, sizeof(MaterialConstants), &dragon);
    }
    else if (wParam == '3')
    {
        dragon.DiffuseAlbedo.y += 0.05f;
        if (dragon.DiffuseAlbedo.y > 1.0f)
        {
            dragon.DiffuseAlbedo.y = 1.0f;
        }
        mEngine.GetScene()->UpdateMaterialCBData(dragon.name, sizeof(MaterialConstants), &dragon);
    }
    else if (wParam == '4')
    {
        dragon.DiffuseAlbedo.y -= 0.05f;
        if (dragon.DiffuseAlbedo.y < 0.0f)
        {
            dragon.DiffuseAlbedo.y = 0.0f;
        }
        mEngine.GetScene()->UpdateMaterialCBData(dragon.name, sizeof(MaterialConstants), &dragon);
    }
    else if (wParam == '5')
    {
        dragon.DiffuseAlbedo.z += 0.05f;
        if (dragon.DiffuseAlbedo.z > 1.0f)
        {
            dragon.DiffuseAlbedo.z = 1.0f;
        }
        mEngine.GetScene()->UpdateMaterialCBData(dragon.name, sizeof(MaterialConstants), &dragon);
    }
    else if (wParam == '6')
    {
        dragon.DiffuseAlbedo.z -= 0.05f;
        if (dragon.DiffuseAlbedo.z < 0.0f)
        {
            dragon.DiffuseAlbedo.z = 0.0f;
        }
        mEngine.GetScene()->UpdateMaterialCBData(dragon.name, sizeof(MaterialConstants), &dragon);
    }
    else if (wParam == '7')
    {
        dragon.FresnelR0.x += 0.05f;
        if (dragon.FresnelR0.x > 1.0f)
        {
            dragon.FresnelR0.x = 1.0f;
        }
        mEngine.GetScene()->UpdateMaterialCBData(dragon.name, sizeof(MaterialConstants), &dragon);
    }
    else if (wParam == '8')
    {
        dragon.FresnelR0.x -= 0.05f;
        if (dragon.FresnelR0.x < 0.0f)
        {
            dragon.FresnelR0.x = 0.0f;
        }
        mEngine.GetScene()->UpdateMaterialCBData(dragon.name, sizeof(MaterialConstants), &dragon);
    }
    else if (wParam == '9')
    {
        dragon.FresnelR0.y += 0.05f;
        if (dragon.FresnelR0.y > 1.0f)
        {
            dragon.FresnelR0.y = 1.0f;
        }
        mEngine.GetScene()->UpdateMaterialCBData(dragon.name, sizeof(MaterialConstants), &dragon);
    }
    else if (wParam == '0')
    {
        dragon.FresnelR0.y -= 0.05f;
        if (dragon.FresnelR0.y < 0.0f)
        {
            dragon.FresnelR0.y = 0.0f;
        }
        mEngine.GetScene()->UpdateMaterialCBData(dragon.name, sizeof(MaterialConstants), &dragon);
    }
    else if (wParam == 'Q')
    {
        dragon.FresnelR0.z += 0.05f;
        if (dragon.FresnelR0.z > 1.0f)
        {
            dragon.FresnelR0.z = 1.0f;
        }
        mEngine.GetScene()->UpdateMaterialCBData(dragon.name, sizeof(MaterialConstants), &dragon);
    }
    else if (wParam == 'W')
    {
        dragon.FresnelR0.z -= 0.05f;
        if (dragon.FresnelR0.z < 0.0f)
        {
            dragon.FresnelR0.z = 0.0f;
        }
        mEngine.GetScene()->UpdateMaterialCBData(dragon.name, sizeof(MaterialConstants), &dragon);
    }
    else if (wParam == 'T')
    {
        dragon.Roughness += 0.05f;
        if (dragon.Roughness > 1.0f)
        {
            dragon.Roughness = 1.0f;
        }
        mEngine.GetScene()->UpdateMaterialCBData(dragon.name, sizeof(MaterialConstants), &dragon);
    }
    else if (wParam == 'Y')
    {
        dragon.Roughness -= 0.05f;
        if (dragon.Roughness < 0.0f)
        {
            dragon.Roughness = 0.0f;
        }
        mEngine.GetScene()->UpdateMaterialCBData(dragon.name, sizeof(MaterialConstants), &dragon);
    }
    std::cout << "dragon diffuse albedo:" << dragon.DiffuseAlbedo << std::endl;
    std::cout << "dragon FresnelR0:" << dragon.FresnelR0 << std::endl;
    std::cout << "dragon roughness:" << dragon.Roughness << std::endl;
    FLEngineApp::OnKeyUp(wParam, lParam);
}