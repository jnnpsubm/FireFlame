#include "ProceduralTerrainApp.h"
#include <fstream>

ProceduralTerrainApp::ProceduralTerrainApp(FireFlame::Engine& e) :FLEngineApp(e,1010.f,1500.f) 
{
    mRadius = 1200.f;
    mPixelStep *= 10.f;

    //mTheta = 0.f;
    //mPhi = 0.f;
}

void ProceduralTerrainApp::Initialize()
{
    BuildShaders();
    BuildNoiseData();
    AddTextures();
    AddMaterials();
    BuildGeometry();
    BuildRenderItems();

    mPasses.push_back("DefaultPass");
    mEngine.GetScene()->AddPass(mShaderDesc.name, mPasses[0]);
}

void ProceduralTerrainApp::BuildNoiseData()
{
    const float step = (float)length / pixel_width;
    mNoiseData = std::make_unique<float[]>(pixel_width*pixel_width);

    for (size_t i = 0; i < pixel_width; ++i)
    {
        for (size_t j = 0; j < pixel_width; ++j)
        {
            mNoiseData[j*pixel_width + i] = std::abs(FireFlame::Noise::Evaluate(i*step, j*step, 0.5f));
            //mNoiseData[j*pixel_width + i] = FireFlame::Noise::Evaluate(i*step, j*step, 0.5f);
        }
    }
}

void ProceduralTerrainApp::AddMaterials()
{
    auto& terrain = mMaterials["terrain"];
    terrain.name = "terrain";
    terrain.DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
    terrain.FresnelR0 = { 0.0f,0.0f,0.0f };
    terrain.Roughness = 0.0f;
    mEngine.GetScene()->AddMaterial
    (
    {
        "terrain",
        mShaderDesc.name, { "darkdirtTex", "lightdirtTex", "snowTex", "heightMap" },
        sizeof(MaterialConstants), &terrain
    }
    );
}

void ProceduralTerrainApp::AddTextures()
{
    mEngine.GetScene()->AddTexture
    (
        "darkdirtTex",
        L"..\\..\\Resources\\terrain\\darkdirt.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "lightdirtTex",
        L"..\\..\\Resources\\terrain\\lightdirt.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "snowTex",
        L"..\\..\\Resources\\terrain\\snow.dds"
    );
    mEngine.GetScene()->AddTexture2D
    (
        "heightMap", (const std::uint8_t*)mNoiseData.get(),
        FireFlame::VERTEX_FORMAT_FLOAT1,
        pixel_width, pixel_width
    );
}

void ProceduralTerrainApp::UpdateMainPassCB(float time_elapsed)
{
    using namespace FireFlame;
    float totalTime = mEngine.TotalTime();

    mMainPassCB.AmbientLight = { 0.01f,0.01f,0.01f,1.0f };
    // above
    mMainPassCB.Lights[0].Direction = -MathHelper::SphericalToCartesian
    (
        1.0f,
        0.f,
        0.f
    );
    mMainPassCB.Lights[0].Strength = { 0.8f, 0.8f, 0.8f };

    // right
    mMainPassCB.Lights[1].Direction = -MathHelper::SphericalToCartesian
    (
        1.0f,
        0.f,
        MathHelper::FL_PIDIV4
    );
    mMainPassCB.Lights[1].Strength = { 0.6f, 0.3f, 0.2f };

    // left
    mMainPassCB.Lights[2].Direction = -MathHelper::SphericalToCartesian
    (
        1.0f,
        MathHelper::FL_PI,
        MathHelper::FL_PIDIV4
    );
    mMainPassCB.Lights[2].Strength = { 0.6f, 0.3f, 0.2f };

    mMainPassCB.Lights[3].Direction = -MathHelper::SphericalToCartesian
    (
        1.0f,
        MathHelper::FL_PIDIV2,
        MathHelper::FL_PIDIV2
    );
    mMainPassCB.Lights[3].Strength = { 0.3f, 0.5f, 0.5f };

    Vector3f sum(0.f, 0.f, 0.f);
    for (size_t i = 0; i < 4; ++i)
    {
        sum += mMainPassCB.Lights[i].Strength;
    }
    for (size_t i = 0; i < 4; ++i)
    {
        mMainPassCB.Lights[i].Strength = mMainPassCB.Lights[i].Strength / sum;
    }
}

void ProceduralTerrainApp::BuildShaders()
{
    using namespace FireFlame;
    mShaderDesc.name = "ProcedualTerrain";
    mShaderDesc.objCBSize = sizeof(ObjectConsts);
    mShaderDesc.passCBSize = sizeof(PassConstants);
    mShaderDesc.materialCBSize = sizeof(MaterialConstants);
    mShaderDesc.ParamDefault();
    mShaderDesc.texSRVDescriptorTableSize = 4;
    mShaderDesc.AddVertexInput("POSITION", FireFlame::VERTEX_FORMAT_FLOAT3);
    mShaderDesc.AddVertexInput("NORMAL", FireFlame::VERTEX_FORMAT_FLOAT3);
    mShaderDesc.AddVertexInput("TEXCOORD", FireFlame::VERTEX_FORMAT_FLOAT2);
    mShaderDesc.AddShaderStage(L"Shaders\\ProceduralTerrain.hlsl", Shader_Type::VS, "VS", "vs_5_0");
    mShaderDesc.AddShaderStage(L"Shaders\\ProceduralTerrain.hlsl", Shader_Type::PS, "PS", "ps_5_0");

    mEngine.GetScene()->AddShader(mShaderDesc);
}

void ProceduralTerrainApp::BuildGeometry()
{
    using namespace FireFlame;

    GeometryGenerator geoGen;
    GeometryGenerator::MeshData earth = geoGen.CreateGeosphere(10.f, 10);

    std::vector<FireFlame::FLVertexNormalTex> vertices(earth.Vertices.size());
    for (size_t i = 0; i < earth.Vertices.size(); ++i)
    {
        vertices[i].Pos = earth.Vertices[i].Position;
        //vertices[i].Pos.y += Noise::FBm(vertices[i].Pos, 1.9f, 1)*1.f;
        //vertices[i].Pos.y += std::abs(Noise::Evaluate(vertices[i].Pos)*1.f);
        vertices[i].Normal = { 0.f,0.f,0.f };
        vertices[i].Tex = earth.Vertices[i].TexC;
    }
    std::vector<std::uint32_t> indices = earth.Indices32;

    // calculate normals
    size_t faces = indices.size() / 3;
    for (size_t i = 0; i < faces; ++i)
    {
        Vector3f v0 = vertices[indices[i * 3 + 0]].Pos;
        Vector3f v1 = vertices[indices[i * 3 + 1]].Pos;
        Vector3f v2 = vertices[indices[i * 3 + 2]].Pos;
        Vector3f e0 = v1 - v0;
        Vector3f e1 = v2 - v0;
        Vector3f normal = Vector3Cross(e0, e1);

        vertices[indices[i * 3 + 0]].Normal += normal;
        vertices[indices[i * 3 + 1]].Normal += normal;
        vertices[indices[i * 3 + 2]].Normal += normal;
    }
    for (size_t i = 0; i < vertices.size(); ++i)
    {
        vertices[i].Normal.Normalize();
    }

    std::cout << "vertex num:" << vertices.size() << std::endl;
    std::cout << "face num:" << faces << std::endl;

    mMeshDesc.emplace_back();
    mMeshDesc.back().name = "Earth";
    mMeshDesc.back().indexCount = (unsigned int)indices.size();
    mMeshDesc.back().indexFormat = Index_Format::UINT32;
    mMeshDesc.back().indices = indices.data();

    mMeshDesc.back().vertexDataCount.push_back((unsigned int)vertices.size());
    mMeshDesc.back().vertexDataSize.push_back(sizeof(FLVertexNormalTex));
    mMeshDesc.back().vertexData.push_back(vertices.data());

    // sub meshes
    mMeshDesc.back().subMeshs.emplace_back("All", (UINT)indices.size());
    
    mEngine.GetScene()->AddPrimitive(mMeshDesc.back());
}

void ProceduralTerrainApp::BuildRenderItems()
{
    using namespace DirectX;

    FireFlame::stRenderItemDesc RItem("Earth", mMeshDesc[0].subMeshs[0]);
    XMFLOAT4X4 trans[2];
    XMStoreFloat4x4
    (
        &trans[0],
        XMMatrixTranspose(XMMatrixScaling(100.f,100.f,100.f))
    );
    XMStoreFloat4x4
    (
        &trans[1],
        XMMatrixTranspose(XMMatrixScaling(50.f, 50.f, 50.f))
    );
    RItem.dataLen = sizeof(XMFLOAT4X4)*_countof(trans);
    RItem.data = &trans[0];
    RItem.mat = mMaterials["terrain"].name;
    mRenderItems.emplace_back(RItem);
    mEngine.GetScene()->AddRenderItem
    (
        mMeshDesc[0].name,
        mShaderDesc.name,
        RItem
    );
}