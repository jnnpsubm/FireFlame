#include "GSExplosionApp.h"

void GSExplosionApp::PreInitialize()
{
    mEngine.SetDefaultClearColor({ 0.7f, 0.7f, 0.7f, 1.0f });
}

void GSExplosionApp::Initialize()
{
    mUndeadLegionLoader.load("D:\\DSIII_CHR\\c3040\\c3040.flver");

    AddShaders();
    AddPSOs();
    AddTextures();
    AddMaterials();
    AddMeshs();
    AddRenderItems();

    mPasses.push_back("DefaultPass");
    mEngine.GetScene()->AddPass(mPasses[0]);
}

void GSExplosionApp::Update(float time_elapsed)
{
    OnKeyboardInput(time_elapsed);
    UpdateCamera(time_elapsed);
    UpdateMainPassCB(time_elapsed);
}

void GSExplosionApp::UpdateMainPassCB(float time_elapsed)
{
    using namespace DirectX;

    // for icosahedron
    auto& passCB = mPassCBs["main"];
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
    mEngine.GetScene()->UpdateShaderPassCBData(mShaderDescs["explosion"].name, sizeof(PassConstantsLight), &passCB);
    mEngine.GetScene()->UpdateShaderPassCBData(mShaderDescs["explosion2"].name, sizeof(PassConstantsLight), &passCB);
}

void GSExplosionApp::AddShaders()
{
    AddShaderExplosion();
    AddShaderExplosion2();
}

void GSExplosionApp::AddShaderExplosion()
{
    using namespace FireFlame;

    auto& shaderDesc = mShaderDescs["explosion"];
    shaderDesc.name = "explosion";
    shaderDesc.objCBSize = sizeof(ObjectConsts2);
    shaderDesc.materialCBSize = sizeof(MaterialConstants);
    shaderDesc.passCBSize = sizeof(PassConstantsLight);
    shaderDesc.objParamIndex = 1;
    shaderDesc.matParamIndex = 2;
    shaderDesc.passParamIndex = 3;
    shaderDesc.AddVertexInput("POSITION", FireFlame::VERTEX_FORMAT_FLOAT3);
    shaderDesc.AddVertexInput("NORMAL", FireFlame::VERTEX_FORMAT_FLOAT3);
    shaderDesc.AddShaderStage(L"Shaders\\explosion.hlsl", Shader_Type::VS, "VS", "vs_5_0");
    shaderDesc.AddShaderStage(L"Shaders\\explosion.hlsl", Shader_Type::GS, "GS", "gs_5_0");
    shaderDesc.AddShaderStage(L"Shaders\\explosion.hlsl", Shader_Type::PS, "PS", "ps_5_0");
    mEngine.GetScene()->AddShader(shaderDesc);
}

void GSExplosionApp::AddShaderExplosion2()
{
    using namespace FireFlame;

    auto& shaderDesc = mShaderDescs["explosion2"];
    shaderDesc.name = "explosion2";
    shaderDesc.objCBSize = sizeof(ObjectConsts2);
    shaderDesc.materialCBSize = sizeof(MaterialConstants2);
    shaderDesc.passCBSize = sizeof(PassConstantsLight);
    shaderDesc.texSRVDescriptorTableSize = 4;
    shaderDesc.ParamDefault();
    shaderDesc.AddVertexInput("POSITION", FireFlame::VERTEX_FORMAT_FLOAT3);
    shaderDesc.AddVertexInput("NORMAL", FireFlame::VERTEX_FORMAT_FLOAT3);
    shaderDesc.AddVertexInput("TEXTURE", FireFlame::VERTEX_FORMAT_FLOAT2);
    shaderDesc.AddShaderStage(L"Shaders\\explosion2.hlsl", Shader_Type::VS, "VS", "vs_5_0");
    shaderDesc.AddShaderStage(L"Shaders\\explosion2.hlsl", Shader_Type::GS, "GS", "gs_5_0");
    shaderDesc.AddShaderStage(L"Shaders\\explosion2.hlsl", Shader_Type::PS, "PS", "ps_5_0");
    mEngine.GetScene()->AddShader(shaderDesc);
}

void GSExplosionApp::AddPSOs()
{
    using namespace FireFlame;

    PSODesc descIcosahedron(mShaderDescs["explosion"].name);
    descIcosahedron.opaque = false;
    mEngine.GetScene()->AddPSO("explosion_default", descIcosahedron);

    PSODesc desc(mShaderDescs["explosion2"].name);
    desc.default();
    desc.opaque = false;
    desc.cullMode = Cull_Mode::None;
    mEngine.GetScene()->AddPSO("undead_legion", desc);
    desc.default();
    desc.opaque = false;
    desc.alpha2Coverage = true;
    desc.cullMode = Cull_Mode::None;
    mEngine.GetScene()->AddPSO("undead_legion_hair", desc);
}

void GSExplosionApp::AddTextures()
{
    AddTexturesUndeadLegion();
}

void GSExplosionApp::AddTexturesUndeadLegion()
{
    mEngine.GetScene()->AddTexture
    (
        "ul_AM",
        L"D:\\DSIII_CHR\\c3040\\c3040\\c3040_AM_a.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "ul_AM_r",
        L"D:\\DSIII_CHR\\c3040\\c3040\\c3040_AM_r.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "ul_cape",
        L"D:\\DSIII_CHR\\c3040\\c3040\\c3040_BD_cape_a.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "ul_cape_r",
        L"D:\\DSIII_CHR\\c3040\\c3040\\c3040_BD_cape_r.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "ul_chest",
        L"D:\\DSIII_CHR\\c3040\\c3040\\c3040_BD_Chest_a.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "ul_chest_r",
        L"D:\\DSIII_CHR\\c3040\\c3040\\c3040_BD_Chest_r.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "ul_chest_d",
        L"D:\\DSIII_CHR\\c3040\\c3040\\c3040_BD_Chest_d.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "ul_HD",
        L"D:\\DSIII_CHR\\c3040\\c3040\\c3040_HD_a.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "ul_HD_r",
        L"D:\\DSIII_CHR\\c3040\\c3040\\c3040_HD_r.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "ul_HD_d",
        L"D:\\DSIII_CHR\\c3040\\c3040\\c3040_HD_d.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "ul_hair",
        L"D:\\DSIII_CHR\\c3040\\c3040\\C3040_HD_hair_a.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "ul_hair_r",
        L"D:\\DSIII_CHR\\c3040\\c3040\\C3040_HD_hair_r.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "lgpart01",
        L"D:\\DSIII_CHR\\c3040\\c3040\\c3040_LG_part01_a.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "ul_lgpart01_r",
        L"D:\\DSIII_CHR\\c3040\\c3040\\c3040_LG_part01_r.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "ul_lgpart01_d",
        L"D:\\DSIII_CHR\\c3040\\c3040\\c3040_LG_part01_d.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "ul_lgpart02",
        L"D:\\DSIII_CHR\\c3040\\c3040\\c3040_LG_part02_a.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "ul_lgpart02_r",
        L"D:\\DSIII_CHR\\c3040\\c3040\\c3040_LG_part02_r.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "ul_lgpart02_d",
        L"D:\\DSIII_CHR\\c3040\\c3040\\c3040_LG_part02_d.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "ul_weapon1",
        L"D:\\DSIII_CHR\\c3040\\c3040\\c3040_WP_A_0113_a.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "ul_weapon1_r",
        L"D:\\DSIII_CHR\\c3040\\c3040\\c3040_WP_A_0113_r.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "ul_weapon2",
        L"D:\\DSIII_CHR\\c3040\\c3040\\c3040_WP_A_0620_a.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "ul_weapon2_r",
        L"D:\\DSIII_CHR\\c3040\\c3040\\c3040_WP_A_0620_r.dds"
    );
    mTexUL.push_back("ul_AM");
    mTexUL.push_back("ul_cape");
    mTexUL.push_back("ul_chest");
    mTexUL.push_back("ul_HD");
    mTexUL.push_back("ul_hair");
    mTexUL.push_back("lgpart01");
    mTexUL.push_back("ul_lgpart02");
    mTexUL.push_back("ul_weapon1");
    mTexUL.push_back("ul_weapon2");

    mTexMapUL[0] = "";
    mTexMapUL[1] = "";
    mTexMapUL[2] = "";
    mTexMapUL[3] = "";
    mTexMapUL[4] = "";
    mTexMapUL[5] = "";
    mTexMapUL[6] = "";
    mTexMapUL[7] = "";
    mTexMapUL[8] = "";
    mTexMapUL[9] = "";
    mTexMapUL[10] = "";
    mTexMapUL[11] = "";
    mTexMapUL[12] = "";
}

void GSExplosionApp::AddMaterials()
{
    AddMaterialDragon();
    AddMaterialUndeadLegion();
}

void GSExplosionApp::AddMaterialDragon()
{
    auto& material = mMaterials["dragon"];
    material.Name = "dragon";
    material.DiffuseAlbedo = FireFlame::Vector4f(0.7f, 0.7f, 0.7f, 1.0f);
    material.FresnelR0 = FireFlame::Vector3f(0.3f, 0.3f, 0.3f);
    material.Roughness = 0.99f;
    mEngine.GetScene()->AddMaterial
    (
        material.Name,
        mShaderDescs["explosion"].name,
        sizeof(MaterialConstants), &material
    );
}

void GSExplosionApp::AddMaterialUndeadLegion()
{
    auto par_count = mUndeadLegionLoader.get_part_count();
    for (size_t i = 0; i < mTexUL.size(); i++)
    {
        std::string texname = mTexUL[i];
        std::string matName = "test_ul_mat_" + texname;
        auto& material = mMaterials2[matName];
        material.Name = matName;
        std::string specularTex, diffuseTex;
        specularTex = texname + "_r";
        if (i == 8)
        {
            material.DiffuseAlbedo = { 2.f, 2.f, 2.f, 1.0f };
            material.FresnelR0 = { 0.9f,0.9f,0.9f };
            material.Roughness = 0.0f;

            material.UseTexture = 7;
        }
        else if (i == 7)
        {
            material.DiffuseAlbedo = { 2.5f, 2.5f, 2.5f, 1.0f };
            material.FresnelR0 = { 0.7f,0.7f,0.7f };
            material.Roughness = 0.1f;

            material.UseTexture = 5;
        }
        else if (i == 6) // boots
        {
            material.DiffuseAlbedo = { 2.5f, 2.5f, 2.5f, 1.0f };
            material.FresnelR0 = { 0.7f,0.7f,0.7f };
            material.Roughness = 0.1f;

            material.UseTexture = 5;
            diffuseTex = texname + "_d";
        }
        else if (i == 5) // leg part1
        {
            material.DiffuseAlbedo = { 10.0f, 10.0f, 10.0f, 1.0f };
            material.FresnelR0 = { 0.7f,0.7f,0.7f };
            material.Roughness = 0.1f;

            material.UseTexture = 6;
            diffuseTex = texname + "_d";
        }
        else if (i == 4) // hair
        {
            material.DiffuseAlbedo = { 4.f, 4.f, 4.f, 1.0f };
            material.FresnelR0 = { 0.3f,0.3f,0.3f };
            material.Roughness = 0.1f;
            material.UseTexture = 5;
        }
        else if (i == 3) // hat
        {
            material.DiffuseAlbedo = { 0.5f, 0.5f, 0.5f, 1.0f };
            material.FresnelR0 = { 0.3f,0.3f,0.3f };
            material.Roughness = 0.1f;
            material.UseTexture = 3;
            diffuseTex = texname + "_d";
        }
        else if (i == 2) // chest
        {
            material.DiffuseAlbedo = { 2.f, 2.f, 2.f, 1.0f };
            material.FresnelR0 = { 0.8f,0.8f,0.8f };
            material.Roughness = 0.6f;

            material.UseTexture = 5;
            diffuseTex = texname + "_d";
        }
        else if (i == 1) // cape
        {
            material.DiffuseAlbedo = { 5.0f, 5.0f, 5.0f, 5.0f };
            material.FresnelR0 = { 0.5f,0.5f,0.5f };
            material.Roughness = 0.6f;

            material.UseTexture = 1;
            material.UseSpecularMap = 1;
        }
        else if (i == 0) // gloves
        {
            material.DiffuseAlbedo = { 1.5f, 1.5f, 1.5f, 1.f };
            material.FresnelR0 = { 0.7f,0.7f,0.7f };
            material.Roughness = 0.0f;

            material.UseTexture = 4;
        }
        else
        {
            material.DiffuseAlbedo = { 5.0f, 5.0f, 5.0f, 5.0f };
            material.FresnelR0 = { 0.05f,0.05f,0.05f };
            material.Roughness = 0.6f;

            material.UseTexture = 1;
            material.UseSpecularMap = 1;
        }

        mTestMatUL.push_back(material.Name);
        mEngine.GetScene()->AddMaterial
        (
        {
            material.Name,
            mShaderDescs["explosion2"].name,
            {
                { texname, FireFlame::SRV_DIMENSION::TEXTURE2D },
                { specularTex, FireFlame::SRV_DIMENSION::TEXTURE2D },
                { diffuseTex, FireFlame::SRV_DIMENSION::TEXTURE2D }
            },
            sizeof(MaterialConstants2), &material
        }
        );
    }
    mPartMatMap["ul_model_0"] = 1;
    mPartMatMap["ul_model_1"] = 1;
    mPartMatMap["ul_model_2"] = 5;
    mPartMatMap["ul_model_3"] = 1;
    mPartMatMap["ul_model_4"] = 0;
    mPartMatMap["ul_model_5"] = 6;
    mPartMatMap["ul_model_6"] = 5;
    mPartMatMap["ul_model_7"] = 2;
    mPartMatMap["ul_model_8"] = 3;
    mPartMatMap["ul_model_9"] = 7;
    mPartMatMap["ul_model_10"] = 4;
    mPartMatMap["ul_model_11"] = 8;
    mPartMatMap["ul_model_12"] = 8;
}

void GSExplosionApp::AddMeshs()
{
    AddMeshDragon();
    AddMeshUndeadLegion();
}

void GSExplosionApp::AddMeshDragon()
{
    std::vector<FireFlame::FLVertexNormal> vertices;
    std::vector<std::uint32_t>             indices;
    if (FireFlame::PLYLoader::Load("..\\..\\Resources\\geometry\\dragon_remeshed.ply", vertices, indices))
    {
        auto& mesh = mMeshDescs["dragon"];
        mesh.name = "dragon";
        mesh.indexCount = (unsigned int)indices.size();
        mesh.indexFormat = FireFlame::Index_Format::UINT32;
        mesh.indices = indices.data();

        mesh.vertexDataCount.push_back((unsigned int)vertices.size());
        mesh.vertexDataSize.push_back(sizeof(FireFlame::FLVertexNormal));
        mesh.vertexData.push_back(vertices.data());

        // sub meshes
        mesh.subMeshs.emplace_back("All", (UINT)indices.size());
        mEngine.GetScene()->AddPrimitive(mesh);

        std::cout << "vertex count:" << vertices.size() << std::endl;
        std::cout << "face count:" << indices.size() / 3 << std::endl;

        // decide scale
        float maxX = 0.f, maxY = 0.f, maxZ = 0.f;
        float minY = (std::numeric_limits<float>::max)();
        for (size_t i = 0; i < vertices.size(); ++i)
        {
            if (std::abs(vertices[i].Pos.x) > maxX)
                maxX = std::abs(vertices[i].Pos.x);
            if (std::abs(vertices[i].Pos.y) > maxY)
                maxY = std::abs(vertices[i].Pos.y);
            if (std::abs(vertices[i].Pos.z) > maxZ)
                maxZ = std::abs(vertices[i].Pos.z);

            if (vertices[i].Pos.y < minY)
                minY = vertices[i].Pos.y;
        }
        spdlog::get("console")->info
        (
            "mRadius:{0:f},maxX:{1:f},maxY:{2:f},maxZ:{3:f},minY:{4:f}",
            mRadius, maxX, maxY, maxZ, minY
        );
        mModelScale = mRadius / (((std::max)((std::max)(maxX, maxY), maxZ) - minY)*4.f);
        mModelTransY = -minY*mModelScale;
    }
}

void GSExplosionApp::AddMeshUndeadLegion()
{
    AddMeshULPart(0, true);
    AddMeshULPart(1, true);
    AddMeshULPart(2, true);
    AddMeshULPart(3, true);
    AddMeshULPart(4, true);
    AddMeshULPart(5, true);
    AddMeshULPart(6, true);
    AddMeshULPart(7, true);
    AddMeshULPart(8, true);
    AddMeshULPart(9, true);
    AddMeshULPart(10, true);
    AddMeshULPart(11, true);
    AddMeshULPart(12, true);
}

void GSExplosionApp::AddMeshULPart(size_t part, bool reverseNormal)
{
    using namespace FireFlame;

    // Mesh
    std::vector<FireFlame::FLVertexNormalTex> vertices;
    auto& rawUVs = mUndeadLegionLoader.get_uvs();
    auto& rawVertices = mUndeadLegionLoader.get_vertices();
    auto& rawIndices = mUndeadLegionLoader.get_indices();

    std::vector<std::uint32_t> indices;
    for (const auto& index : rawIndices[part])
    {
        indices.push_back(index);
    }

    vertices.reserve(rawVertices[part].size());
    for (size_t i = 0; i < rawVertices[part].size(); i++)
    {
        vertices.emplace_back
        (
            rawVertices[part][i].x, rawVertices[part][i].y, rawVertices[part][i].z,
            0.f, 0.f, 0.f,
            rawUVs[part][i].u, -rawUVs[part][i].v
        );
    }

    // begin normals
    for (size_t i = 0; i < indices.size() / 3; i++)
    {
        auto& v0 = vertices[indices[i * 3 + 0]];
        auto& v1 = vertices[indices[i * 3 + 1]];
        auto& v2 = vertices[indices[i * 3 + 2]];
        auto e0 = v1.Pos - v0.Pos;
        auto e1 = v2.Pos - v0.Pos;
        auto normal = FireFlame::Vector3Cross(reverseNormal ? e1 : e0, reverseNormal ? e0 : e1);

        v0.Normal += normal;
        v1.Normal += normal;
        v2.Normal += normal;
    }
    for (auto& vertex : vertices)
    {
        vertex.Normal.Normalize();
    }
    // end normals

    std::string meshName = "ul_model_" + std::to_string(part);
    auto& mesh = mMeshDescs[meshName];
    mesh.name = meshName;
    mesh.primitiveTopology = Primitive_Topology::TriangleList;
    mesh.indexCount = (unsigned int)indices.size();
    mesh.indexFormat = Index_Format::UINT32;
    mesh.indices = indices.data();

    mesh.vertexDataCount.push_back((unsigned int)vertices.size());
    mesh.vertexDataSize.push_back(sizeof(FLVertexNormalTex));
    mesh.vertexData.push_back(vertices.data());

    // sub meshes
    mesh.subMeshs.emplace_back("All", (UINT)indices.size());
    mEngine.GetScene()->AddPrimitive(mesh);
}

void GSExplosionApp::AddRenderItems()
{
    AddRenderItemsDragon();
    AddRenderItemsUL();
}

void GSExplosionApp::AddRenderItemsDragon()
{
    using namespace DirectX;

    FireFlame::stRenderItemDesc RItem("dragon01", mMeshDescs["dragon"].subMeshs[0]);
    RItem.mat = "dragon";
    XMStoreFloat4x4
    (
        &mObjectConsts.World,
        XMMatrixTranspose
        (
            XMMatrixRotationZ(FireFlame::MathHelper::FL_PIDIV2)*
            XMMatrixScaling(mModelScale, mModelScale, mModelScale)*
            XMMatrixTranslation(0.0f, mModelTransY, 0.0f)
        )
    );
    XMStoreFloat4x4
    (
        &mObjectConsts.TexTransform,
        XMMatrixIdentity()
    );
    RItem.dataLen = sizeof(ObjectConsts2);
    RItem.data = &mObjectConsts;
    mEngine.GetScene()->AddRenderItem
    (
        mMeshDescs["dragon"].name,
        mShaderDescs["explosion"].name,
        "explosion_default",
        RItem
    );
    spdlog::get("console")->info("Added dragon......StartExplosion:{0}", mObjectConsts.StartExplosion);
}

void GSExplosionApp::AddRenderItemsUL()
{
    using namespace DirectX;
    auto parts = mUndeadLegionLoader.get_part_count();
    for (size_t part = 0; part < parts; part++)
    {
        std::string meshName = "ul_model_" + std::to_string(part);
        std::string matName = mTestMatUL[mPartMatMap[meshName]];
        std::string renderItemName = meshName + "_inst" + std::to_string(0);

        // add render item
        FireFlame::stRenderItemDesc RItem(renderItemName, mMeshDescs[meshName].subMeshs[0]);
        RItem.mat = matName;
        ObjectConsts2 objConsts;
        DirectX::XMMATRIX fkRotate = XMMatrixRotationY(0.0f);
        DirectX::XMMATRIX fkScale = XMMatrixScaling(0.02f, 0.02f, 0.02f);
        DirectX::XMMATRIX fkOffset = XMMatrixTranslation
        (
            2.f, 0.0f, 0.f
        );
        DirectX::XMMATRIX fkWorld = fkRotate*fkScale*fkOffset;
        XMStoreFloat4x4
        (
            &objConsts.World,
            XMMatrixTranspose(fkWorld)
        );
        XMStoreFloat4x4
        (
            &objConsts.TexTransform,
            XMMatrixIdentity()
        );
        RItem.dataLen = sizeof(ObjectConsts2);
        RItem.data = &objConsts;
        mRenderItems[RItem.name] = RItem;
        mULPartsConsts[renderItemName] = objConsts;
        mEngine.GetScene()->AddRenderItem
        (
            mMeshDescs[meshName].name,
            mShaderDescs["explosion2"].name,
            part == 17 ? "undead_legion_hair" : "undead_legion",
            RItem
        );
    }
}

void GSExplosionApp::OnKeyUp(WPARAM wParam, LPARAM lParam)
{
    FLEngineApp3::OnKeyUp(wParam, lParam);
    if (wParam == 'E')
    {
        mObjectConsts.StartExplosion = !mObjectConsts.StartExplosion;
        mObjectConsts.StartExplosionTime = mEngine.TotalTime();
        mEngine.GetScene()->UpdateRenderItemCBData("dragon01", sizeof(ObjectConsts2), &mObjectConsts);

        for (auto& itObjConst : mULPartsConsts)
        {
            auto& objCBData = itObjConst.second;
            objCBData.StartExplosion = !objCBData.StartExplosion;
            objCBData.StartExplosionTime = mEngine.TotalTime();
            mEngine.GetScene()->UpdateRenderItemCBData(itObjConst.first, sizeof(ObjectConsts2), &objCBData);
        }
        spdlog::get("console")->info("Start explosion......");
    }
}
