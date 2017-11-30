#include "MPSApp.h"

void MPSApp::Initialize()
{
    AddShaders();
    AddPSOs();
    AddMeshs();
}

void MPSApp::AddShaders()
{
    using namespace FireFlame;
    mShaderDesc.name = "MPSShader";
    mShaderDesc.objCBSize = sizeof(ObjectConsts);
    mShaderDesc.passCBSize = sizeof(PassConstants);
    mShaderDesc.materialCBSize = sizeof(MaterialConstants);
    mShaderDesc.ParamDefault();
    mShaderDesc.AddVertexInput("POSITION", FireFlame::VERTEX_FORMAT_FLOAT3);
    mShaderDesc.AddVertexInput("NORMAL", FireFlame::VERTEX_FORMAT_FLOAT3);
    mShaderDesc.AddVertexInput("TEXCOORD", FireFlame::VERTEX_FORMAT_FLOAT2);
    auto& vs = mShaderDesc.AddShaderStage(L"Shaders\\MPSShader.hlsl", Shader_Type::VS, "VS", "vs_5_0");
    mShaderMacrosVS[""] = "";
    auto& ps = mShaderDesc.AddShaderStage(L"Shaders\\MPSShader.hlsl", Shader_Type::PS, "PS", "ps_5_0");
    mShaderMacrosPS[""] = "";

    std::vector<std::pair<std::string, std::string>> macros = { { "FOG", "1" } };
    auto& psFogged = mShaderDesc.AddShaderStage
    (
        L"Shaders\\MPSShader.hlsl",
        Shader_Type::PS, "PS", "ps_5_0",
        macros
    );
    mShaderMacrosPS["fogged"] = psFogged.Macros2String();

    macros.emplace_back("ALPHA_CLIP", "1");
    auto& psAlphaClip = mShaderDesc.AddShaderStage
    (
        L"Shaders\\MPSShader.hlsl",
        Shader_Type::PS, "PS", "ps_5_0",
        macros
    );
    mShaderMacrosPS["fog_and_alpha_clip"] = psAlphaClip.Macros2String();

    mEngine.GetScene()->AddShader(mShaderDesc);
}

void MPSApp::AddPSOs()
{
    using namespace FireFlame;
    auto scene = Engine::GetEngine()->GetScene();

    PSODesc desc(mShaderDesc.name, mShaderMacrosVS[""], mShaderMacrosPS[""]);
    scene->AddPSO("default", desc);
}

void MPSApp::AddMeshs()
{
    AddRoomMesh();
}

void MPSApp::AddRoomMesh()
{
    // Create and specify geometry.  For this sample we draw a floor
    // and a wall with a mirror on it.  We put the floor, wall, and
    // mirror geometry in one vertex buffer.
    //
    //   |-----------------|
    //   |                 |
    //   |----|-------|----|
    //   |Wall|Mirror |Wall|
    //   |    |       |    |
    //   /-----------------/
    //  /   Floor         /
    // /-----------------/
    using namespace FireFlame;
    std::array<FLVertexNormalTex, 20> vertices =
    {
        // Floor: Observe we tile texture coordinates.
        FLVertexNormalTex(-3.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 4.0f), // 0 
        FLVertexNormalTex(-3.5f, 0.0f,   0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f),
        FLVertexNormalTex(7.5f, 0.0f,   0.0f, 0.0f, 1.0f, 0.0f, 4.0f, 0.0f),
        FLVertexNormalTex(7.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 4.0f, 4.0f),

        // Wall: Observe we tile texture coordinates, and that we
        // leave a gap in the middle for the mirror.
        FLVertexNormalTex(-3.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 2.0f), // 4
        FLVertexNormalTex(-3.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f),
        FLVertexNormalTex(-2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.5f, 0.0f),
        FLVertexNormalTex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.5f, 2.0f),

        FLVertexNormalTex(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 2.0f), // 8 
        FLVertexNormalTex(2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f),
        FLVertexNormalTex(7.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 2.0f, 0.0f),
        FLVertexNormalTex(7.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 2.0f, 2.0f),

        FLVertexNormalTex(-3.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f), // 12
        FLVertexNormalTex(-3.5f, 6.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f),
        FLVertexNormalTex(7.5f, 6.0f, 0.0f, 0.0f, 0.0f, -1.0f, 6.0f, 0.0f),
        FLVertexNormalTex(7.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 6.0f, 1.0f),

        // Mirror
        FLVertexNormalTex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f), // 16
        FLVertexNormalTex(-2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f),
        FLVertexNormalTex(2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f),
        FLVertexNormalTex(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f)
    };

    std::array<std::int16_t, 30> indices =
    {
        // Floor
        0, 1, 2,
        0, 2, 3,

        // Walls
        4, 5, 6,
        4, 6, 7,

        8, 9, 10,
        8, 10, 11,

        12, 13, 14,
        12, 14, 15,

        // Mirror
        16, 17, 18,
        16, 18, 19
    };

    mMeshDesc["room"].name = "room";
    mMeshDesc["room"].indexCount = (unsigned int)indices.size();
    mMeshDesc["room"].indexFormat = Index_Format::UINT16;
    mMeshDesc["room"].indices = indices.data();

    mMeshDesc["room"].vertexDataCount.push_back((unsigned int)vertices.size());
    mMeshDesc["room"].vertexDataSize.push_back(sizeof(FLVertexNormalTex));
    mMeshDesc["room"].vertexData.push_back(vertices.data());

    // sub meshes
    mMeshDesc["room"].subMeshs.emplace_back("floor", 6);
    mMeshDesc["room"].subMeshs.emplace_back("wall", 18, 6);
    mMeshDesc["room"].subMeshs.emplace_back("mirror", 6, 24);
    mEngine.GetScene()->AddPrimitive(mMeshDesc["room"]);
}