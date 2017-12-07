#include "LandAndWavesApp.h"

LandAndWavesApp::LandAndWavesApp(FireFlame::Engine& e) :FLEngineApp(e, 15.0f, 1500.f)
{
    mTheta = 1.5f*FireFlame::MathHelper::FL_PI;
    mPhi = FireFlame::MathHelper::FL_PIDIV2 - 0.1f;
    mRadius = 100.0f;

    mPixelStep = 0.05f;
}

void LandAndWavesApp::Initialize()
{
    BuildShaders();
    BuildLandGeometry();
    BuildWavesGeometry();

    BuildRenderItems();

    mPasses.push_back("DefaultPass");
    mEngine.GetScene()->AddPass(mPasses[0]);
}

void LandAndWavesApp::Update(float time_elapsed)
{
    FLEngineApp::Update(time_elapsed);

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
    std::vector<FLVertexColor> vertices(mWaves->VertexCount(), { 0.f,0.f,0.f, 0.000000000f, 0.000000000f, 1.000000000f, 1.000000000f });
    for (int i = 0; i < mWaves->VertexCount(); ++i)
    {
        vertices[i].Pos = { mWaves->Position(i).x,mWaves->Position(i).y,mWaves->Position(i).z };
        //FLVertex v;
        //v.Pos = { mWaves->Position(i).x,mWaves->Position(i).y,mWaves->Position(i).z };
        //v.Color = { 0.000000000f, 0.000000000f, 1.000000000f, 1.000000000f }
        //currWavesVB->CopyData(i, v);
        //mEngine.GetScene()->UpdateMeshCurrVBFrameRes(mMeshDesc[1].name, i, sizeof(FLVertex), &v);
    }
    mEngine.GetScene()->UpdateMeshCurrVBFrameRes
    (
        mMeshDesc[1].name, 0, 
        sizeof(FLVertexColor)*vertices.size(),
        vertices.data()
    );

    // Set the dynamic VB of the wave render item to the current frame VB.
    //mWavesRitem->Geo->VertexBufferGPU = currWavesVB->Resource();
}

void LandAndWavesApp::BuildWavesGeometry()
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

    UINT vbByteSize = mWaves->VertexCount() * sizeof(FireFlame::FLVertexColor);
    UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

    mMeshDesc.emplace_back();
    mMeshDesc[1].name = "Waves";
    mMeshDesc[1].indexCount = (unsigned int)indices.size();
    mMeshDesc[1].indexFormat = Index_Format::UINT16;
    mMeshDesc[1].indices = indices.data();

    mMeshDesc[1].vertexInFrameRes = true;
    mMeshDesc[1].vertexDataCount.push_back((unsigned int)mWaves->VertexCount());
    mMeshDesc[1].vertexDataSize.push_back(sizeof(FLVertexColor));
    //mMeshDesc[1].vertexData.push_back(vertices.data());

    // sub meshes
    mMeshDesc[1].subMeshs.emplace_back("All", (UINT)indices.size());
    mEngine.GetScene()->AddPrimitive(mMeshDesc[1]);
}

void LandAndWavesApp::BuildRenderItems()
{
    using namespace DirectX;

    FireFlame::stRenderItemDesc RItem("Grid", mMeshDesc[0].subMeshs[0]);
    XMFLOAT4X4 worldTrans = FireFlame::Matrix4X4();
    RItem.dataLen = sizeof(XMFLOAT4X4);
    RItem.data = &worldTrans;
    mRenderItems.emplace_back(RItem);
    mEngine.GetScene()->AddRenderItem(mMeshDesc[0].name,mShaderDesc.name,RItem);

    FireFlame::stRenderItemDesc RItem2("Waves", mMeshDesc[1].subMeshs[0]);
    //XMFLOAT4X4 worldTrans = FireFlame::Matrix4X4();
    RItem2.dataLen = sizeof(XMFLOAT4X4);
    RItem2.data = &worldTrans;
    mRenderItems.emplace_back(RItem2);
    mEngine.GetScene()->AddRenderItem(mMeshDesc[1].name, mShaderDesc.name, RItem2);
}

void LandAndWavesApp::BuildShaders()
{
    using namespace FireFlame;
    mShaderDesc.name = "LandAndWaves";
    mShaderDesc.objCBSize = sizeof(ObjectConsts);
    mShaderDesc.passCBSize = sizeof(PassConstants);
    mShaderDesc.AddVertexInput("POSITION", FireFlame::VERTEX_FORMAT_FLOAT3);
    mShaderDesc.AddVertexInput("COLOR", FireFlame::VERTEX_FORMAT_FLOAT4);
    //mShaderDesc.AddVertexInput("COLOR", FireFlame::VERTEX_FORMAT_A8R8G8B8_UNORM);
    mShaderDesc.AddShaderStage(L"Shaders\\LandAndWaves.hlsl", Shader_Type::VS, "VS", "vs_5_0");
    mShaderDesc.AddShaderStage(L"Shaders\\LandAndWaves.hlsl", Shader_Type::PS, "PS", "ps_5_0");

    mEngine.GetScene()->AddShader(mShaderDesc);
}

void LandAndWavesApp::BuildLandGeometry()
{
    using namespace FireFlame;
    GeometryGenerator geoGen;
    GeometryGenerator::MeshData grid = geoGen.CreateGrid(160.0f, 160.0f, 50, 50);

    //
    // Extract the vertex elements we are interested and apply the height function to
    // each vertex.  In addition, color the vertices based on their height so we have
    // sandy looking beaches, grassy low hills, and snow mountain peaks.
    //

    std::vector<FLVertexColor> vertices(grid.Vertices.size());
    for (size_t i = 0; i < grid.Vertices.size(); ++i)
    {
        auto& p = grid.Vertices[i].Position;
        vertices[i].Pos = p;
        vertices[i].Pos.y = GetHillsHeight(p.x, p.z);

        // Color the vertex based on its height.
        if (vertices[i].Pos.y < -10.0f)
        {
            // Sandy beach color.
            vertices[i].Color = { 1.0f, 0.96f, 0.62f, 1.0f };
        }
        else if (vertices[i].Pos.y < 5.0f)
        {
            // Light yellow-green.
            vertices[i].Color = { 0.48f, 0.77f, 0.46f, 1.0f };
        }
        else if (vertices[i].Pos.y < 12.0f)
        {
            // Dark yellow-green.
            vertices[i].Color = { 0.1f, 0.48f, 0.19f, 1.0f };
        }
        else if (vertices[i].Pos.y < 20.0f)
        {
            // Dark brown.
            vertices[i].Color = { 0.45f, 0.39f, 0.34f, 1.0f };
        }
        else
        {
            // White snow.
            vertices[i].Color = { 1.0f, 1.0f, 1.0f, 1.0f };
        }
    }

    std::vector<std::uint16_t> indices = grid.GetIndices16();

    mMeshDesc.emplace_back();
    mMeshDesc[0].name = "Land";
    mMeshDesc[0].indexCount = (unsigned int)indices.size();
    mMeshDesc[0].indexFormat = Index_Format::UINT16;
    mMeshDesc[0].indices = indices.data();

    mMeshDesc[0].vertexDataCount.push_back((unsigned int)vertices.size());
    mMeshDesc[0].vertexDataSize.push_back(sizeof(FLVertexColor));
    mMeshDesc[0].vertexData.push_back(vertices.data());

    // sub meshes
    mMeshDesc[0].subMeshs.emplace_back("All", (UINT)indices.size());
    mEngine.GetScene()->AddPrimitive(mMeshDesc[0]);
}

float LandAndWavesApp::GetHillsHeight(float x, float z) const
{
    return 0.3f*(z*sinf(0.1f*x) + x*cosf(0.1f*z));
}