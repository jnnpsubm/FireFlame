#include "PickingApp.h"
#include <fstream>
#include <assert.h>

void PickingApp::PreInitialize() {}

void PickingApp::Initialize()
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
    mEngine.GetScene()->SetCamera(&mCamera);
}

void PickingApp::Update(float time_elapsed)
{
    FLEngineApp4::Update(time_elapsed);
}

void PickingApp::UpdateMainPassCB(float time_elapsed)
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

void PickingApp::AddShaders()
{
    using namespace FireFlame;

    auto& shaderDesc = mShaderDescs["main"];
    shaderDesc.name = "main";
    shaderDesc.objCBSize = sizeof(ObjectConsts);
    shaderDesc.maxObjCBDescriptor = 126;
    shaderDesc.materialCBSize = sizeof(MaterialConstants);
    shaderDesc.passCBSize = sizeof(PassConstantsLight);
    shaderDesc.objParamIndex = 0;
    shaderDesc.passParamIndex = 1;
    shaderDesc.texSRVDescriptorTableSize = 7;
    shaderDesc.maxTexSRVDescriptor = 7;
    shaderDesc.useDynamicMat = true;

    std::wstring strHlslFile = L"Shaders\\Main.hlsl";
    shaderDesc.AddVertexInput("POSITION", FireFlame::VERTEX_FORMAT_FLOAT3);
    shaderDesc.AddVertexInput("NORMAL", FireFlame::VERTEX_FORMAT_FLOAT3);
    shaderDesc.AddVertexInput("TEXCOORD", FireFlame::VERTEX_FORMAT_FLOAT2);
    shaderDesc.AddShaderStage(strHlslFile, Shader_Type::VS, "VS", "vs_5_1");
    shaderDesc.AddShaderStage(strHlslFile, Shader_Type::PS, "PS", "ps_5_1");

    mEngine.GetScene()->AddShader(shaderDesc);
}

void PickingApp::AddPSOs()
{
    using namespace FireFlame;

    PSODesc descDefault(mShaderDescs["main"].name);
    mEngine.GetScene()->AddPSO("default", descDefault);

    descDefault.opaque = false;
    descDefault.depthFunc = COMPARISON_FUNC::LESS_EQUAL;
    mEngine.GetScene()->AddPSO("pickedPSO", descDefault);
}

void PickingApp::AddTextures()
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

void PickingApp::AddMaterials()
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

    auto& highlightMat = mMaterials["highlightMat"];
    highlightMat.Name = "highlightMat";
    highlightMat.DiffuseMapIndex = 6;
    highlightMat.DiffuseAlbedo = FireFlame::Vector4f(1.0f, 1.0f, 0.0f, 0.6f);
    highlightMat.FresnelR0 = FireFlame::Vector3f(0.06f, 0.06f, 0.06f);
    highlightMat.Roughness = 0.0f;
    mEngine.GetScene()->AddMaterial
    (
        highlightMat.Name,
        mShaderDescs["main"].name, "",
        sizeof(MaterialConstants), &highlightMat
    );
}

void PickingApp::AddMeshs()
{
    //AddMeshSkull();
    AddMeshCar();
}

void PickingApp::AddMeshCar()
{
    using namespace DirectX;
    using namespace FireFlame;

    std::ifstream fin("../../Resources/Models/car.txt");
    if (!fin)
    {
        MessageBox(0, L"Models/car.txt not found.", 0, 0);
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
    XMFLOAT3 vMinf3(maxfloat, maxfloat, maxfloat);
    XMFLOAT3 vMaxf3(minfloat, minfloat, minfloat);

    XMVECTOR vMin = XMLoadFloat3(&vMinf3);
    XMVECTOR vMax = XMLoadFloat3(&vMaxf3);

    std::vector<FLVertexNormalTex> vertices(vcount);
    for (UINT i = 0; i < vcount; ++i)
    {
        fin >> vertices[i].Pos.x >> vertices[i].Pos.y >> vertices[i].Pos.z;
        fin >> vertices[i].Normal.x >> vertices[i].Normal.y >> vertices[i].Normal.z;

        XMVECTOR P = XMLoadFloat3((XMFLOAT3*)&vertices[i].Pos);

        vertices[i].Tex = { 0.0f, 0.0f };

        vMin = XMVectorMin(vMin, P);
        vMax = XMVectorMax(vMax, P);
    }

    BoundingBox boundsBox;
    XMStoreFloat3(&boundsBox.Center, 0.5f*(vMin + vMax));
    XMStoreFloat3(&boundsBox.Extents, 0.5f*(vMax - vMin));

    DirectX::BoundingSphere boundsSphere;
    XMStoreFloat3(&boundsSphere.Center, 0.5f*(vMin + vMax));

    // search for sphere radius
    auto sphereCenter = XMLoadFloat3(&boundsSphere.Center);
    DirectX::XMVECTOR maxLength = XMLoadFloat3(&XMFLOAT3(minfloat, minfloat, minfloat));
    for (UINT i = 0; i < vcount; ++i)
    {
        DirectX::XMFLOAT3 Pos(vertices[i].Pos.x, vertices[i].Pos.y, vertices[i].Pos.z);
        DirectX::XMVECTOR P = XMLoadFloat3(&Pos);
        auto length = XMVector3Length(P - sphereCenter);
        if (XMVector3Greater(length, maxLength)) // SIMD so about same as compare one value
        {
            maxLength = length;
        }
    }
    boundsSphere.Radius = XMVectorGetX(maxLength);
    // end

    fin >> ignore;
    fin >> ignore;
    fin >> ignore;

    std::vector<std::int32_t> indices(3 * tcount);
    for (UINT i = 0; i < tcount; ++i)
    {
        fin >> indices[i * 3 + 0] >> indices[i * 3 + 1] >> indices[i * 3 + 2];
    }

    fin.close();

    auto& meshDesc = mMeshDescs["car"];
    meshDesc.name = "car";
    meshDesc.indexCount = (unsigned int)indices.size();
    meshDesc.indexFormat = Index_Format::UINT32;
    meshDesc.indices = indices.data();

    meshDesc.vertexData.push_back(vertices.data());
    meshDesc.vertexDataCount.push_back((unsigned)vertices.size());
    meshDesc.vertexDataSize.push_back(sizeof(FLVertexNormalTex));

    // sub meshes
    //meshDesc.subMeshs.emplace_back("all", (UINT)indices.size(), boundsBox);
    meshDesc.subMeshs.emplace_back("all", (UINT)indices.size(), boundsSphere);
    meshDesc.subMeshs.emplace_back("pickedTriangle", (UINT)3, boundsBox);
    mEngine.GetScene()->AddPrimitive(meshDesc);
}

void PickingApp::AddMeshSkull()
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
    DirectX::BoundingBox boundsBox;
    DirectX::BoundingSphere boundsSphere;
    XMStoreFloat3(&boundsBox.Center, 0.5f*(vMin + vMax));
    XMStoreFloat3(&boundsBox.Extents, 0.5f*(vMax - vMin));
    XMStoreFloat3(&boundsSphere.Center, 0.5f*(vMin + vMax));

    // search for sphere radius
    auto sphereCenter = XMLoadFloat3(&boundsSphere.Center);
    DirectX::XMVECTOR maxLength = XMLoadFloat3(&XMFLOAT3(minfloat, minfloat, minfloat));
    for (UINT i = 0; i < vcount; ++i)
    {
        DirectX::XMFLOAT3 Pos(vertices[i].Pos.x, vertices[i].Pos.y, vertices[i].Pos.z);
        DirectX::XMVECTOR P = XMLoadFloat3(&Pos);
        auto length = XMVector3Length(P - sphereCenter);
        if (XMVector3Greater(length, maxLength)) // SIMD so about same as compare one value
        {
            maxLength = length;
        }
    }
    boundsSphere.Radius = XMVectorGetX(maxLength);
    // end

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
    meshDesc.subMeshs.emplace_back("all", (UINT)indices.size(), boundsBox);
    //meshDesc.subMeshs.emplace_back("all", (UINT)indices.size(), boundsSphere);
    mEngine.GetScene()->AddPrimitive(meshDesc);
}

void PickingApp::AddRenderItems()
{
    using namespace DirectX;

    std::string meshName = "car";
    //std::string meshName = "skull";
    FireFlame::stRenderItemDesc modelRitem("model1", mMeshDescs[meshName].subMeshs[0]);
    FireFlame::InstanceData instdata;
    instdata.worldTrans = XMFLOAT4X4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);
    XMStoreFloat4x4((DirectX::XMFLOAT4X4*)&instdata.texTransform, XMMatrixScaling(2.0f, 2.0f, 1.0f));
    instdata.materialIndex = 0;
    modelRitem.InstDatas.push_back(instdata);

    mRenderItems[modelRitem.name] = modelRitem;
    mEngine.GetScene()->AddRenderItem
    (
        mMeshDescs[meshName].name,
        mShaderDescs["main"].name,
        "default",
        modelRitem
    );

    FireFlame::stRenderItemDesc pickedRitem("pickedTriangle", mMeshDescs[meshName].subMeshs[1]);
    pickedRitem.opaque = false;
    instdata.worldTrans = XMFLOAT4X4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);
    XMStoreFloat4x4((DirectX::XMFLOAT4X4*)&instdata.texTransform, XMMatrixScaling(2.0f, 2.0f, 1.0f));
    instdata.materialIndex = (UINT)(mMaterials.size()-1);
    pickedRitem.InstDatas.push_back(instdata);

    //mRenderItems[pickedRitem.name] = pickedRitem;
    mPickedRenderItem = pickedRitem;
    mEngine.GetScene()->AddRenderItem
    (
        mMeshDescs[meshName].name,
        mShaderDescs["main"].name,
        "pickedPSO",
        pickedRitem
    );
}

void PickingApp::OnKeyUp(WPARAM wParam, LPARAM lParam)
{
    FLEngineApp4::OnKeyUp(wParam, lParam);
}

void PickingApp::OnMouseDown(WPARAM btnState, int x, int y)
{
    FLEngineApp4::OnMouseDown(btnState, x, y);
    if ((btnState & MK_RBUTTON) != 0)
    {
        Pick(x, y);
    }
}

void PickingApp::Pick(int sx, int sy)
{
    // Assume nothing is picked to start, so the picked render-item is invisible.
    mEngine.GetScene()->RenderItemVisible("pickedTriangle", false);

    std::string hitRenderItem;
    UINT hitTriangle = 0;
    if (mEngine.GetScene()->TestIntersect(sx, sy, {"model1"}, hitRenderItem, hitTriangle))
    {
        mEngine.GetScene()->RenderItemVisible("pickedTriangle", true);
        mEngine.GetScene()->RenderItemChangeSubmesh("pickedTriangle", 3, 3 * hitTriangle, 0);
    }
}