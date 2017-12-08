#include "ProceduralTerrainApp.h"
#include <fstream>

ProceduralTerrainApp::ProceduralTerrainApp(FireFlame::Engine& e) :FLEngineApp(e,1.f,150.f) 
{
    mRadius = 22.f;
    //mPixelStep *= 1.f;

    //mTheta = 0.f;
    //mPhi = 0.f;
}

void ProceduralTerrainApp::Initialize()
{
    BuildShaders();
    AddTextures();
    AddMaterials();
    BuildGeometry();
    BuildRenderItems();

    mPasses.push_back("DefaultPass");
    mEngine.GetScene()->AddPass(mPasses[0]);
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
        mShaderDesc.name, 
        { 
            {"darkdirtTex", FireFlame::SRV_DIMENSION::TEXTURE2D },
            {"lightdirtTex", FireFlame::SRV_DIMENSION::TEXTURE2D },
            {"snowTex", FireFlame::SRV_DIMENSION::TEXTURE2D },
        },
        sizeof(MaterialConstants), &terrain
    }
    );
}

void ProceduralTerrainApp::AddTextures()
{
    auto data = GetResource(103, L"DDS");
    mEngine.GetScene()->AddTexture
    (
        "darkdirtTex",
        data.first, data.second
    );

    data = GetResource(104, L"DDS");
    mEngine.GetScene()->AddTexture
    (
        "lightdirtTex",
        data.first, data.second
    );

    data = GetResource(106, L"DDS");
    mEngine.GetScene()->AddTexture
    (
        "snowTex",
        data.first, data.second
    );
}

void ProceduralTerrainApp::Update(float time_elapsed)
{
    using namespace DirectX;

    OnKeyboardInput(time_elapsed);

    UpdateCamera(time_elapsed);

    /*ObjectConsts ObjConstants;
    for (const auto& RItem : mRenderItems) {
    mEngine.GetScene()->UpdateRenderItemCBData(RItem.name, sizeof(ObjectConsts), &ObjConstants);
    }*/

    // pass constants
    XMMATRIX view = XMLoadFloat4x4(&mView);
    XMMATRIX proj = XMLoadFloat4x4(&mProj);

    XMMATRIX viewProj = XMMatrixMultiply(view, proj);
    XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
    XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
    XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);

    XMStoreFloat4x4(&mMainPassCB.View, XMMatrixTranspose(view));
    XMStoreFloat4x4(&mMainPassCB.InvView, XMMatrixTranspose(invView));
    XMStoreFloat4x4(&mMainPassCB.Proj, XMMatrixTranspose(proj));
    XMStoreFloat4x4(&mMainPassCB.InvProj, XMMatrixTranspose(invProj));
    XMStoreFloat4x4(&mMainPassCB.ViewProj, XMMatrixTranspose(viewProj));
    XMStoreFloat4x4(&mMainPassCB.InvViewProj, XMMatrixTranspose(invViewProj));
    mMainPassCB.EyePosW = mEyePos;
    float clientWidth = (float)mEngine.GetWindow()->ClientWidth();
    float clientHeight = (float)mEngine.GetWindow()->ClientHeight();
    mMainPassCB.RenderTargetSize = XMFLOAT2(clientWidth, clientHeight);
    mMainPassCB.InvRenderTargetSize = XMFLOAT2(1.0f / clientWidth, 1.0f / clientHeight);
    mMainPassCB.NearZ = 1.0f;
    mMainPassCB.FarZ = 1000.0f;
    mMainPassCB.TotalTime = mEngine.TotalTime();
    mMainPassCB.DeltaTime = mEngine.DeltaTime();

    mMainPassCB.AmbientLight = { 0.25f, 0.25f, 0.35f, 1.0f };
    mMainPassCB.Lights[0].Direction = -FireFlame::MathHelper::SphericalToCartesian(1.0f, mSunTheta, mSunPhi);
    mMainPassCB.Lights[0].Strength = { 1.0f, 1.0f, 0.9f };

    UpdateMainPassCB(time_elapsed);
    if (mPasses.size())
        mEngine.GetScene()->UpdateShaderPassCBData(mShaderDesc.name, sizeof(PassConstants), &mMainPassCB);
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
    mMainPassCB.minHeight = mMinHeight;
    mMainPassCB.maxHeight = mMaxHeight;
}

void ProceduralTerrainApp::BuildShaders()
{
    using namespace FireFlame;

    std::string shaderdata = (const char*)GetResource(101, L"SHADER").first;

    mShaderDesc.name = "ProcedualTerrain";
    mShaderDesc.objCBSize = sizeof(ObjectConsts);
    mShaderDesc.passCBSize = sizeof(PassConstants);
    mShaderDesc.materialCBSize = sizeof(MaterialConstants);
    mShaderDesc.ParamDefault();
    mShaderDesc.texSRVDescriptorTableSize = 4;
    mShaderDesc.AddVertexInput("POSITION", FireFlame::VERTEX_FORMAT_FLOAT3);
    mShaderDesc.AddVertexInput("NORMAL", FireFlame::VERTEX_FORMAT_FLOAT3);
    mShaderDesc.AddVertexInput("TEXCOORD", FireFlame::VERTEX_FORMAT_FLOAT2);
    mShaderDesc.AddShaderStage(shaderdata/*L"Shaders\\ProceduralTerrain.hlsl"*/, Shader_Type::VS, "VS", "vs_5_0");
    mShaderDesc.AddShaderStage(shaderdata/*L"Shaders\\ProceduralTerrain.hlsl"*/, Shader_Type::PS, "PS", "ps_5_0");

    mEngine.GetScene()->AddShader(mShaderDesc);
}

void ProceduralTerrainApp::BuildGeometry()
{
    std::vector<FireFlame::FLVertexNormalTex> vertices;
    std::vector<std::uint32_t>             indices;
    if (FireFlame::PLYLoader::Load(mTerrainFile, vertices, indices))
    {
        for (auto& vertex : vertices)
        {
            vertex.Pos *= 80.f;
        }

        mMeshDesc.emplace_back();
        mMeshDesc.back().name = "grid";
        mMeshDesc.back().indexCount = (unsigned int)indices.size();
        mMeshDesc.back().indexFormat = FireFlame::Index_Format::UINT32;
        mMeshDesc.back().indices = indices.data();

        mMeshDesc.back().vertexDataCount.push_back((unsigned int)vertices.size());
        mMeshDesc.back().vertexDataSize.push_back(sizeof(FireFlame::FLVertexNormalTex));
        mMeshDesc.back().vertexData.push_back(vertices.data());

        // sub meshes
        mMeshDesc.back().subMeshs.emplace_back("All", (UINT)indices.size());
        mEngine.GetScene()->AddPrimitive(mMeshDesc.back());

        std::cout << "vertex count:" << vertices.size() << std::endl;
        std::cout << "face count:" << indices.size() / 3 << std::endl;

        // decide scale
        mMinHeight = (std::numeric_limits<float>::max)();
        mMaxHeight = (std::numeric_limits<float>::min)();
        for (size_t i = 0; i < vertices.size(); ++i)
        {
            if (vertices[i].Pos.y > mMaxHeight)
                mMaxHeight = vertices[i].Pos.y;
            if (vertices[i].Pos.y < mMinHeight)
                mMinHeight = vertices[i].Pos.y;
        }
        //mModelScale = mRadius / (((std::max)((std::max)(maxX, maxY), maxZ) - minY)*4.f);
        //mModelTransY = -minY*mModelScale;
    }
}

void ProceduralTerrainApp::BuildRenderItems()
{
    using namespace DirectX;

    FireFlame::stRenderItemDesc RItem("Earth", mMeshDesc[0].subMeshs[0]);
    XMFLOAT4X4 trans[2];
    XMStoreFloat4x4
    (
        &trans[0],
        XMMatrixTranspose
        (
            XMMatrixScaling(mModelScale, mModelScale, mModelScale)*
            XMMatrixTranslation(0.0f, mModelTransY, 0.0f)
        )
    );
    XMStoreFloat4x4
    (
        &trans[1],
        XMMatrixTranspose(XMMatrixScaling(10.f, 10.f, 10.f))
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

std::pair<std::uint8_t*, size_t> ProceduralTerrainApp::GetResource(int resID, const wchar_t* resType)
{
    HRSRC hRsrc = FindResource(nullptr, MAKEINTRESOURCE(resID), resType);
    if (NULL == hRsrc)
        throw std::exception("unable to find shader resources");
    DWORD dwSize = SizeofResource(NULL, hRsrc);
    HGLOBAL hGlobal = LoadResource(NULL, hRsrc);
    if (NULL == hGlobal)
        throw std::exception("unable to load shader resources");

    LPVOID pBuffer = LockResource(hGlobal);
    if (NULL == pBuffer)
        throw std::exception("unable to lock shader resources");

    return std::make_pair((std::uint8_t*)pBuffer, (size_t)dwSize);
}