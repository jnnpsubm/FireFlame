#include "MPSApp.h"

MPSApp::MPSApp(FireFlame::Engine& e) : FLEngineApp2(e)
{
    float mTheta = 1.24f*FireFlame::MathHelper::FL_PI;
    float mPhi = 0.42f*FireFlame::MathHelper::FL_PI;
    float mRadius = 12.0f;
}

void MPSApp::Initialize()
{
    AddShaders();
    AddPSOs();
    AddMeshs();
    AddTextures();
    AddMaterials();
    AddRenderItems();
    AddPasses();
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

void MPSApp::AddTextures()
{
    mEngine.GetScene()->AddTexture
    (
        "bricksTex",
        L"..\\..\\Resources\\Textures\\bricks3.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "checkboardTex",
        L"..\\..\\Resources\\Textures\\checkboard.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "iceTex",
        L"..\\..\\Resources\\Textures\\ice.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "white1x1Tex",
        L"..\\..\\Resources\\Textures\\white1x1.dds"
    );
}

void MPSApp::AddMaterials()
{
    auto& bricks = mMaterials["bricks"];
    bricks.Name = "bricks";
    bricks.DiffuseAlbedo = FireFlame::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    bricks.FresnelR0 = FireFlame::Vector3f(0.05f, 0.05f, 0.05f);
    bricks.Roughness = 0.25f;
    mEngine.GetScene()->AddMaterial
    (
        bricks.Name,
        mShaderDesc.name, "bricksTex",
        sizeof(MaterialConstants), &bricks
    );

    auto& checkertile = mMaterials["checkertile"];
    checkertile.Name = "checkertile";
    checkertile.DiffuseAlbedo = FireFlame::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    checkertile.FresnelR0 = FireFlame::Vector3f(0.07f, 0.07f, 0.07f);
    checkertile.Roughness = 0.3f;
    mEngine.GetScene()->AddMaterial
    (
        checkertile.Name,
        mShaderDesc.name, "checkboardTex",
        sizeof(MaterialConstants), &checkertile
    );

    auto& icemirror = mMaterials["icemirror"];
    icemirror.Name = "icemirror";
    icemirror.DiffuseAlbedo = FireFlame::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    icemirror.FresnelR0 = FireFlame::Vector3f(0.1f, 0.1f, 0.1f);
    icemirror.Roughness = 0.5f;
    mEngine.GetScene()->AddMaterial
    (
        icemirror.Name,
        mShaderDesc.name, "iceTex",
        sizeof(MaterialConstants), &icemirror
    );

    auto& skullMat = mMaterials["skullMat"];
    skullMat.Name = "skullMat";
    skullMat.DiffuseAlbedo = FireFlame::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    skullMat.FresnelR0 = FireFlame::Vector3f(0.05f, 0.05f, 0.05f);
    skullMat.Roughness = 0.3f;
    mEngine.GetScene()->AddMaterial
    (
        skullMat.Name,
        mShaderDesc.name, "white1x1Tex",
        sizeof(MaterialConstants), &skullMat
    );

    auto& shadowMat = mMaterials["shadowMat"];
    shadowMat.Name = "shadowMat";
    shadowMat.DiffuseAlbedo = FireFlame::Vector4f(0.0f, 0.0f, 0.0f, 0.5f);
    shadowMat.FresnelR0 = FireFlame::Vector3f(0.001f, 0.001f, 0.001f);
    shadowMat.Roughness = 0.0f;
    mEngine.GetScene()->AddMaterial
    (
        shadowMat.Name,
        mShaderDesc.name, "white1x1Tex",
        sizeof(MaterialConstants), &shadowMat
    );
}

void MPSApp::AddRenderItems()
{
    using namespace DirectX;

    FireFlame::stRenderItemDesc RItem("floor", mMeshDesc["room"].subMeshs[0]);
    RItem.mat = "checkertile";
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
    mRenderItems[RItem.name] = RItem;
    mEngine.GetScene()->AddRenderItem
    (
        mMeshDesc["room"].name,
        mShaderDesc.name,
        "default",
        RItem
    );

    /*FireFlame::stRenderItemDesc RItem2("Waves", mMeshDesc[1].subMeshs[0]);
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
    );*/
}

void MPSApp::AddPasses()
{
    mPasses.push_back("DefaultPass");
    mEngine.GetScene()->AddPass(mShaderDesc.name, mPasses[0]);
}