#include "ProceduralTerrainApp.h"

ProceduralTerrainApp::ProceduralTerrainApp(FireFlame::Engine& e) :FLEngineApp(e,15.f,1500.f) 
{
    mRadius = 800.f;
    mPixelStep *= 100.f;
}

void ProceduralTerrainApp::Initialize()
{
    BuildShaders();
    BuildGeometry();
    BuildRenderItems();

    mPasses.push_back("DefaultPass");
    mEngine.GetScene()->AddPass(mShaderDesc.name, mPasses[0]);
}

void ProceduralTerrainApp::BuildShaders()
{
    using namespace FireFlame;
    mShaderDesc.name = "ProcedualTerrain";
    mShaderDesc.objCBSize = sizeof(ObjectConsts);
    mShaderDesc.passCBSize = sizeof(PassConstants);
    mShaderDesc.AddVertexInput("POSITION", FireFlame::VERTEX_FORMAT_FLOAT3);
    mShaderDesc.AddVertexInput("COLOR", FireFlame::VERTEX_FORMAT_FLOAT4);
    mShaderDesc.AddShaderStage(L"Shaders\\ProceduralTerrain.hlsl", Shader_Type::VS, "VS", "vs_5_0");
    mShaderDesc.AddShaderStage(L"Shaders\\ProceduralTerrain.hlsl", Shader_Type::PS, "PS", "ps_5_0");

    mEngine.GetScene()->AddShader(mShaderDesc);
}

void ProceduralTerrainApp::BuildGeometry()
{
    using namespace FireFlame;

    GeometryGenerator geoGen;
    GeometryGenerator::MeshData earth = geoGen.CreateGeosphere(500.f, 5);

    std::vector<FireFlame::FLVertex> vertices(earth.Vertices.size());
    for (size_t i = 0; i < earth.Vertices.size(); ++i)
    {
        vertices[i].Pos = earth.Vertices[i].Position;
        vertices[i].Color = { 0.000000000f, 0.392156899f, 0.000000000f, 1.000000000f };
    }

    std::vector<std::uint16_t> indices = earth.GetIndices16();

    mMeshDesc.emplace_back();
    mMeshDesc[0].name = "Earth";
    mMeshDesc[0].indexCount = (unsigned int)indices.size();
    mMeshDesc[0].indexFormat = Index_Format::UINT16;
    mMeshDesc[0].indices = indices.data();

    mMeshDesc[0].vertexDataCount.push_back((unsigned int)vertices.size());
    mMeshDesc[0].vertexDataSize.push_back(sizeof(FLVertex));
    mMeshDesc[0].vertexData.push_back(vertices.data());

    // sub meshes
    mMeshDesc[0].subMeshs.emplace_back("Box", (UINT)indices.size());
    
    mEngine.GetScene()->AddPrimitive(mMeshDesc[0]);
}

void ProceduralTerrainApp::BuildRenderItems()
{
    using namespace DirectX;

    FireFlame::stRenderItemDesc RItemDesc("Earth", mMeshDesc[0].subMeshs[0]);
    XMFLOAT4X4 worldTrans = FireFlame::Matrix4X4();
    /*XMStoreFloat4x4
    (
        &worldTrans,
        XMMatrixTranspose(XMMatrixScaling(2.0f, 2.0f, 2.0f)*XMMatrixTranslation(0.0f, 0.5f, 0.0f))
    );*/
    RItemDesc.dataLen = sizeof(XMFLOAT4X4);
    RItemDesc.data = &worldTrans;
    mRenderItems.emplace_back(RItemDesc);
    mEngine.GetScene()->AddRenderItem
    (
        mMeshDesc[0].name,
        mShaderDesc.name,
        RItemDesc
    );
}