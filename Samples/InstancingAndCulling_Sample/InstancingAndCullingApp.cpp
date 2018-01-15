#include "InstancingAndCullingApp.h"
#include <fstream>
#include <assert.h>

void InstancingAndCullingApp::PreInitialize() {}

void InstancingAndCullingApp::Initialize()
{
    AddShaders();
    AddPSOs();
    AddTextures();
    AddMaterials();
    AddMeshs();
    AddRenderItems();

    mPasses.push_back("DefaultPass");
    mEngine.GetScene()->AddPass(mPasses[0]);

    mCamera.SetPosition(0.0f, 2.0f, -15.0f);
}

void InstancingAndCullingApp::Update(float time_elapsed)
{
    FLEngineApp4::Update(time_elapsed);
}

void InstancingAndCullingApp::UpdateMainPassCB(float time_elapsed)
{
    using namespace DirectX;

    // pass constants
    XMMATRIX view = mCamera.GetView();
    XMMATRIX proj = mCamera.GetProj();

    XMMATRIX viewProj = XMMatrixMultiply(view, proj);
    XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
    XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
    XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);

    auto& passCB = mPassCBs["main"];
    XMStoreFloat4x4(&passCB.View, XMMatrixTranspose(view));
    XMStoreFloat4x4(&passCB.InvView, XMMatrixTranspose(invView));
    XMStoreFloat4x4(&passCB.Proj, XMMatrixTranspose(proj));
    XMStoreFloat4x4(&passCB.InvProj, XMMatrixTranspose(invProj));
    XMStoreFloat4x4(&passCB.ViewProj, XMMatrixTranspose(viewProj));
    XMStoreFloat4x4(&passCB.InvViewProj, XMMatrixTranspose(invViewProj));
    passCB.EyePosW = mCamera.GetPosition3f();
    float clientWidth = (float)mEngine.GetWindow()->ClientWidth();
    float clientHeight = (float)mEngine.GetWindow()->ClientHeight();
    passCB.RenderTargetSize = XMFLOAT2(clientWidth, clientHeight);
    passCB.InvRenderTargetSize = XMFLOAT2(1.0f / clientWidth, 1.0f / clientHeight);
    passCB.NearZ = 1.0f;
    passCB.FarZ = 1000.0f;
    passCB.TotalTime = mEngine.TotalTime();
    passCB.DeltaTime = mEngine.DeltaTime();

    passCB.AmbientLight = { 0.35f, 0.35f, 0.35f, 1.0f };
    passCB.Lights[0].Direction = { 0.57735f, -0.57735f, 0.57735f };
    passCB.Lights[0].Strength = { 0.8f, 0.8f, 0.8f };
    passCB.Lights[1].Direction = { -0.57735f, -0.57735f, 0.57735f };
    passCB.Lights[1].Strength = { 0.4f, 0.4f, 0.4f };
    passCB.Lights[2].Direction = { 0.0f, -0.707f, -0.707f };
    passCB.Lights[2].Strength = { 0.2f, 0.2f, 0.2f };

    mEngine.GetScene()->UpdateShaderPassCBData(mShaderDescs["main"].name, sizeof(PassConstantsLight), &passCB);
}

void InstancingAndCullingApp::AddShaders()
{
    using namespace FireFlame;

    auto& shaderDesc = mShaderDescs["main"];
    shaderDesc.name = "main";
    shaderDesc.objCBSize = sizeof(ObjectConsts);
    shaderDesc.maxObjCBDescriptor = 125;
    shaderDesc.materialCBSize = sizeof(MaterialConstants);
    shaderDesc.passCBSize = sizeof(PassConstantsLight);
    shaderDesc.objParamIndex = -1;
    shaderDesc.passParamIndex = 1;
    shaderDesc.texSRVDescriptorTableSize = 7;
    shaderDesc.maxTexSRVDescriptor = 7;
    shaderDesc.useDynamicMat = true;
    shaderDesc.useInstancing = true;

    std::wstring strHlslFile = L"Shaders\\Main.hlsl";
    shaderDesc.AddVertexInput("POSITION", FireFlame::VERTEX_FORMAT_FLOAT3);
    shaderDesc.AddVertexInput("NORMAL", FireFlame::VERTEX_FORMAT_FLOAT3);
    shaderDesc.AddVertexInput("TEXCOORD", FireFlame::VERTEX_FORMAT_FLOAT2);
    shaderDesc.AddShaderStage(strHlslFile, Shader_Type::VS, "VS", "vs_5_1");
    shaderDesc.AddShaderStage(strHlslFile, Shader_Type::PS, "PS", "ps_5_1");

    mEngine.GetScene()->AddShader(shaderDesc);
}

void InstancingAndCullingApp::AddPSOs()
{
    using namespace FireFlame;

    PSODesc descDefault(mShaderDescs["main"].name);
    mEngine.GetScene()->AddPSO("default", descDefault);
}

void InstancingAndCullingApp::AddTextures()
{
    mEngine.GetScene()->AddTexture
    (
        "bricksTex",
        L"../../Resources/Textures/bricks.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "stoneTex",
        L"../../Resources/Textures/stone.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "tileTex",
        L"../../Resources/Textures/tile.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "crateTex",
        L"../../Resources/Textures/WoodCrate01.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "iceTex",
        L"../../Resources/Textures/ice.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "grassTex",
        L"../../Resources/Textures/grass.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "defaultTex",
        L"../../Resources/Textures/white1x1.dds"
    );
    mEngine.GetScene()->AddTextureGroup
    (
        "main",
        { 
            FireFlame::TEX("bricksTex"),
            FireFlame::TEX("stoneTex"),
            FireFlame::TEX("tileTex"),
            FireFlame::TEX("crateTex"),
            FireFlame::TEX("iceTex"),
            FireFlame::TEX("grassTex"),
            FireFlame::TEX("defaultTex")
        }
    );
}

void InstancingAndCullingApp::AddMaterials()
{
    auto& bricks0 = mMaterials["bricks0"];
    bricks0.Name = "bricks0";
    bricks0.DiffuseMapIndex = 0;
    bricks0.DiffuseAlbedo = FireFlame::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    bricks0.FresnelR0 = FireFlame::Vector3f(0.02f, 0.02f, 0.02f);
    bricks0.Roughness = 0.1f;
    mEngine.GetScene()->AddMaterial
    (
        bricks0.Name,
        mShaderDescs["main"].name, "",
        sizeof(MaterialConstants), &bricks0
    );

    auto& stone0 = mMaterials["stone0"];
    stone0.Name = "stone0";
    stone0.DiffuseMapIndex = 1;
    stone0.DiffuseAlbedo = FireFlame::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    stone0.FresnelR0 = FireFlame::Vector3f(0.05f, 0.05f, 0.05f);
    stone0.Roughness = 0.3f;
    mEngine.GetScene()->AddMaterial
    (
        stone0.Name,
        mShaderDescs["main"].name, "",
        sizeof(MaterialConstants), &stone0
    );

    auto& tile0 = mMaterials["tile0"];
    tile0.Name = "tile0";
    tile0.DiffuseMapIndex = 2;
    tile0.DiffuseAlbedo = FireFlame::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    tile0.FresnelR0 = FireFlame::Vector3f(0.02f, 0.02f, 0.02f);
    tile0.Roughness = 0.3f;
    mEngine.GetScene()->AddMaterial
    (
        tile0.Name,
        mShaderDescs["main"].name, "",
        sizeof(MaterialConstants), &tile0
    );

    auto& checkboard0 = mMaterials["checkboard0"];
    checkboard0.Name = "checkboard0";
    checkboard0.DiffuseMapIndex = 3;
    checkboard0.DiffuseAlbedo = FireFlame::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    checkboard0.FresnelR0 = FireFlame::Vector3f(0.05f, 0.05f, 0.05f);
    checkboard0.Roughness = 0.2f;
    mEngine.GetScene()->AddMaterial
    (
        checkboard0.Name,
        mShaderDescs["main"].name, "",
        sizeof(MaterialConstants), &checkboard0
    );

    auto& ice0 = mMaterials["ice0"];
    ice0.Name = "ice0";
    ice0.DiffuseMapIndex = 4;
    ice0.DiffuseAlbedo = FireFlame::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    ice0.FresnelR0 = FireFlame::Vector3f(0.1f, 0.1f, 0.1f);
    ice0.Roughness = 0.0f;
    mEngine.GetScene()->AddMaterial
    (
        ice0.Name,
        mShaderDescs["main"].name, "",
        sizeof(MaterialConstants), &ice0
    );

    auto& grass0 = mMaterials["grass0"];
    grass0.Name = "grass0";
    grass0.DiffuseMapIndex = 5;
    grass0.DiffuseAlbedo = FireFlame::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    grass0.FresnelR0 = FireFlame::Vector3f(0.05f, 0.05f, 0.05f);
    grass0.Roughness = 0.2f;
    mEngine.GetScene()->AddMaterial
    (
        grass0.Name,
        mShaderDescs["main"].name, "",
        sizeof(MaterialConstants), &grass0
    );

    auto& skullMat = mMaterials["skullMat"];
    skullMat.Name = "skullMat";
    skullMat.DiffuseMapIndex = 6;
    skullMat.DiffuseAlbedo = FireFlame::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    skullMat.FresnelR0 = FireFlame::Vector3f(0.05f, 0.05f, 0.05f);
    skullMat.Roughness = 0.5f;
    mEngine.GetScene()->AddMaterial
    (
        skullMat.Name,
        mShaderDescs["main"].name, "",
        sizeof(MaterialConstants), &skullMat
    );
}

void InstancingAndCullingApp::AddMeshs()
{
    std::ifstream fin("../../Resources/Geometry/skull.txt");
    if (!fin)
    {
        MessageBox(0, L"Models/skull.txt not found.", 0, 0);
        return;
    }

    UINT vcount = 0;
    UINT tcount = 0;
    std::string ignore;

    fin >> ignore >> vcount;
    fin >> ignore >> tcount;
    fin >> ignore >> ignore >> ignore >> ignore;

    auto maxfloat = (std::numeric_limits<float>::max)();
    auto minfloat = (std::numeric_limits<float>::min)();
    DirectX::XMFLOAT3 vMinf3(maxfloat, maxfloat, maxfloat);
    DirectX::XMFLOAT3 vMaxf3(minfloat, minfloat, minfloat);

    DirectX::XMVECTOR vMin = XMLoadFloat3(&vMinf3);
    DirectX::XMVECTOR vMax = XMLoadFloat3(&vMaxf3);

    std::vector<FireFlame::FLVertexNormalTex> vertices(vcount);
    for (UINT i = 0; i < vcount; ++i)
    {
        fin >> vertices[i].Pos.x >> vertices[i].Pos.y >> vertices[i].Pos.z;
        fin >> vertices[i].Normal.x >> vertices[i].Normal.y >> vertices[i].Normal.z;

        DirectX::XMFLOAT3 Pos(vertices[i].Pos.x, vertices[i].Pos.y, vertices[i].Pos.z);
        DirectX::XMVECTOR P = XMLoadFloat3(&Pos);
        // Project point onto unit sphere and generate spherical texture coordinates.
        DirectX::XMFLOAT3 spherePos;
        XMStoreFloat3(&spherePos, DirectX::XMVector3Normalize(P));

        float theta = atan2f(spherePos.z, spherePos.x);

        // Put in [0, 2pi].
        if (theta < 0.0f)
            theta += DirectX::XM_2PI;

        float phi = acosf(spherePos.y);

        float u = theta / (2.0f*DirectX::XM_PI);
        float v = phi / DirectX::XM_PI;

        vertices[i].Tex = { u, v };

        vMin = DirectX::XMVectorMin(vMin, P);
        vMax = DirectX::XMVectorMax(vMax, P);
    }

    using namespace DirectX;
    DirectX::BoundingBox bounds;
    XMStoreFloat3(&bounds.Center, 0.5f*(vMin + vMax));
    XMStoreFloat3(&bounds.Extents, 0.5f*(vMax - vMin));

    fin >> ignore;
    fin >> ignore;
    fin >> ignore;

    std::vector<std::int32_t> indices(3 * tcount);
    for (UINT i = 0; i < tcount; ++i)
    {
        fin >> indices[i * 3 + 0] >> indices[i * 3 + 1] >> indices[i * 3 + 2];
    }

    fin.close();

    using namespace FireFlame;

    auto& meshDesc = mMeshDescs["skull"];
    meshDesc.name = "skull";
    meshDesc.indexCount = (unsigned int)indices.size();
    meshDesc.indexFormat = Index_Format::UINT32;
    meshDesc.indices = indices.data();

    meshDesc.vertexData.push_back(vertices.data());
    meshDesc.vertexDataCount.push_back((unsigned)vertices.size());
    meshDesc.vertexDataSize.push_back(sizeof(FLVertexNormalTex));

    // sub meshes
    meshDesc.subMeshs.emplace_back("all", (UINT)indices.size());
    mEngine.GetScene()->AddPrimitive(meshDesc);
}

void InstancingAndCullingApp::AddRenderItems()
{
    using namespace DirectX;

    FireFlame::stRenderItemDesc skullRitem("skull", mMeshDescs["skull"].subMeshs[0]);
    
    // Generate instance data.
    const int n = 5;
    skullRitem.dataLen = sizeof(ObjectConsts);
    skullRitem.dataCount = n * n * n;
    skullRitem.data = new ObjectConsts[skullRitem.dataCount];

    float width = 200.0f;
    float height = 200.0f;
    float depth = 200.0f;

    float x = -0.5f*width;
    float y = -0.5f*height;
    float z = -0.5f*depth;
    float dx = width / (n - 1);
    float dy = height / (n - 1);
    float dz = depth / (n - 1);
    for (int k = 0; k < n; ++k)
    {
        for (int i = 0; i < n; ++i)
        {
            for (int j = 0; j < n; ++j)
            {
                int index = k * n*n + i * n + j;
                ObjectConsts objConsts;
                auto world = XMFLOAT4X4(
                    1.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 1.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 1.0f, 0.0f,
                    x + j * dx, y + i * dy, z + k * dz, 1.0f);
                auto worldTrans = XMLoadFloat4x4(&world);
                XMStoreFloat4x4(&objConsts.World, XMMatrixTranspose(worldTrans));
                XMStoreFloat4x4(&objConsts.TexTransform, XMMatrixTranspose(XMMatrixScaling(2.0f, 2.0f, 1.0f)));
                objConsts.MaterialIndex = index % mMaterials.size();
            }
        }
    }

    mRenderItems[skullRitem.name] = skullRitem;
    mEngine.GetScene()->AddRenderItem
    (
        mMeshDescs["skull"].name,
        mShaderDescs["main"].name,
        "default",
        skullRitem
    );
}

void InstancingAndCullingApp::OnKeyUp(WPARAM wParam, LPARAM lParam)
{
    FLEngineApp4::OnKeyUp(wParam, lParam);
}