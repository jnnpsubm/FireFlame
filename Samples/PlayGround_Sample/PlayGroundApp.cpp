#include "PlayGroundApp.h"
#include <fstream>
#include <assert.h>

void PlayGroundApp::PreInitialize() {}

void PlayGroundApp::Initialize()
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

void PlayGroundApp::Update(float time_elapsed)
{
    FLEngineApp4::Update(time_elapsed);
}

void PlayGroundApp::UpdateMainPassCB(float time_elapsed)
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

    passCB.AmbientLight = { 0.25f, 0.25f, 0.35f, 1.0f };
    passCB.Lights[0].Direction = { 0.57735f, -0.57735f, 0.57735f };
    passCB.Lights[0].Strength = { 0.6f, 0.6f, 0.6f };
    passCB.Lights[1].Direction = { -0.57735f, -0.57735f, 0.57735f };
    passCB.Lights[1].Strength = { 0.3f, 0.3f, 0.3f };
    passCB.Lights[2].Direction = { 0.0f, -0.707f, -0.707f };
    passCB.Lights[2].Strength = { 0.15f, 0.15f, 0.15f };

    //mEngine.GetScene()->UpdateShaderPassCBData(mShaderDescs["main"].name, sizeof(PassConstantsLight), &passCB);
    mEngine.GetScene()->UpdateShaderPassCBData(mShaderDescs["sky"].name, sizeof(PassConstantsLight), &passCB);
    mEngine.GetScene()->UpdateShaderPassCBData(mShaderDescs["terrain"].name, sizeof(PassConstantsLight), &passCB);
}

void PlayGroundApp::AddShaders()
{
    AddShaderSky();
    AddShaderTerrain();
}

void PlayGroundApp::AddShaderSky()
{
    using namespace FireFlame;

    auto& shaderDesc = mShaderDescs["sky"];
    shaderDesc.name = "sky";
    shaderDesc.objCBSize = sizeof(ObjectConsts);
    shaderDesc.materialCBSize = sizeof(MaterialConstants);
    shaderDesc.passCBSize = sizeof(PassConstantsLight);
    shaderDesc.objParamIndex = 0;
    shaderDesc.passParamIndex = 1;
    shaderDesc.texSRVDescriptorTableSize = 7;
    shaderDesc.maxTexSRVDescriptor = 7;
    shaderDesc.useDynamicMat = true;

    std::wstring strHlslFile = L"Shaders\\Sky.hlsl";
    shaderDesc.AddVertexInput("POSITION", FireFlame::VERTEX_FORMAT_FLOAT3);
    shaderDesc.AddVertexInput("NORMAL", FireFlame::VERTEX_FORMAT_FLOAT3);
    shaderDesc.AddVertexInput("TEXCOORD", FireFlame::VERTEX_FORMAT_FLOAT2);
    shaderDesc.AddShaderStage(strHlslFile, Shader_Type::VS, "VS", "vs_5_1");
    shaderDesc.AddShaderStage(strHlslFile, Shader_Type::PS, "PS", "ps_5_1");

    mEngine.GetScene()->AddShader(shaderDesc);
}

void PlayGroundApp::AddShaderTerrain()
{
    using namespace FireFlame;

    auto& shaderDesc = mShaderDescs["terrain"];
    shaderDesc.name = "terrain";
    shaderDesc.objCBSize = sizeof(ObjectConsts);
    shaderDesc.passCBSize = sizeof(PassConstants);
    shaderDesc.materialCBSize = sizeof(MaterialConstants);
    shaderDesc.ParamDefault();

    shaderDesc.AddVertexInput("POSITION", FireFlame::VERTEX_FORMAT_FLOAT3);
    shaderDesc.AddVertexInput("TEXCOORD", FireFlame::VERTEX_FORMAT_FLOAT2);
    shaderDesc.AddShaderStage(L"Shaders\\terrain.hlsl", Shader_Type::VS, "VS", "vs_5_0");
    shaderDesc.AddShaderStage(L"Shaders\\terrain.hlsl", Shader_Type::HS, "HS", "hs_5_0");
    shaderDesc.AddShaderStage(L"Shaders\\terrain.hlsl", Shader_Type::DS, "DS", "ds_5_0");
    shaderDesc.AddShaderStage(L"Shaders\\terrain.hlsl", Shader_Type::PS, "PS", "ps_5_0");

    mEngine.GetScene()->AddShader(shaderDesc);
}

void PlayGroundApp::AddPSOs()
{
    using namespace FireFlame;

    //PSODesc descDefault(mShaderDescs["main"].name);
    //mEngine.GetScene()->AddPSO("default", descDefault);

    PSODesc descSky(mShaderDescs["sky"].name);
    descSky.depthFunc = FireFlame::COMPARISON_FUNC::LESS_EQUAL;
    descSky.cullMode = FireFlame::Cull_Mode::None;
    mEngine.GetScene()->AddPSO("skyPSO", descSky);

    PSODesc descTerrain(mShaderDescs["terrain"].name);
    descTerrain.topologyType = Primitive_Topology_Type::Patch;
    mEngine.GetScene()->AddPSO("terrainPSO", descTerrain);
}

void PlayGroundApp::AddTextures()
{
    std::vector<std::string> texNames =
    {
        "skyCubeMap"
    };

    std::vector<std::wstring> texFilenames =
    {
        //L"../../Resources/Textures/snowcube1024.dds"
        L"../../Resources/Textures/desertcube1024.dds"
    };

    for (int i = 0; i < (int)texNames.size(); ++i)
    {
        mEngine.GetScene()->AddTexture
        (
            texNames[i],
            texFilenames[i]
        );
    }

    mEngine.GetScene()->AddTextureGroup
    (
        "sky",
        {
            FireFlame::TEX(texNames[0],FireFlame::SRV_DIMENSION::TEXTURECUBE)
        }
    );
}

void PlayGroundApp::AddMaterials()
{
    auto& terrain = mMaterials["terrain"];
    terrain.Name = "terrain";
    terrain.DiffuseAlbedo = FireFlame::Vector4f(0.2f, 0.5f, 0.5f, 1.0f);
    terrain.FresnelR0 = FireFlame::Vector3f(0.35f, 0.75f, 0.75f);
    terrain.Roughness = 0.025f;
    mEngine.GetScene()->AddMaterial
    (
        terrain.Name,
        mShaderDescs["terrain"].name, "",
        sizeof(MaterialConstants), &terrain
    );
}

void PlayGroundApp::AddMeshs()
{
    AddMeshTerrain();
    AddMeshShapes();
    AddMeshSkull();
}

void PlayGroundApp::AddMeshTerrain()
{
    using namespace FireFlame;

    GeometryGenerator geoGen;
    //GeometryGenerator::MeshData grid = geoGen.CreateGridPatch(3000.0f, 3000.0f, 600+4, 600+4);
    GeometryGenerator::MeshData grid = geoGen.CreateGridPatch(30.0f, 15.0f, 4, 2+4);

    std::vector<FLVertexTex> vertices(grid.Vertices.size());
    for (size_t i = 0; i < grid.Vertices.size(); ++i)
    {
        vertices[i].Pos = grid.Vertices[i].Position;
        vertices[i].Pos.y -= 15.f;
        vertices[i].Pos.y += MathHelper::RandF() * 10.f;
        vertices[i].Tex = grid.Vertices[i].TexC;
    }

    std::vector<std::uint32_t> indices = grid.Indices32;

    auto& meshDesc = mMeshDescs["terrain"];
    meshDesc.name = "terrain";
    meshDesc.indexCount = (unsigned int)indices.size();
    meshDesc.indexFormat = Index_Format::UINT32;
    meshDesc.indices = indices.data();

    meshDesc.vertexData.push_back(vertices.data());
    meshDesc.vertexDataCount.push_back((unsigned)vertices.size());
    meshDesc.vertexDataSize.push_back(sizeof(FLVertexTex));

    // sub meshes
    meshDesc.subMeshs.emplace_back("all", (UINT)indices.size());
    mEngine.GetScene()->AddPrimitive(meshDesc);
}

void PlayGroundApp::AddMeshShapes()
{
    using namespace FireFlame;

    GeometryGenerator geoGen;
    GeometryGenerator::MeshData box = geoGen.CreateBox(1.0f, 1.0f, 1.0f, 3);
    GeometryGenerator::MeshData grid = geoGen.CreateGrid(20.0f, 30.0f, 60, 40);
    GeometryGenerator::MeshData sphere = geoGen.CreateSphere(0.5f, 20, 20);
    GeometryGenerator::MeshData cylinder = geoGen.CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20);

    UINT boxVertexOffset = 0;
    UINT gridVertexOffset = (UINT)box.Vertices.size();
    UINT sphereVertexOffset = gridVertexOffset + (UINT)grid.Vertices.size();
    UINT cylinderVertexOffset = sphereVertexOffset + (UINT)sphere.Vertices.size();

    UINT boxIndexOffset = 0;
    UINT gridIndexOffset = (UINT)box.Indices32.size();
    UINT sphereIndexOffset = gridIndexOffset + (UINT)grid.Indices32.size();
    UINT cylinderIndexOffset = sphereIndexOffset + (UINT)sphere.Indices32.size();

    auto totalVertexCount =
        box.Vertices.size() +
        grid.Vertices.size() +
        sphere.Vertices.size() +
        cylinder.Vertices.size();

    std::vector<FLVertexNormalTangentTex> vertices(totalVertexCount);

    UINT k = 0;
    for (size_t i = 0; i < box.Vertices.size(); ++i, ++k)
    {
        vertices[k].Pos = box.Vertices[i].Position;
        vertices[k].Normal = box.Vertices[i].Normal;
        vertices[k].Tangent = box.Vertices[i].TangentU;
        vertices[k].Tex = box.Vertices[i].TexC;
    }

    for (size_t i = 0; i < grid.Vertices.size(); ++i, ++k)
    {
        vertices[k].Pos = grid.Vertices[i].Position;
        vertices[k].Normal = grid.Vertices[i].Normal;
        vertices[k].Tangent = grid.Vertices[i].TangentU;
        vertices[k].Tex = grid.Vertices[i].TexC;
    }

    for (size_t i = 0; i < sphere.Vertices.size(); ++i, ++k)
    {
        vertices[k].Pos = sphere.Vertices[i].Position;
        vertices[k].Normal = sphere.Vertices[i].Normal;
        vertices[k].Tangent = sphere.Vertices[i].TangentU;
        vertices[k].Tex = sphere.Vertices[i].TexC;
    }

    for (size_t i = 0; i < cylinder.Vertices.size(); ++i, ++k)
    {
        vertices[k].Pos = cylinder.Vertices[i].Position;
        vertices[k].Normal = cylinder.Vertices[i].Normal;
        vertices[k].Tangent = cylinder.Vertices[i].TangentU;
        vertices[k].Tex = cylinder.Vertices[i].TexC;
    }

    std::vector<std::uint16_t> indices;
    indices.insert(indices.end(), std::begin(box.GetIndices16()), std::end(box.GetIndices16()));
    indices.insert(indices.end(), std::begin(grid.GetIndices16()), std::end(grid.GetIndices16()));
    indices.insert(indices.end(), std::begin(sphere.GetIndices16()), std::end(sphere.GetIndices16()));
    indices.insert(indices.end(), std::begin(cylinder.GetIndices16()), std::end(cylinder.GetIndices16()));

    auto& meshDesc = mMeshDescs["shapes"];
    meshDesc.name = "shapes";
    meshDesc.indexCount = (unsigned int)indices.size();
    meshDesc.indexFormat = Index_Format::UINT16;
    meshDesc.indices = indices.data();

    meshDesc.vertexData.push_back(vertices.data());
    meshDesc.vertexDataCount.push_back((unsigned)vertices.size());
    meshDesc.vertexDataSize.push_back(sizeof(FLVertexNormalTangentTex));

    // sub meshes
    meshDesc.subMeshs.emplace_back("box", (UINT)box.Indices32.size(), boxIndexOffset, boxVertexOffset);
    meshDesc.subMeshs.emplace_back("grid", (UINT)grid.Indices32.size(), gridIndexOffset, gridVertexOffset);
    meshDesc.subMeshs.emplace_back("sphere", (UINT)sphere.Indices32.size(), sphereIndexOffset, sphereVertexOffset);
    meshDesc.subMeshs.emplace_back("cylinder", (UINT)cylinder.Indices32.size(), cylinderIndexOffset, cylinderVertexOffset);
    mEngine.GetScene()->AddPrimitive(meshDesc);
}

void PlayGroundApp::AddMeshSkull()
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

void PlayGroundApp::AddRenderItems()
{
    AddRenderItemSky();
    AddRenderItemTerrain();
}

void PlayGroundApp::AddRenderItemSky()
{
    using namespace DirectX;

    FireFlame::stRenderItemDesc skyRitem("sky", mMeshDescs["shapes"].subMeshs[2]);
    ObjectConsts objConsts;
    //XMStoreFloat4x4(&objConsts.World, XMMatrixTranspose(XMMatrixScaling(5000.f, 5000.f, 5000.f)));
    XMStoreFloat4x4(&objConsts.World, XMMatrixTranspose(XMMatrixScaling(1.f, 1.f, 1.f)));
    XMStoreFloat4x4(&objConsts.TexTransform, XMMatrixTranspose(XMMatrixScaling(1.0f, 1.0f, 1.0f)));
    objConsts.MaterialIndex = 3;
    skyRitem.dataLen = sizeof(ObjectConsts);
    skyRitem.data = &objConsts;
    skyRitem.cullMode = FireFlame::Cull_Mode::None;
    mRenderItems[skyRitem.name] = skyRitem;
    mEngine.GetScene()->AddRenderItem
    (
        mMeshDescs["shapes"].name,
        mShaderDescs["sky"].name,
        "skyPSO",
        5,
        skyRitem
    );
}

void PlayGroundApp::AddRenderItemTerrain()
{
    using namespace DirectX;

    FireFlame::stRenderItemDesc RItem("land", mMeshDescs["terrain"].subMeshs[0]);
    RItem.topology = FireFlame::Primitive_Topology::CONTROL_POINT_PATCHLIST_16;
    XMFLOAT4X4 trans[1];
    XMStoreFloat4x4
    (
        &trans[0],
        XMMatrixTranspose(XMMatrixIdentity())
    );
    RItem.dataLen = sizeof(XMFLOAT4X4)*_countof(trans);
    RItem.data = &trans[0];
    RItem.mat = "terrain";
    mRenderItems[RItem.name] = RItem;
    mEngine.GetScene()->AddRenderItem
    (
        mMeshDescs["terrain"].name,
        mShaderDescs["terrain"].name,
        "terrainPSO",
        RItem
    );
}

void PlayGroundApp::OnKeyUp(WPARAM wParam, LPARAM lParam)
{
    FLEngineApp4::OnKeyUp(wParam, lParam);
}