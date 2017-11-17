#include "LitWavesApp.h"

LitWavesApp::LitWavesApp(FireFlame::Engine& e) : FLEngineApp(e, 15.f, 1500.f)
{
    mTheta = 1.5f*FireFlame::MathHelper::FL_PI;
    mPhi = FireFlame::MathHelper::FL_PIDIV2 - 0.1f;
    mRadius = 100.0f;

    mPixelStep = 0.05f;
}

void LitWavesApp::Initialize()
{
    BuildShaders();
    AddMaterials();
    BuildLandGeometry();
    BuildWavesGeometry();

    BuildRenderItems();

    mPasses.push_back("DefaultPass");
    mEngine.GetScene()->AddPass(mShaderDesc.name, mPasses[0]);
}

void LitWavesApp::Update(float time_elapsed)
{
    using namespace FireFlame;

    FLEngineApp::Update(time_elapsed);

    if (mWaterMatDirty)
        mEngine.GetScene()->UpdateMaterialCBData(mWaterMat.name, sizeof(MaterialConstants), &mWaterMat);
    if (mGrassMatDirty)
        mEngine.GetScene()->UpdateMaterialCBData(mGrassMat.name, sizeof(MaterialConstants), &mGrassMat);
    
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
    std::vector<FLVertexNormal> vertices(mWaves->VertexCount());
    for (int i = 0; i < mWaves->VertexCount(); ++i)
    {
        vertices[i].Pos = { mWaves->Position(i).x,mWaves->Position(i).y,mWaves->Position(i).z };
        vertices[i].Normal = { mWaves->Normal(i).x,mWaves->Normal(i).y, mWaves->Normal(i).z };
    }
    mEngine.GetScene()->UpdateMeshCurrVBFrameRes
    (
        mMeshDesc[1].name, 0,
        sizeof(FLVertexNormal)*vertices.size(),
        vertices.data()
    );
}

void LitWavesApp::UpdateMainPassCB(float time_elapsed)
{
    //float red = (std::sinf(mEngine.TotalTime()) + 1.f) / 2.f;
    //red = FireFlame::MathHelper::Clamp(red, 0.3f, 1.0f);
    //mMainPassCB.Lights[0].Strength = { red, 0.2f, 0.2f };
}

void LitWavesApp::BuildWavesGeometry()
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

    UINT vbByteSize = mWaves->VertexCount() * sizeof(FireFlame::FLVertexNormal);
    UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

    mMeshDesc.emplace_back();
    mMeshDesc[1].name = "Waves";
    mMeshDesc[1].indexCount = (unsigned int)indices.size();
    mMeshDesc[1].indexFormat = Index_Format::UINT16;
    mMeshDesc[1].indices = indices.data();

    mMeshDesc[1].vertexInFrameRes = true;
    mMeshDesc[1].vertexDataCount.push_back((unsigned int)mWaves->VertexCount());
    mMeshDesc[1].vertexDataSize.push_back(sizeof(FLVertexNormal));
    //mMeshDesc[1].vertexData.push_back(vertices.data());

    // sub meshes
    mMeshDesc[1].subMeshs.emplace_back("All", (UINT)indices.size());
    mEngine.GetScene()->AddPrimitive(mMeshDesc[1]);
}

void LitWavesApp::BuildRenderItems()
{
    using namespace DirectX;

    FireFlame::stRenderItemDesc RItem("Grid", mMeshDesc[0].subMeshs[0]);
    XMFLOAT4X4 worldTrans = FireFlame::Matrix4X4();
    RItem.dataLen = sizeof(XMFLOAT4X4);
    RItem.data = &worldTrans;
    RItem.mat = "grass";
    mRenderItems.emplace_back(RItem);
    mEngine.GetScene()->AddRenderItem(mMeshDesc[0].name, mShaderDesc.name, RItem);

    FireFlame::stRenderItemDesc RItem2("Waves", mMeshDesc[1].subMeshs[0]);
    //XMFLOAT4X4 worldTrans = FireFlame::Matrix4X4();
    RItem2.dataLen = sizeof(XMFLOAT4X4);
    RItem2.data = &worldTrans;
    RItem2.mat = "water";
    mRenderItems.emplace_back(RItem2);
    mEngine.GetScene()->AddRenderItem(mMeshDesc[1].name, mShaderDesc.name, RItem2);
}

void LitWavesApp::BuildShaders()
{
    using namespace FireFlame;
    mShaderDesc.name = "LitWaves";
    mShaderDesc.objCBSize = sizeof(ObjectConsts);
    mShaderDesc.passCBSize = sizeof(PassConstants);
    mShaderDesc.materialCBSize = sizeof(MaterialConstants);
    mShaderDesc.AddVertexInput("POSITION", FireFlame::VERTEX_FORMAT_FLOAT3);
    mShaderDesc.AddVertexInput("NORMAL",   FireFlame::VERTEX_FORMAT_FLOAT3);
    mShaderDesc.AddShaderStage(L"Shaders\\LitWaves.hlsl", Shader_Type::VS, "VS", "vs_5_0");
    mShaderDesc.AddShaderStage(L"Shaders\\LitWaves.hlsl", Shader_Type::PS, "PS", "ps_5_0");
    mShaderDesc.materialRegister = 1;
    mShaderDesc.passRegister = 2;

    mEngine.GetScene()->AddShader(mShaderDesc);
}

void LitWavesApp::AddMaterials()
{
    mGrassMat.name = "grass";
    mGrassMat.DiffuseAlbedo = { 0.2f, 0.6f, 0.2f, 1.0f };
    //mGrassMat.DiffuseAlbedo = { 0.8f, 0.2f, 0.2f, 1.0f };
    mGrassMat.FresnelR0 = { 0.01f, 0.01f, 0.01f };
    mGrassMat.Roughness = 0.125f;
    mEngine.GetScene()->AddMaterial("grass", mShaderDesc.name, sizeof(MaterialConstants), &mGrassMat);

    // This is not a good water material definition, but we do not have all the rendering
    // tools we need (transparency, environment reflection), so we fake it for now.
    mWaterMat.name = "water";
    mWaterMat.DiffuseAlbedo = { 0.0f, 0.2f, 0.6f, 1.0f };
    //mWaterMat.DiffuseAlbedo = { 0.6f, 0.6f, 0.6f, 1.0f };
    mWaterMat.FresnelR0 = { 0.1f, 0.1f, 0.1f };
    mWaterMat.Roughness = 0.0f;
    mEngine.GetScene()->AddMaterial(mWaterMat.name, mShaderDesc.name, sizeof(MaterialConstants), &mWaterMat);
}

void LitWavesApp::BuildLandGeometry()
{
    using namespace FireFlame;
    GeometryGenerator geoGen;
    GeometryGenerator::MeshData grid = geoGen.CreateGrid(160.0f, 160.0f, 50, 50);

    std::vector<FLVertexNormal> vertices(grid.Vertices.size());
    for (size_t i = 0; i < grid.Vertices.size(); ++i)
    {
        auto& p = grid.Vertices[i].Position;
        vertices[i].Pos = p;
        vertices[i].Pos.y = GetHillsHeight(p.x, p.z);
        vertices[i].Normal = GetHillsNormal(p.x, p.z);
    }

    std::vector<std::uint16_t> indices = grid.GetIndices16();

    mMeshDesc.emplace_back();
    mMeshDesc[0].name = "Land";
    mMeshDesc[0].indexCount = (unsigned int)indices.size();
    mMeshDesc[0].indexFormat = Index_Format::UINT16;
    mMeshDesc[0].indices = indices.data();

    mMeshDesc[0].vertexDataCount.push_back((unsigned int)vertices.size());
    mMeshDesc[0].vertexDataSize.push_back(sizeof(FLVertexNormal));
    mMeshDesc[0].vertexData.push_back(vertices.data());

    // sub meshes
    mMeshDesc[0].subMeshs.emplace_back("All", (UINT)indices.size());
    mEngine.GetScene()->AddPrimitive(mMeshDesc[0]);
}

void LitWavesApp::OnKeyboardInput(float time_elapsed)
{
    FLEngineApp::OnKeyboardInput(time_elapsed);

    std::string info;
    if (GetAsyncKeyState('R') & 0x8000)
    {
        mWaterMatDirty = true;
        mGrassMatDirty = true;
        mWaterMat.Roughness += 0.1f;
        mGrassMat.Roughness += 0.1f;
        info = "=====  water roughness:" + std::to_string(mWaterMat.Roughness) +
               " grass roughness:" + std::to_string(mGrassMat.Roughness) +
               "=====\n";
    }
    if (GetAsyncKeyState('T') & 0x8000)
    {
        mWaterMatDirty = true;
        mGrassMatDirty = true;
        mWaterMat.Roughness -= 0.1f;
        mGrassMat.Roughness -= 0.1f;
        if (mWaterMat.Roughness < 0.f)
            mWaterMat.Roughness = 0.f;
        if (mGrassMat.Roughness < 0.f)
            mGrassMat.Roughness = 0.f;
        info = "=====  water roughness:" + std::to_string(mWaterMat.Roughness) +
               " grass roughness:" + std::to_string(mGrassMat.Roughness) +
               "=====\n";
    }
    if (!info.empty())
        OutputDebugStringA(info.c_str());
}

float LitWavesApp::GetHillsHeight(float x, float z) const
{
    return 0.3f*(z*sinf(0.1f*x) + x*cosf(0.1f*z));
}

FireFlame::Vector3f LitWavesApp::GetHillsNormal(float x, float z) const
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