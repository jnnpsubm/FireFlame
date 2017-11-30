#include "BlendApp.h"

void BlendApp::PreInitialize()
{
    mEngine.SetDefaultClearColor(mMainPassCB.FogColor);
}

void BlendApp::Initialize()
{
    AddShaders();

    AddTextures();
    AddMaterials();
    //AddGeoMeshs();
    AddBoxMesh();
    AddWavesMesh();
    AddLandMesh();
    AddRenderItems();

    mPasses.push_back("DefaultPass");
    mEngine.GetScene()->AddPass(mShaderDesc.name, mPasses[0]);
}

void BlendApp::UpdateMainPassCB(float time_elapsed)
{
    using namespace FireFlame;
    float totalTime = mEngine.TotalTime();

    mMainPassCB.AmbientLight = { 0.25f, 0.25f, 0.35f, 1.0f };
    mMainPassCB.Lights[0].Direction = { 0.57735f, -0.57735f, 0.57735f };
    mMainPassCB.Lights[0].Strength = { 0.8f, 0.8f, 0.8f };
    mMainPassCB.Lights[1].Direction = { -0.57735f, -0.57735f, 0.57735f };
    mMainPassCB.Lights[1].Strength = { 0.4f, 0.4f, 0.4f };
    mMainPassCB.Lights[2].Direction = { 0.0f, -0.707f, -0.707f };
    mMainPassCB.Lights[2].Strength = { 0.2f, 0.2f, 0.2f };

    if (mWaveStart)
        UpdateWaves();
}

void BlendApp::UpdateWaves()
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
        mMeshDesc[1].name, 0,
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
    mEngine.GetScene()->UpdateMaterialCBData(waterMat.name, sizeof(MaterialConstants2), &waterMat);
}

void BlendApp::AddShaders()
{
    using namespace FireFlame;
    mShaderDesc.name = "BlendApp";
    mShaderDesc.objCBSize = sizeof(ObjectConsts);
    mShaderDesc.passCBSize = sizeof(PassConstants);
    mShaderDesc.materialCBSize = sizeof(MaterialConstants2);
    mShaderDesc.ParamDefault();
    mShaderDesc.AddVertexInput("POSITION", FireFlame::VERTEX_FORMAT_FLOAT3);
    mShaderDesc.AddVertexInput("NORMAL", FireFlame::VERTEX_FORMAT_FLOAT3);
    mShaderDesc.AddVertexInput("TEXCOORD", FireFlame::VERTEX_FORMAT_FLOAT2);
    auto vs = mShaderDesc.AddShaderStage(L"Shaders\\BlendApp.hlsl", Shader_Type::VS, "VS", "vs_5_0");

    // ps with macros
    auto& ps = mShaderDesc.AddShaderStage(L"Shaders\\BlendApp.hlsl", Shader_Type::PS, "PS", "ps_5_0");
    mShaderMacrosPS[""] = ps.Macros2String();

    std::vector<std::pair<std::string, std::string>> macros = { { "FOG", "1" } };
    auto& psFogged = mShaderDesc.AddShaderStage
    (
        L"Shaders\\BlendApp.hlsl",
        Shader_Type::PS, "PS", "ps_5_0",
        macros
    );
    mShaderMacrosPS["fogged"] = psFogged.Macros2String();

    macros.emplace_back("ALPHA_CLIP", "1");
    auto& psAlphaClip = mShaderDesc.AddShaderStage
    (
        L"Shaders\\BlendApp.hlsl",
        Shader_Type::PS, "PS", "ps_5_0",
        macros
    );
    mShaderMacrosPS["fogged_and_alpha_clip"] = psAlphaClip.Macros2String();
    // end
    
    mEngine.GetScene()->AddShader(mShaderDesc);
}

void BlendApp::AddTextures()
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
}

void BlendApp::AddMaterials()
{
    auto& grass = mMaterials["grass"];
    grass.name = "grass";
    grass.DiffuseAlbedo = FireFlame::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    grass.FresnelR0 = FireFlame::Vector3f(0.01f, 0.01f, 0.01f);
    grass.Roughness = 0.125f;
    grass.UseTexture = 1;
    mEngine.GetScene()->AddMaterial
    (
        grass.name,
        mShaderDesc.name, "grassTex",
        sizeof(MaterialConstants2), &grass
    );

    auto& wirefence = mMaterials["wirefence"];
    wirefence.name = "wirefence";
    wirefence.DiffuseAlbedo = FireFlame::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    wirefence.FresnelR0 = FireFlame::Vector3f(0.1f, 0.1f, 0.1f);
    wirefence.Roughness = 0.25f;
    wirefence.UseTexture = 1;
    mEngine.GetScene()->AddMaterial
    (
        wirefence.name,
        mShaderDesc.name, "wirefenceTex",
        sizeof(MaterialConstants2), &wirefence
    );

    auto& water = mMaterials["water"];
    water.name = "water";
    water.DiffuseAlbedo = FireFlame::Vector4f(1.0f, 1.0f, 1.0f, 0.5f);
    water.FresnelR0 = FireFlame::Vector3f(0.1f, 0.1f, 0.1f);
    water.Roughness = 0.0f;
    water.UseTexture = 1;
    mEngine.GetScene()->AddMaterial
    (
        water.name,
        mShaderDesc.name, "waterTex",
        sizeof(MaterialConstants2), &water
    );
}

void BlendApp::AddGeoMeshs()
{
    using namespace FireFlame;
   
    GeometryGenerator geoGen;
    GeometryGenerator::MeshData box = geoGen.CreateBox(1.f, 1.f, 1.f, 3);
    GeometryGenerator::MeshData grid = geoGen.CreateGrid(200.0f, 300.0f, 60, 40);
    GeometryGenerator::MeshData sphere = geoGen.CreateGeosphere(0.5f, 4);
    GeometryGenerator::MeshData cylinder = geoGen.CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20);

    // Cache the vertex offsets to each object in the concatenated vertex buffer.
    UINT boxVertexOffset = 0;
    UINT gridVertexOffset = (UINT)box.Vertices.size();
    UINT sphereVertexOffset = gridVertexOffset + (UINT)grid.Vertices.size();
    UINT cylinderVertexOffset = sphereVertexOffset + (UINT)sphere.Vertices.size();
    // Cache the starting index for each object in the concatenated index buffer.
    UINT boxIndexOffset = 0;
    UINT gridIndexOffset = (UINT)box.Indices32.size();
    UINT sphereIndexOffset = gridIndexOffset + (UINT)grid.Indices32.size();
    UINT cylinderIndexOffset = sphereIndexOffset + (UINT)sphere.Indices32.size();

    // put together vertices
    auto totalVertexCount =
        box.Vertices.size() +
        grid.Vertices.size() +
        sphere.Vertices.size() +
        cylinder.Vertices.size();
    std::vector<FireFlame::FLVertexNormalTex> vertices(totalVertexCount);
    UINT k = 0;
    for (size_t i = 0; i < box.Vertices.size(); ++i, ++k)
    {
        vertices[k].Pos = box.Vertices[i].Position;
        vertices[k].Normal = box.Vertices[i].Normal;
        vertices[k].Tex = box.Vertices[i].TexC;
    }
    for (size_t i = 0; i < grid.Vertices.size(); ++i, ++k)
    {
        vertices[k].Pos = grid.Vertices[i].Position;
        vertices[k].Normal = grid.Vertices[i].Normal;
        vertices[k].Tex = grid.Vertices[i].TexC;
    }
    for (size_t i = 0; i < sphere.Vertices.size(); ++i, ++k)
    {
        vertices[k].Pos = sphere.Vertices[i].Position;
        vertices[k].Normal = sphere.Vertices[i].Normal;
        vertices[k].Tex = sphere.Vertices[i].TexC;
    }
    for (size_t i = 0; i < cylinder.Vertices.size(); ++i, ++k)
    {
        vertices[k].Pos = cylinder.Vertices[i].Position;
        vertices[k].Normal = cylinder.Vertices[i].Normal;
        vertices[k].Tex = cylinder.Vertices[i].TexC;
    }

    std::vector<std::uint32_t> indices;
    indices.insert(indices.end(), std::begin(box.Indices32), std::end(box.Indices32));
    indices.insert(indices.end(), std::begin(grid.Indices32), std::end(grid.Indices32));
    indices.insert(indices.end(), std::begin(sphere.Indices32), std::end(sphere.Indices32));
    indices.insert(indices.end(), std::begin(cylinder.Indices32), std::end(cylinder.Indices32));

    mMeshDesc.emplace_back();
    mMeshDesc.back().name = "Shapes";
    mMeshDesc.back().primitiveTopology = Primitive_Topology::TriangleList;
    mMeshDesc.back().indexCount = (unsigned int)indices.size();
    mMeshDesc.back().indexFormat = Index_Format::UINT32;
    mMeshDesc.back().indices = indices.data();

    mMeshDesc.back().vertexDataCount.push_back((unsigned int)vertices.size());
    mMeshDesc.back().vertexDataSize.push_back(sizeof(FLVertexNormalTex));
    mMeshDesc.back().vertexData.push_back(vertices.data());

    // sub meshes
    mMeshDesc.back().subMeshs.emplace_back
    (
        "Box", (UINT)box.Indices32.size(), boxIndexOffset, boxVertexOffset
    );
    mMeshDesc[0].subMeshs.emplace_back
    (
        "Grid", (UINT)grid.Indices32.size(), gridIndexOffset, gridVertexOffset
    );
    mMeshDesc[0].subMeshs.emplace_back
    (
        "Sphere", (UINT)sphere.Indices32.size(), sphereIndexOffset, sphereVertexOffset
    );
    mMeshDesc[0].subMeshs.emplace_back
    (
        "Cylinder", (UINT)cylinder.Indices32.size(), cylinderIndexOffset, cylinderVertexOffset
    );
    mEngine.GetScene()->AddPrimitive(mMeshDesc.back());
}

void BlendApp::AddWavesMesh()
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

    mMeshDesc.emplace_back();
    mMeshDesc[1].name = "Waves";
    mMeshDesc[1].indexCount = (unsigned int)indices.size();
    mMeshDesc[1].indexFormat = Index_Format::UINT16;
    mMeshDesc[1].indices = indices.data();

    mMeshDesc[1].vertexInFrameRes = true;
    mMeshDesc[1].vertexDataCount.push_back((unsigned int)mWaves->VertexCount());
    mMeshDesc[1].vertexDataSize.push_back(sizeof(FLVertexNormalTex));

    // sub meshes
    mMeshDesc[1].subMeshs.emplace_back("All", (UINT)indices.size());
    mEngine.GetScene()->AddPrimitive(mMeshDesc[1]);
}

void BlendApp::AddBoxMesh()
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

    mMeshDesc.emplace_back();
    mMeshDesc.back().name = "Box";
    mMeshDesc.back().primitiveTopology = Primitive_Topology::TriangleList;
    mMeshDesc.back().indexCount = (unsigned int)indices.size();
    mMeshDesc.back().indexFormat = Index_Format::UINT32;
    mMeshDesc.back().indices = indices.data();

    mMeshDesc.back().vertexDataCount.push_back((unsigned int)vertices.size());
    mMeshDesc.back().vertexDataSize.push_back(sizeof(FLVertexNormalTex));
    mMeshDesc.back().vertexData.push_back(vertices.data());

    // sub meshes
    mMeshDesc.back().subMeshs.emplace_back("Box", (UINT)box.Indices32.size());
    mEngine.GetScene()->AddPrimitive(mMeshDesc.back());
}

void BlendApp::AddLandMesh()
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

    mMeshDesc.emplace_back();
    mMeshDesc[2].name = "Land";
    mMeshDesc[2].indexCount = (unsigned int)indices.size();
    mMeshDesc[2].indexFormat = Index_Format::UINT16;
    mMeshDesc[2].indices = indices.data();

    mMeshDesc[2].vertexDataCount.push_back((unsigned int)vertices.size());
    mMeshDesc[2].vertexDataSize.push_back(sizeof(FLVertexNormalTex));
    mMeshDesc[2].vertexData.push_back(vertices.data());

    // sub meshes
    mMeshDesc[2].subMeshs.emplace_back("All", (UINT)indices.size());
    mEngine.GetScene()->AddPrimitive(mMeshDesc[2]);
}

float BlendApp::GetHillsHeight(float x, float z) const
{
    return 0.3f*(z*sinf(0.1f*x) + x*cosf(0.1f*z));
}

FireFlame::Vector3f BlendApp::GetHillsNormal(float x, float z) const
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

void BlendApp::AddRenderItems()
{
    using namespace DirectX;

    FireFlame::stRenderItemDesc RItem("Box1", mMeshDesc[0].subMeshs[0]);
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
    mRenderItems.emplace_back(RItem);
    mEngine.GetScene()->AddRenderItem
    (
        mMeshDesc[0].name,
        mShaderDesc.name,
        "",
        mShaderMacrosPS["fogged_and_alpha_clip"],
        RItem
    );

    FireFlame::stRenderItemDesc RItem2("Waves", mMeshDesc[1].subMeshs[0]);
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
    mRenderItems.emplace_back(RItem2);
    mEngine.GetScene()->AddRenderItem
    (
        mMeshDesc[1].name, 
        mShaderDesc.name, 
        "",
        mShaderMacrosPS["fogged"],
        RItem2
    );

    FireFlame::stRenderItemDesc RItem3("grid", mMeshDesc[2].subMeshs[0]);
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
    mRenderItems.emplace_back(RItem3);
    mEngine.GetScene()->AddRenderItem
    (
        mMeshDesc[2].name, 
        mShaderDesc.name, 
        "",
        mShaderMacrosPS["fogged"],
        RItem3
    );
}

void BlendApp::OnKeyUp(WPARAM wParam, LPARAM lParam)
{
    FLEngineApp::OnKeyUp(wParam, lParam);
    if ((int)wParam == 'R')
    {
        mEngine.Resume();
    }else if ((int)wParam == 'S')
    {
        mEngine.Stop();
    }
}