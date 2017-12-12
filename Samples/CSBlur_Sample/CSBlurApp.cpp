#include "CSBlurApp.h"

void CSBlurApp::PreInitialize()
{
    mEngine.SetDefaultClearColor(mPassCBs["main"].FogColor);
}

void CSBlurApp::Initialize()
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

void CSBlurApp::Update(float time_elapsed)
{
    FLEngineApp3::Update(time_elapsed);

    auto& passCB = mPassCBs["main"];
    mEngine.GetScene()->UpdateShaderPassCBData(mShaderDescs["tree"].name, sizeof(PassConstants), &passCB);

    if (mWaveStart) UpdateWaves();
}

void CSBlurApp::UpdateMainPassCB(float time_elapsed)
{
    FLEngineApp3::UpdateMainPassCB(time_elapsed);
}

void CSBlurApp::UpdateWaves()
{
    using namespace FireFlame;

    // update waves
    // Every quarter second, generate a random wave.
    static float t_base = 0.0f;
    if ((mEngine.TotalTime() - t_base) >= 0.25f)
    {
        t_base += 0.25f;

        int i = MathHelper::Rand(4, mWaves->RowCount() - 5);
        int j = MathHelper::Rand(4, mWaves->ColumnCount() - 5);

        float r = MathHelper::RandF(0.2f, 0.5f);

        mWaves->Disturb(i, j, r);
    }

    // Update the wave simulation.
    mWaves->Update(mEngine.DeltaTime());

    // Update the wave vertex buffer with the new solution.
    //auto currWavesVB = mCurrFrameResource->WavesVB.get();
    std::vector<FLVertexNormalTex> vertices(mWaves->VertexCount());
    for (int i = 0; i < mWaves->VertexCount(); ++i)
    {
        vertices[i].Pos = { mWaves->Position(i).x,mWaves->Position(i).y,mWaves->Position(i).z };
        vertices[i].Normal = { mWaves->Normal(i).x,mWaves->Normal(i).y, mWaves->Normal(i).z };
        vertices[i].Tex.x = 0.5f + vertices[i].Pos.x / mWaves->Width();
        vertices[i].Tex.y = 0.5f - vertices[i].Pos.z / mWaves->Depth();
    }
    mEngine.GetScene()->UpdateMeshCurrVBFrameRes
    (
        mMeshDescs["waves"].name, 0,
        sizeof(FLVertexNormalTex)*vertices.size(),
        vertices.data()
    );

    // Scroll the water material texture coordinates.
    auto& waterMat = mMaterials["water"];

    float& tu = waterMat.MatTransform[0][3];
    float& tv = waterMat.MatTransform[1][3];

    tu += 0.1f * mEngine.DeltaTime();
    tv += 0.02f * mEngine.DeltaTime();

    if (tu >= 1.0f)
        tu -= 1.0f;

    if (tv >= 1.0f)
        tv -= 1.0f;

    waterMat.MatTransform[0][3] = tu;
    waterMat.MatTransform[1][3] = tv;
    mEngine.GetScene()->UpdateMaterialCBData(waterMat.Name, sizeof(MaterialConstants2), &waterMat);
}

void CSBlurApp::AddShaders()
{
    AddShaderMain();
    AddShaderTree();
    AddShaderDepthComplexity();
}

void CSBlurApp::AddShaderMain()
{
    using namespace FireFlame;

    auto& shaderDesc = mShaderDescs["main"];
    shaderDesc.name = "main";
    shaderDesc.objCBSize = sizeof(ObjectConsts);
    shaderDesc.passCBSize = sizeof(PassConstants);
    shaderDesc.materialCBSize = sizeof(MaterialConstants2);
    shaderDesc.ParamDefault();
    shaderDesc.AddVertexInput("POSITION", FireFlame::VERTEX_FORMAT_FLOAT3);
    shaderDesc.AddVertexInput("NORMAL", FireFlame::VERTEX_FORMAT_FLOAT3);
    shaderDesc.AddVertexInput("TEXCOORD", FireFlame::VERTEX_FORMAT_FLOAT2);
    auto vs = shaderDesc.AddShaderStage(L"Shaders\\Main.hlsl", Shader_Type::VS, "VS", "vs_5_0");

    // ps with macros
    auto& ps = shaderDesc.AddShaderStage(L"Shaders\\Main.hlsl", Shader_Type::PS, "PS", "ps_5_0");
    mShaderMacrosPS[""] = ps.Macros2String();

    std::vector<std::pair<std::string, std::string>> macros = { { "FOG", "1" } };
    auto& psFogged = shaderDesc.AddShaderStage
    (
        L"Shaders\\Main.hlsl",
        Shader_Type::PS, "PS", "ps_5_0",
        macros
    );
    mShaderMacrosPS["fogged"] = psFogged.Macros2String();

    macros.emplace_back("ALPHA_CLIP", "1");
    auto& psAlphaClip = shaderDesc.AddShaderStage
    (
        L"Shaders\\Main.hlsl",
        Shader_Type::PS, "PS", "ps_5_0",
        macros
    );
    mShaderMacrosPS["fogged_and_alpha_clip"] = psAlphaClip.Macros2String();
    // end
    
    mEngine.GetScene()->AddShader(shaderDesc);
}

void CSBlurApp::AddShaderTree()
{
    using namespace FireFlame;

    auto& shaderDesc = mShaderDescs["tree"];
    shaderDesc.name = "tree";
    shaderDesc.objCBSize = sizeof(ObjectConsts);
    shaderDesc.passCBSize = sizeof(PassConstants);
    shaderDesc.materialCBSize = sizeof(MaterialConstants2);
    shaderDesc.ParamDefault();
    shaderDesc.AddVertexInput("POSITION", FireFlame::VERTEX_FORMAT_FLOAT3);
    shaderDesc.AddVertexInput("SIZE", FireFlame::VERTEX_FORMAT_FLOAT2);
    
    shaderDesc.AddShaderStage(L"Shaders\\TreeSprite.hlsl", Shader_Type::VS, "VS", "vs_5_0");
    shaderDesc.AddShaderStage(L"Shaders\\TreeSprite.hlsl", Shader_Type::GS, "GS", "gs_5_0");

    // ps with macros
    auto& ps = shaderDesc.AddShaderStage(L"Shaders\\TreeSprite.hlsl", Shader_Type::PS, "PS", "ps_5_0");
    mTreeShaderMacrosPS[""] = ps.Macros2String();

    std::vector<std::pair<std::string, std::string>> macros = { { "FOG", "1" } };
    auto& psFogged = shaderDesc.AddShaderStage
    (
        L"Shaders\\TreeSprite.hlsl",
        Shader_Type::PS, "PS", "ps_5_0",
        macros
    );
    mTreeShaderMacrosPS["fogged"] = psFogged.Macros2String();

    macros.emplace_back("ALPHA_CLIP", "1");
    auto& psAlphaClip = shaderDesc.AddShaderStage
    (
        L"Shaders\\TreeSprite.hlsl",
        Shader_Type::PS, "PS", "ps_5_0",
        macros
    );
    mTreeShaderMacrosPS["fogged_and_alpha_clip"] = psAlphaClip.Macros2String();
    // end

    mEngine.GetScene()->AddShader(shaderDesc);
}

void CSBlurApp::AddShaderDepthComplexity()
{
    using namespace FireFlame;

    auto& shaderDC = mShaderDescs["DepthComplexity"];
    shaderDC.name = "DepthComplexity";
    shaderDC.objCBSize = sizeof(DepthComplexityObjConsts);
    shaderDC.AddVertexInput("POSITION", FireFlame::VERTEX_FORMAT_FLOAT3);
    shaderDC.AddShaderStage(L"Shaders\\DC.hlsl", Shader_Type::VS, "VS", "vs_5_0");
    shaderDC.AddShaderStage(L"Shaders\\DC.hlsl", Shader_Type::PS, "PS", "ps_5_0");
   
    mEngine.GetScene()->AddShader(shaderDC);
}

void CSBlurApp::AddPSOs()
{
    using namespace FireFlame;

    PSODesc descTree(mShaderDescs["tree"].name,"",mTreeShaderMacrosPS["fogged_and_alpha_clip"]);
    descTree.opaque = true;
    descTree.topology = Primitive_Topology::PointList;
    descTree.alpha2Coverage = true;
    descTree.cullMode = Cull_Mode::None;
    // for depth complexity
    descTree.stencilEnable = true;
    descTree.stencilFailOp = STENCIL_OP::INCR;
    descTree.stencilDepthFailOp = STENCIL_OP::INCR;
    descTree.stencilPassOp = STENCIL_OP::INCR;
    descTree.stencilFunc = COMPARISON_FUNC::ALWAYS;
    // end
    mEngine.GetScene()->AddPSO("tree", descTree);

    PSODesc descDepthComplexity(mShaderDescs["DepthComplexity"].name);
    descDepthComplexity.stencilEnable = true;
    descDepthComplexity.stencilFunc = COMPARISON_FUNC::EQUAL;
    mEngine.GetScene()->AddPSO("depth_complexity_default", descDepthComplexity);

    PSODesc desc(mShaderDescs["main"].name);
    desc.cullMode = Cull_Mode::None;
    desc.shaderMacroPS = mShaderMacrosPS["fogged_and_alpha_clip"];
    // for depth complexity
    desc.stencilEnable = true;
    desc.stencilFailOp = STENCIL_OP::INCR;
    desc.stencilDepthFailOp = STENCIL_OP::INCR;
    desc.stencilPassOp = STENCIL_OP::INCR;
    desc.stencilFunc = COMPARISON_FUNC::ALWAYS;
    // end
    mEngine.GetScene()->AddPSO("cull_none_ps_fogged_clipped", desc);

    desc.default();
    // for depth complexity
    desc.stencilEnable = true;
    desc.stencilFailOp = STENCIL_OP::INCR;
    desc.stencilDepthFailOp = STENCIL_OP::INCR;
    desc.stencilPassOp = STENCIL_OP::INCR;
    desc.stencilFunc = COMPARISON_FUNC::ALWAYS;
    // end
    desc.shaderMacroPS = mShaderMacrosPS["fogged"];
    mEngine.GetScene()->AddPSO("ps_fogged", desc);
    desc.opaque = false;
    mEngine.GetScene()->AddPSO("transparent_ps_fogged", desc);
}

void CSBlurApp::AddTextures()
{
    mEngine.GetScene()->AddTexture
    (
        "wirefenceTex",
        L"..\\..\\Resources\\Textures\\WireFence.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "waterTex",
        L"..\\..\\Resources\\Textures\\water1.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "grassTex",
        L"..\\..\\Resources\\Textures\\grass.dds"
    );

    mEngine.GetScene()->AddTexture
    (
        "treeArrayTex",
        L"..\\..\\Resources\\Textures\\treearray.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "treeArrayTex2",
        L"..\\..\\Resources\\Textures\\treearray2.dds"
    );
}

void CSBlurApp::AddMaterials()
{
    auto& grass = mMaterials["grass"];
    grass.Name = "grass";
    grass.DiffuseAlbedo = FireFlame::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    grass.FresnelR0 = FireFlame::Vector3f(0.01f, 0.01f, 0.01f);
    grass.Roughness = 0.125f;
    mEngine.GetScene()->AddMaterial
    (
        grass.Name,
        mShaderDescs["main"].name, "grassTex",
        sizeof(MaterialConstants2), &grass
    );

    auto& wirefence = mMaterials["wirefence"];
    wirefence.Name = "wirefence";
    wirefence.DiffuseAlbedo = FireFlame::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    wirefence.FresnelR0 = FireFlame::Vector3f(0.1f, 0.1f, 0.1f);
    wirefence.Roughness = 0.25f;
    mEngine.GetScene()->AddMaterial
    (
        wirefence.Name,
        mShaderDescs["main"].name, "wirefenceTex",
        sizeof(MaterialConstants2), &wirefence
    );

    auto& water = mMaterials["water"];
    water.Name = "water";
    water.DiffuseAlbedo = FireFlame::Vector4f(1.0f, 1.0f, 1.0f, 0.5f);
    water.FresnelR0 = FireFlame::Vector3f(0.1f, 0.1f, 0.1f);
    water.Roughness = 0.0f;
    mEngine.GetScene()->AddMaterial
    (
        water.Name,
        mShaderDescs["main"].name, "waterTex",
        sizeof(MaterialConstants2), &water
    );

    auto& treeSprites = mMaterials["treeSprites"];
    treeSprites.Name = "treeSprites";
    treeSprites.DiffuseAlbedo = FireFlame::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    treeSprites.FresnelR0 = FireFlame::Vector3f(0.01f, 0.01f, 0.01f);
    treeSprites.Roughness = 0.125f;
    mEngine.GetScene()->AddMaterial
    (
    {
        treeSprites.Name,
        mShaderDescs["tree"].name, 
        {
            {"treeArrayTex2", FireFlame::SRV_DIMENSION::TEXTURE2DARRAY}
        },
        sizeof(MaterialConstants), &treeSprites
    }  
    );
}

void CSBlurApp::AddMeshs()
{
    AddMeshBox();
    AddMeshWaves();
    AddMeshLand();
    AddMeshTrees();
    AddMeshFullScreenRect();
}

void CSBlurApp::AddMeshWaves()
{
    using namespace FireFlame;

    mWaves = std::make_unique<Waves>(128, 128, 1.0f, 0.03f, 4.0f, 0.2f);
    std::vector<std::uint16_t> indices(3 * mWaves->TriangleCount()); // 3 indices per face
    assert(mWaves->VertexCount() < 0x0000ffff);

    // Iterate over each quad.
    int m = mWaves->RowCount();
    int n = mWaves->ColumnCount();
    int k = 0;
    for (int i = 0; i < m - 1; ++i)
    {
        for (int j = 0; j < n - 1; ++j)
        {
            indices[k] = i*n + j;
            indices[k + 1] = i*n + j + 1;
            indices[k + 2] = (i + 1)*n + j;

            indices[k + 3] = (i + 1)*n + j;
            indices[k + 4] = i*n + j + 1;
            indices[k + 5] = (i + 1)*n + j + 1;

            k += 6; // next quad
        }
    }

    auto& meshDesc = mMeshDescs["waves"];
    meshDesc.name = "waves";
    meshDesc.indexCount = (unsigned int)indices.size();
    meshDesc.indexFormat = Index_Format::UINT16;
    meshDesc.indices = indices.data();

    meshDesc.vertexInFrameRes = true;
    meshDesc.vertexDataCount.push_back((unsigned int)mWaves->VertexCount());
    meshDesc.vertexDataSize.push_back(sizeof(FLVertexNormalTex));

    // sub meshes
    meshDesc.subMeshs.emplace_back("All", (UINT)indices.size());
    mEngine.GetScene()->AddPrimitive(meshDesc);
}

void CSBlurApp::AddMeshBox()
{
    using namespace FireFlame;

    GeometryGenerator geoGen;
    GeometryGenerator::MeshData box = geoGen.CreateBox(8.0f, 8.0f, 8.0f, 3);

    std::vector<FireFlame::FLVertexNormalTex> vertices(box.Vertices.size());
    for (size_t i = 0; i < box.Vertices.size(); ++i)
    {
        vertices[i].Pos = box.Vertices[i].Position;
        vertices[i].Normal = box.Vertices[i].Normal;
        vertices[i].Tex = box.Vertices[i].TexC;
    }

    std::vector<std::uint32_t> indices = box.Indices32;

    auto& meshDesc = mMeshDescs["box"];
    meshDesc.name = "box";
    meshDesc.primitiveTopology = Primitive_Topology::TriangleList;
    meshDesc.indexCount = (unsigned int)indices.size();
    meshDesc.indexFormat = Index_Format::UINT32;
    meshDesc.indices = indices.data();

    meshDesc.vertexDataCount.push_back((unsigned int)vertices.size());
    meshDesc.vertexDataSize.push_back(sizeof(FLVertexNormalTex));
    meshDesc.vertexData.push_back(vertices.data());

    // sub meshes
    meshDesc.subMeshs.emplace_back("all", (UINT)box.Indices32.size());
    mEngine.GetScene()->AddPrimitive(meshDesc);
}

void CSBlurApp::AddMeshLand()
{
    using namespace FireFlame;

    GeometryGenerator geoGen;
    GeometryGenerator::MeshData grid = geoGen.CreateGrid(160.0f, 160.0f, 50, 50);

    std::vector<FLVertexNormalTex> vertices(grid.Vertices.size());
    for (size_t i = 0; i < grid.Vertices.size(); ++i)
    {
        auto& p = grid.Vertices[i].Position;
        vertices[i].Pos = p;
        vertices[i].Pos.y = GetHillsHeight(p.x, p.z);
        vertices[i].Normal = GetHillsNormal(p.x, p.z);
        vertices[i].Tex = grid.Vertices[i].TexC;
    }

    std::vector<std::uint16_t> indices = grid.GetIndices16();

    auto& meshDesc = mMeshDescs["land"];
    meshDesc.name = "land";
    meshDesc.indexCount = (unsigned int)indices.size();
    meshDesc.indexFormat = Index_Format::UINT16;
    meshDesc.indices = indices.data();

    meshDesc.vertexDataCount.push_back((unsigned int)vertices.size());
    meshDesc.vertexDataSize.push_back(sizeof(FLVertexNormalTex));
    meshDesc.vertexData.push_back(vertices.data());

    // sub meshes
    meshDesc.subMeshs.emplace_back("all", (UINT)indices.size());
    mEngine.GetScene()->AddPrimitive(meshDesc);
}

void CSBlurApp::AddMeshTrees()
{
    using namespace FireFlame;

    static const int treeCount = 16;
    std::array<FLVertexSize, 16> vertices;
    for (UINT i = 0; i < treeCount; ++i)
    {
        float x = MathHelper::RandF(-45.0f, 45.0f);
        float z = MathHelper::RandF(-45.0f, 45.0f);
        float y = GetHillsHeight(x, z);
        while (y < 2.0f)
        {
            x = MathHelper::RandF(-45.0f, 45.0f);
            z = MathHelper::RandF(-45.0f, 45.0f);
            y = GetHillsHeight(x, z);
        }

        // Move tree slightly above land height.
        y += 8.0f;

        vertices[i].Pos = { x, y, z };
        vertices[i].Size = { 20.0f, 20.0f };
    }

    std::array<std::uint16_t, 16> indices =
    {
        0, 1, 2, 3, 4, 5, 6, 7,
        8, 9, 10, 11, 12, 13, 14, 15
    };

    auto& meshDesc = mMeshDescs["trees"];
    meshDesc.name = "trees";
    meshDesc.indexCount = (unsigned int)indices.size();
    meshDesc.indexFormat = Index_Format::UINT16;
    meshDesc.indices = indices.data();

    meshDesc.vertexDataCount.push_back((unsigned int)vertices.size());
    meshDesc.vertexDataSize.push_back(sizeof(FLVertexSize));
    meshDesc.vertexData.push_back(vertices.data());

    // sub meshes
    meshDesc.subMeshs.emplace_back("all", (UINT)indices.size());
    mEngine.GetScene()->AddPrimitive(meshDesc);
}

void CSBlurApp::AddMeshFullScreenRect()
{
    using namespace FireFlame;

    std::array<FLVertex, 4> vertices = {
        // front face 
        FLVertex(-1.0f, -1.0f,  0.0f),
        FLVertex(-1.0f, +1.0f,  0.0f),
        FLVertex(+1.0f, +1.0f,  0.0f),
        FLVertex(+1.0f, -1.0f,  0.0f)
    };
    std::array<std::uint16_t, 6> indices = {
        // front face
        0, 1, 2,
        0, 2, 3
    };

    auto& meshDesc = mMeshDescs["full_screen_rect"];
    meshDesc.name = "full_screen_rect";
    meshDesc.indexCount = (unsigned int)indices.size();
    meshDesc.indexFormat = Index_Format::UINT16;
    meshDesc.indices = indices.data();

    meshDesc.vertexDataCount.push_back((unsigned int)vertices.size());
    meshDesc.vertexDataSize.push_back(sizeof(FLVertex));
    meshDesc.vertexData.push_back(vertices.data());

    // sub meshes
    meshDesc.subMeshs.emplace_back("all", (UINT)indices.size());
    mEngine.GetScene()->AddPrimitive(meshDesc);
}

float CSBlurApp::GetHillsHeight(float x, float z) const
{
    return 0.3f*(z*sinf(0.1f*x) + x*cosf(0.1f*z));
}

FireFlame::Vector3f CSBlurApp::GetHillsNormal(float x, float z) const
{
    // n = (-df/dx, 1, -df/dz)
    FireFlame::Vector3f n
    (
        -0.03f*z*cosf(0.1f*x) - 0.3f*cosf(0.1f*z),
        1.0f,
        -0.3f*sinf(0.1f*x) + 0.03f*x*sinf(0.1f*z)
    );
    n.Normalize();

    return n;
}

void CSBlurApp::AddRenderItems()
{
    AddRenderItemsMain();
    AddRenderItemsTree();
    AddRenderItemsDepthComplexity();
}

void CSBlurApp::AddRenderItemsMain()
{
    using namespace DirectX;

    FireFlame::stRenderItemDesc RItem("box1", mMeshDescs["box"].subMeshs[0]);
    RItem.mat = "wirefence";
    XMFLOAT4X4 trans[2];
    XMStoreFloat4x4
    (
        &trans[0],
        XMMatrixTranspose(XMMatrixTranslation(3.0f, 2.0f, -9.0f))
    );
    XMStoreFloat4x4
    (
        &trans[1],
        XMMatrixTranspose(XMMatrixScaling(1.0f, 1.0f, 1.0f))
    );
    RItem.dataLen = sizeof(XMFLOAT4X4)*_countof(trans);
    RItem.data = &trans[0];
    RItem.cullMode = FireFlame::Cull_Mode::None;
    mRenderItems[RItem.name] = RItem;
    mEngine.GetScene()->AddRenderItem
    (
        mMeshDescs["box"].name,
        mShaderDescs["main"].name,
        "cull_none_ps_fogged_clipped",
        0,
        RItem
    );

    FireFlame::stRenderItemDesc RItem2("Waves", mMeshDescs["waves"].subMeshs[0]);
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
    RItem2.dataLen = sizeof(XMFLOAT4X4)*_countof(trans);
    RItem2.data = &trans[0];
    RItem2.mat = "water";
    RItem2.opaque = false;
    mRenderItems[RItem.name] = RItem;
    mEngine.GetScene()->AddRenderItem
    (
        mMeshDescs["waves"].name,
        mShaderDescs["main"].name,
        "transparent_ps_fogged",
        0,
        RItem2
    );

    FireFlame::stRenderItemDesc RItem3("grid", mMeshDescs["land"].subMeshs[0]);
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
    mRenderItems[RItem.name] = RItem;
    mEngine.GetScene()->AddRenderItem
    (
        mMeshDescs["land"].name,
        mShaderDescs["main"].name,
        "ps_fogged",
        0,
        RItem3
    );
}

void CSBlurApp::AddRenderItemsTree()
{
    using namespace DirectX;

    FireFlame::stRenderItemDesc RItem("trees", mMeshDescs["trees"].subMeshs[0]);
    RItem.mat = "treeSprites";
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
    RItem.opaque = true;
    RItem.topology = FireFlame::Primitive_Topology::PointList;
    mRenderItems[RItem.name] = RItem;
    mEngine.GetScene()->AddRenderItem
    (
        mMeshDescs["trees"].name,
        mShaderDescs["tree"].name,
        "tree",
        0,
        RItem
    );
}

void CSBlurApp::AddRenderItemsDepthComplexity()
{
    std::string itmename = "DepthComplexity";
    FireFlame::stRenderItemDesc RItem(itmename, mMeshDescs["full_screen_rect"].subMeshs[0]);
    DepthComplexityObjConsts consts;
    RItem.dataLen = sizeof(DepthComplexityObjConsts);
    RItem.data = &consts;

    consts.color = { 1.f,1.f,1.f };
    static const int MAX_DEPTH = 12;
    static const float COLOR_STEP = -1.0f / MAX_DEPTH;
    for (int depth = 0; depth < MAX_DEPTH; depth++)
    {
        RItem.name = itmename + "Depth:" + std::to_string(depth);
        consts.color += { COLOR_STEP, COLOR_STEP, COLOR_STEP };
        RItem.stencilRef = depth;
        mRenderItems[RItem.name] = RItem;
        mEngine.GetScene()->AddRenderItem
        (
            mMeshDescs["full_screen_rect"].name,
            mShaderDescs["DepthComplexity"].name,
            "depth_complexity_default",
            1,
            RItem
        );
    }

    mEngine.GetScene()->PrimitiveVisible(mMeshDescs["full_screen_rect"].name, mShowDepthComplexity);
}

void CSBlurApp::OnKeyUp(WPARAM wParam, LPARAM lParam)
{
    FLEngineApp3::OnKeyUp(wParam, lParam);
    if ((int)wParam == 'R')
    {
        mEngine.Resume();
    }else if ((int)wParam == 'S')
    {
        mEngine.Stop();
    }else if ((int)wParam == 'D')
    {
        mShowDepthComplexity = !mShowDepthComplexity;
        mEngine.GetScene()->PrimitiveVisible(mMeshDescs["full_screen_rect"].name, mShowDepthComplexity);
    }
}