#include "MPSApp.h"
#include <fstream>

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
    AddSkullMesh();
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

void MPSApp::AddSkullMesh()
{
    using namespace FireFlame;

    std::ifstream fin("../../Resources/geometry/skull.txt");

    if (!fin)
    {
        MessageBox(0, L"../../Resources/geometry/skull.txt not found.", 0, 0);
        return;
    }

    UINT vcount = 0;
    UINT tcount = 0;
    std::string ignore;

    fin >> ignore >> vcount;
    fin >> ignore >> tcount;
    fin >> ignore >> ignore >> ignore >> ignore;

    std::vector<FLVertexNormalTex> vertices(vcount);
    for (UINT i = 0; i < vcount; ++i)
    {
        fin >> vertices[i].Pos.x >> vertices[i].Pos.y >> vertices[i].Pos.z;
        fin >> vertices[i].Normal.x >> vertices[i].Normal.y >> vertices[i].Normal.z;

        // Model does not have texture coordinates, so just zero them out.
        vertices[i].Tex = { 0.0f, 0.0f };
    }

    fin >> ignore;
    fin >> ignore;
    fin >> ignore;

    std::vector<std::uint32_t> indices(3 * tcount);
    for (UINT i = 0; i < tcount; ++i)
    {
        fin >> indices[i * 3 + 0] >> indices[i * 3 + 1] >> indices[i * 3 + 2];
    }

    fin.close();

    auto& meshDesc = mMeshDesc["skull"];
    meshDesc.name = "skull";

    meshDesc.indexCount = (unsigned int)indices.size();
    meshDesc.indexFormat = Index_Format::UINT32;
    meshDesc.indices = indices.data();

    meshDesc.vertexDataCount.push_back((unsigned int)vertices.size());
    meshDesc.vertexDataSize.push_back(sizeof(FLVertexNormalTex));
    meshDesc.vertexData.push_back(vertices.data());

    // sub meshes
    meshDesc.subMeshs.emplace_back("all", (unsigned int)indices.size());
    mEngine.GetScene()->AddPrimitive(meshDesc);
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
    AddRenderItemFloor();
    AddRenderItemWall();
    AddRenderItemSkull();
}

void MPSApp::AddRenderItemFloor()
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
}

void MPSApp::AddRenderItemWall()
{
    using namespace DirectX;

    FireFlame::stRenderItemDesc RItem("wall", mMeshDesc["room"].subMeshs[1]);
    RItem.mat = "bricks";
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
}

void MPSApp::AddRenderItemSkull()
{
    using namespace DirectX;

    FireFlame::stRenderItemDesc RItem("skull", mMeshDesc["skull"].subMeshs[0]);
    RItem.mat = "skullMat";
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
        mMeshDesc["skull"].name,
        mShaderDesc.name,
        "default",
        RItem
    );
}

void MPSApp::AddPasses()
{
    mPasses.push_back("DefaultPass");
    mEngine.GetScene()->AddPass(mShaderDesc.name, mPasses[0]);
}

void MPSApp::OnKeyboardInput(float time_elapsed)
{
    //
    // Allow user to move skull.
    //

    const float dt = mEngine.DeltaTime();

    if (GetAsyncKeyState('A') & 0x8000)
        mSkullTranslation.x -= 1.0f*dt;

    if (GetAsyncKeyState('D') & 0x8000)
        mSkullTranslation.x += 1.0f*dt;

    if (GetAsyncKeyState('W') & 0x8000)
        mSkullTranslation.y += 1.0f*dt;

    if (GetAsyncKeyState('S') & 0x8000)
        mSkullTranslation.y -= 1.0f*dt;

    // Don't let user move below ground plane.
    mSkullTranslation.y = (std::max)(mSkullTranslation.y, 0.0f);

    // Update the new world matrix.
    DirectX::XMMATRIX skullRotate = DirectX::XMMatrixRotationY(0.5f*FireFlame::MathHelper::FL_PI);
    DirectX::XMMATRIX skullScale = DirectX::XMMatrixScaling(0.45f, 0.45f, 0.45f);
    DirectX::XMMATRIX skullOffset = DirectX::XMMatrixTranslation(mSkullTranslation.x, mSkullTranslation.y, mSkullTranslation.z);
    DirectX::XMMATRIX skullWorld = skullRotate*skullScale*skullOffset;
    DirectX::XMFLOAT4X4 transform[2];
    DirectX::XMStoreFloat4x4(&transform[0], DirectX::XMMatrixTranspose(skullWorld));
    DirectX::XMStoreFloat4x4(&transform[1], DirectX::XMMatrixIdentity());
    mEngine.GetScene()->UpdateRenderItemCBData("skull", sizeof(transform), &transform[0]);

    //// Update reflection world matrix.
    //XMVECTOR mirrorPlane = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f); // xy plane
    //XMMATRIX R = XMMatrixReflect(mirrorPlane);
    //XMStoreFloat4x4(&mReflectedSkullRitem->World, skullWorld * R);

    //// Update shadow world matrix.
    //XMVECTOR shadowPlane = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); // xz plane
    //XMVECTOR toMainLight = -XMLoadFloat3(&mMainPassCB.Lights[0].Direction);
    //XMMATRIX S = XMMatrixShadow(shadowPlane, toMainLight);
    //XMMATRIX shadowOffsetY = XMMatrixTranslation(0.0f, 0.001f, 0.0f);
    //XMStoreFloat4x4(&mShadowedSkullRitem->World, skullWorld * S * shadowOffsetY);

    //mSkullRitem->NumFramesDirty = gNumFrameResources;
    //mReflectedSkullRitem->NumFramesDirty = gNumFrameResources;
    //mShadowedSkullRitem->NumFramesDirty = gNumFrameResources;
}