#include "CubeMapDynamicApp.h"
#include <fstream>
#include <assert.h>

void CubeMapDynamicApp::PreInitialize() {}

void CubeMapDynamicApp::Initialize()
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

void CubeMapDynamicApp::Update(float time_elapsed)
{
    FLEngineApp4::Update(time_elapsed);

    using namespace DirectX;
    XMMATRIX skullScale = XMMatrixScaling(0.2f, 0.2f, 0.2f);
    XMMATRIX skullOffset = XMMatrixTranslation(3.0f, 2.0f, 0.0f);
    XMMATRIX skullLocalRotate = XMMatrixRotationY(2.0f*mEngine.TotalTime());
    XMMATRIX skullGlobalRotate = XMMatrixRotationY(0.5f*mEngine.TotalTime());

    ObjectConsts objConsts;
    XMStoreFloat4x4(&objConsts.World, XMMatrixTranspose(skullScale*skullLocalRotate*skullOffset*skullGlobalRotate));
    XMStoreFloat4x4(&objConsts.TexTransform, XMMatrixIdentity());
    objConsts.MaterialIndex = 3;
    mEngine.GetScene()->UpdateRenderItemCBData("skull", sizeof(ObjectConsts), &objConsts);
}

void CubeMapDynamicApp::UpdateMainPassCB(float time_elapsed)
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

    mEngine.GetScene()->UpdateShaderPassCBData(mShaderDescs["main"].name, sizeof(PassConstantsLight), &passCB);
    mEngine.GetScene()->UpdateShaderPassCBData(mShaderDescs["sky"].name, sizeof(PassConstantsLight), &passCB);
}

void CubeMapDynamicApp::AddShaders()
{
    AddShaderMain();
    AddShaderSky();
}

void CubeMapDynamicApp::AddShaderMain()
{
    using namespace FireFlame;

    auto& shaderDesc = mShaderDescs["main"];
    shaderDesc.name = "main";
    shaderDesc.objCBSize = sizeof(ObjectConsts);
    shaderDesc.materialCBSize = sizeof(MaterialConstants);
    shaderDesc.passCBSize = sizeof(PassConstantsLight);
    shaderDesc.objParamIndex = 0;
    shaderDesc.passParamIndex = 1;
    shaderDesc.texSRVDescriptorTableSize = 6;
    shaderDesc.maxTexSRVDescriptor = 6;
    shaderDesc.useDynamicMat = true;
    shaderDesc.dynamicMatRegister = 6;
    shaderDesc.dynamicMatSpace = 0;

    std::wstring strHlslFile = L"Shaders\\Main.hlsl";
    shaderDesc.AddVertexInput("POSITION", FireFlame::VERTEX_FORMAT_FLOAT3);
    shaderDesc.AddVertexInput("NORMAL", FireFlame::VERTEX_FORMAT_FLOAT3);
    shaderDesc.AddVertexInput("TEXCOORD", FireFlame::VERTEX_FORMAT_FLOAT2);
    shaderDesc.AddShaderStage(strHlslFile, Shader_Type::VS, "VS", "vs_5_1");
    shaderDesc.AddShaderStage(strHlslFile, Shader_Type::PS, "PS", "ps_5_1");

    mEngine.GetScene()->AddShader(shaderDesc);
}

void CubeMapDynamicApp::AddShaderSky()
{
    using namespace FireFlame;

    auto& shaderDesc = mShaderDescs["sky"];
    shaderDesc.name = "sky";
    shaderDesc.objCBSize = sizeof(ObjectConsts);
    shaderDesc.materialCBSize = sizeof(MaterialConstants);
    shaderDesc.passCBSize = sizeof(PassConstantsLight);
    shaderDesc.objParamIndex = 0;
    shaderDesc.passParamIndex = 1;
    shaderDesc.texSRVDescriptorTableSize = 6;
    shaderDesc.maxTexSRVDescriptor = 6;
    shaderDesc.useDynamicMat = true;
    shaderDesc.dynamicMatRegister = 6;
    shaderDesc.dynamicMatSpace = 0;

    std::wstring strHlslFile = L"Shaders\\Sky.hlsl";
    shaderDesc.AddVertexInput("POSITION", FireFlame::VERTEX_FORMAT_FLOAT3);
    shaderDesc.AddVertexInput("NORMAL", FireFlame::VERTEX_FORMAT_FLOAT3);
    shaderDesc.AddVertexInput("TEXCOORD", FireFlame::VERTEX_FORMAT_FLOAT2);
    shaderDesc.AddShaderStage(strHlslFile, Shader_Type::VS, "VS", "vs_5_1");
    shaderDesc.AddShaderStage(strHlslFile, Shader_Type::PS, "PS", "ps_5_1");

    mEngine.GetScene()->AddShader(shaderDesc);
}

void CubeMapDynamicApp::AddPSOs()
{
    using namespace FireFlame;

    PSODesc descDefault(mShaderDescs["main"].name);
    mEngine.GetScene()->AddPSO("default", descDefault);

    //descDefault.shaderName = mShaderDescs["sky"].name;
    PSODesc descSky(mShaderDescs["sky"].name);
    descSky.depthFunc = FireFlame::COMPARISON_FUNC::LESS_EQUAL;
    descSky.cullMode = FireFlame::Cull_Mode::None;
    mEngine.GetScene()->AddPSO("skyPSO", descSky);
}

void CubeMapDynamicApp::AddTextures()
{
    mEngine.GetScene()->AddTexture
    (
        "bricksDiffuseMap",
        L"../../Resources/Textures/bricks2.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "tileDiffuseMap",
        L"../../Resources/Textures/tile.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "defaultDiffuseMap",
        L"../../Resources/Textures/white1x1.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "skyCubeMap",
        //L"../../Resources/Textures/grasscube1024.dds"
        L"../../Resources/terrain/desertcube1024.dds"
        //L"../../Resources/terrain/sunsetcube1024.dds"
    );
    mEngine.GetScene()->AddTextureDynamicCubeMap
    (
        "dynamicCubeMap", 1024, 1024
    );
    mEngine.GetScene()->AddTextureGroup
    (
        "main",
        { 
            FireFlame::TEX("bricksDiffuseMap"),
            FireFlame::TEX("tileDiffuseMap"),
            FireFlame::TEX("defaultDiffuseMap"),
            FireFlame::TEX("skyCubeMap",FireFlame::SRV_DIMENSION::TEXTURECUBE),
            FireFlame::TEX("skyCubeMap",FireFlame::SRV_DIMENSION::TEXTURECUBE),
            FireFlame::TEX("dynamicCubeMap",FireFlame::SRV_DIMENSION::TEXTURECUBE)
        }
    );

    mEngine.GetScene()->AddTextureGroup
    (
        "sky",
        {
            FireFlame::TEX("bricksDiffuseMap"),
            FireFlame::TEX("tileDiffuseMap"),
            FireFlame::TEX("defaultDiffuseMap"),
            FireFlame::TEX("skyCubeMap",FireFlame::SRV_DIMENSION::TEXTURECUBE),
            FireFlame::TEX("skyCubeMap",FireFlame::SRV_DIMENSION::TEXTURECUBE),
            FireFlame::TEX("skyCubeMap",FireFlame::SRV_DIMENSION::TEXTURECUBE)
        }
    );
}

void CubeMapDynamicApp::AddMaterials()
{
    auto& bricks0 = mMaterials["bricks0"];
    bricks0.Name = "bricks0";
    bricks0.DiffuseMapIndex = 0;
    bricks0.DiffuseAlbedo = FireFlame::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    bricks0.FresnelR0 = FireFlame::Vector3f(0.1f, 0.1f, 0.1f);
    bricks0.Roughness = 0.3f;
    mEngine.GetScene()->AddMaterial
    (
        bricks0.Name,
        mShaderDescs["main"].name, "",
        sizeof(MaterialConstants), &bricks0
    );

    auto& tile0 = mMaterials["tile0"];
    tile0.Name = "tile0";
    tile0.DiffuseMapIndex = 1;
    tile0.DiffuseAlbedo = FireFlame::Vector4f(0.9f, 0.9f, 0.9f, 1.0f);
    tile0.FresnelR0 = FireFlame::Vector3f(0.2f, 0.2f, 0.2f);
    tile0.Roughness = 0.1f;
    mEngine.GetScene()->AddMaterial
    (
        tile0.Name,
        mShaderDescs["main"].name, "",
        sizeof(MaterialConstants), &tile0
    );

    auto& mirror0 = mMaterials["mirror0"];
    mirror0.Name = "mirror0";
    mirror0.DiffuseMapIndex = 2;
    mirror0.DiffuseAlbedo = FireFlame::Vector4f(0.0f, 0.0f, 0.1f, 1.0f);
    mirror0.FresnelR0 = FireFlame::Vector3f(0.98f, 0.97f, 0.95f);
    mirror0.Roughness = 0.1f;
    mEngine.GetScene()->AddMaterial
    (
        mirror0.Name,
        mShaderDescs["main"].name, "",
        sizeof(MaterialConstants), &mirror0
    );

    auto& skullMat = mMaterials["skullMat"];
    skullMat.Name = "skullMat";
    skullMat.DiffuseMapIndex = 2;
    skullMat.DiffuseAlbedo = FireFlame::Vector4f(0.8f, 0.8f, 0.8f, 1.0f);
    skullMat.FresnelR0 = FireFlame::Vector3f(0.2f, 0.2f, 0.2f);
    skullMat.Roughness = 0.2f;
    mEngine.GetScene()->AddMaterial
    (
        skullMat.Name,
        mShaderDescs["main"].name, "",
        sizeof(MaterialConstants), &skullMat
    );

    auto& sky = mMaterials["sky"];
    sky.Name = "sky";
    sky.DiffuseMapIndex = 3;
    sky.DiffuseAlbedo = FireFlame::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    sky.FresnelR0 = FireFlame::Vector3f(0.1f, 0.1f, 0.1f);
    sky.Roughness = 1.0f;
    mEngine.GetScene()->AddMaterial
    (
        sky.Name,
        mShaderDescs["main"].name, "",
        sizeof(MaterialConstants), &sky
    );

    auto& mirror1 = mMaterials["mirror1"];
    mirror1.Name = "mirror1";
    mirror1.DiffuseMapIndex = 2;
    mirror1.CubeMapIndex = 1;
    mirror1.DiffuseAlbedo = FireFlame::Vector4f(0.0f, 0.0f, 0.1f, 1.0f);
    mirror1.FresnelR0 = FireFlame::Vector3f(0.98f, 0.97f, 0.95f);
    mirror1.Roughness = 0.1f;
    mEngine.GetScene()->AddMaterial
    (
        mirror1.Name,
        mShaderDescs["main"].name, "",
        sizeof(MaterialConstants), &mirror1
    );
}

void CubeMapDynamicApp::AddMeshs()
{
    AddMeshShapes();
    AddMeshSkull();
}

void CubeMapDynamicApp::AddMeshShapes()
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

    std::vector<FLVertexNormalTex> vertices(totalVertexCount);

    UINT k = 0;
    for (size_t i = 0; i < box.Vertices.size(); ++i, ++k)
    {
        vertices[k].Pos = box.Vertices[i].Position;
        vertices[k].Normal = box.Vertices[i].Normal;
        vertices[k].Tex = box.Vertices[i].TexC;
    }

    for (size_t i = 0; i < grid.Vertices.size(); ++i, ++k)
    {
        vertices[k].Pos = grid.Vertices[i].Position;
        vertices[k].Normal = grid.Vertices[i].Normal;
        vertices[k].Tex = grid.Vertices[i].TexC;
    }

    for (size_t i = 0; i < sphere.Vertices.size(); ++i, ++k)
    {
        vertices[k].Pos = sphere.Vertices[i].Position;
        vertices[k].Normal = sphere.Vertices[i].Normal;
        vertices[k].Tex = sphere.Vertices[i].TexC;
    }

    for (size_t i = 0; i < cylinder.Vertices.size(); ++i, ++k)
    {
        vertices[k].Pos = cylinder.Vertices[i].Position;
        vertices[k].Normal = cylinder.Vertices[i].Normal;
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
    meshDesc.vertexDataSize.push_back(sizeof(FLVertexNormalTex));

    // sub meshes
    meshDesc.subMeshs.emplace_back("box", (UINT)box.Indices32.size(), boxIndexOffset, boxVertexOffset);
    meshDesc.subMeshs.emplace_back("grid", (UINT)grid.Indices32.size(), gridIndexOffset, gridVertexOffset);
    meshDesc.subMeshs.emplace_back("sphere", (UINT)sphere.Indices32.size(), sphereIndexOffset, sphereVertexOffset);
    meshDesc.subMeshs.emplace_back("cylinder", (UINT)cylinder.Indices32.size(), cylinderIndexOffset, cylinderVertexOffset);
    mEngine.GetScene()->AddPrimitive(meshDesc);
}

void CubeMapDynamicApp::AddMeshSkull()
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

void CubeMapDynamicApp::AddRenderItems()
{
    using namespace DirectX;

    FireFlame::stRenderItemDesc skyRitem("sky", mMeshDescs["shapes"].subMeshs[2]);
    ObjectConsts objConsts;
    //XMStoreFloat4x4(&objConsts.World, XMMatrixTranspose(XMMatrixScaling(5000.f, 5000.f, 5000.f)));
    XMStoreFloat4x4(&objConsts.World, XMMatrixTranspose(XMMatrixScaling(1.f, 1.f, 1.f)));
    XMStoreFloat4x4(&objConsts.TexTransform, XMMatrixTranspose(XMMatrixScaling(1.0f, 1.0f, 1.0f)));
    objConsts.MaterialIndex = 4;
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

    FireFlame::stRenderItemDesc skullRitem("skull", mMeshDescs["skull"].subMeshs[0]);
    XMStoreFloat4x4(&objConsts.World, XMMatrixIdentity());
    XMStoreFloat4x4(&objConsts.TexTransform, XMMatrixIdentity());
    objConsts.MaterialIndex = 3;
    skullRitem.dataLen = sizeof(ObjectConsts);
    skullRitem.data = &objConsts;
    mRenderItems[skullRitem.name] = skullRitem;
    mEngine.GetScene()->AddRenderItem
    (
        mMeshDescs["skull"].name,
        mShaderDescs["main"].name,
        "default",
        skullRitem
    );

    FireFlame::stRenderItemDesc globeRitem("globe", mMeshDescs["shapes"].subMeshs[2]);
    XMStoreFloat4x4(&objConsts.World, XMMatrixTranspose(XMMatrixScaling(2.f, 2.f, 2.f)*XMMatrixTranslation(0.0f, 2.0f, 0.0f)));
    XMStoreFloat4x4(&objConsts.TexTransform, XMMatrixTranspose(XMMatrixScaling(1.0f, 1.0f, 1.0f)));
    objConsts.MaterialIndex = 5;
    globeRitem.dataLen = sizeof(ObjectConsts);
    globeRitem.data = &objConsts;
    mRenderItems[globeRitem.name] = globeRitem;
    mEngine.GetScene()->AddRenderItem
    (
        mMeshDescs["shapes"].name,
        mShaderDescs["main"].name,
        "default",
        globeRitem
    );

    FireFlame::stRenderItemDesc boxRitem("box", mMeshDescs["shapes"].subMeshs[0]);
    XMStoreFloat4x4(&objConsts.World, XMMatrixTranspose(XMMatrixScaling(2.0f, 1.0f, 2.0f)*XMMatrixTranslation(0.0f, 0.5f, 0.0f)));
    XMStoreFloat4x4(&objConsts.TexTransform, XMMatrixTranspose(XMMatrixScaling(1.0f, 1.0f, 1.0f)));
    objConsts.MaterialIndex = 0;
    boxRitem.dataLen = sizeof(ObjectConsts);
    boxRitem.data = &objConsts;
    mRenderItems[boxRitem.name] = boxRitem;
    mEngine.GetScene()->AddRenderItem
    (
        mMeshDescs["shapes"].name,
        mShaderDescs["main"].name,
        "default",
        boxRitem
    );

    FireFlame::stRenderItemDesc gridRitem("grid", mMeshDescs["shapes"].subMeshs[1]);
    objConsts.World = FireFlame::Matrix4X4();
    XMStoreFloat4x4(&objConsts.TexTransform, XMMatrixTranspose(XMMatrixScaling(8.0f, 8.0f, 1.0f)));
    objConsts.MaterialIndex = 1;
    gridRitem.dataLen = sizeof(ObjectConsts);
    gridRitem.data = &objConsts;
    mRenderItems[gridRitem.name] = gridRitem;
    mEngine.GetScene()->AddRenderItem
    (
        mMeshDescs["shapes"].name,
        mShaderDescs["main"].name,
        "default",
        gridRitem
    );

    XMMATRIX brickTexTransform = XMMatrixScaling(1.0f, 1.0f, 1.0f);
    for (int i = 0; i < 5; ++i)
    {
        using FireFlame::stRenderItemDesc;
        stRenderItemDesc leftCylRitem(std::string("leftCyl_")+std::to_string(i), mMeshDescs["shapes"].subMeshs[3]);
        stRenderItemDesc rightCylRitem(std::string("rightCyl_") + std::to_string(i), mMeshDescs["shapes"].subMeshs[3]);
        stRenderItemDesc leftSphereRitem(std::string("leftSphere_") + std::to_string(i), mMeshDescs["shapes"].subMeshs[2]);
        stRenderItemDesc rightSphereRitem(std::string("rightSphere_") + std::to_string(i), mMeshDescs["shapes"].subMeshs[2]);

        XMMATRIX leftCylWorld = XMMatrixTranslation(-5.0f, 1.5f, -10.0f + i * 5.0f);
        XMMATRIX rightCylWorld = XMMatrixTranslation(+5.0f, 1.5f, -10.0f + i * 5.0f);

        XMMATRIX leftSphereWorld = XMMatrixTranslation(-5.0f, 3.5f, -10.0f + i * 5.0f);
        XMMATRIX rightSphereWorld = XMMatrixTranslation(+5.0f, 3.5f, -10.0f + i * 5.0f);

        XMStoreFloat4x4(&objConsts.World, XMMatrixTranspose(rightCylWorld));
        XMStoreFloat4x4(&objConsts.TexTransform, XMMatrixTranspose(brickTexTransform));
        objConsts.MaterialIndex = 0;
        leftCylRitem.dataLen = sizeof(ObjectConsts);
        leftCylRitem.data = &objConsts;
        mRenderItems[leftCylRitem.name] = leftCylRitem;
        mEngine.GetScene()->AddRenderItem
        (
            mMeshDescs["shapes"].name,
            mShaderDescs["main"].name,
            "default",
            leftCylRitem
        );

        XMStoreFloat4x4(&objConsts.World, XMMatrixTranspose(leftCylWorld));
        XMStoreFloat4x4(&objConsts.TexTransform, XMMatrixTranspose(brickTexTransform));
        objConsts.MaterialIndex = 0;
        rightCylRitem.dataLen = sizeof(ObjectConsts);
        rightCylRitem.data = &objConsts;
        mRenderItems[rightCylRitem.name] = rightCylRitem;
        mEngine.GetScene()->AddRenderItem
        (
            mMeshDescs["shapes"].name,
            mShaderDescs["main"].name,
            "default",
            rightCylRitem
        );

        XMStoreFloat4x4(&objConsts.World, XMMatrixTranspose(leftSphereWorld));
        objConsts.TexTransform = FireFlame::Matrix4X4();
        objConsts.MaterialIndex = 2;
        leftSphereRitem.dataLen = sizeof(ObjectConsts);
        leftSphereRitem.data = &objConsts;
        mRenderItems[leftSphereRitem.name] = leftSphereRitem;
        mEngine.GetScene()->AddRenderItem
        (
            mMeshDescs["shapes"].name,
            mShaderDescs["main"].name,
            "default",
            leftSphereRitem
        );

        XMStoreFloat4x4(&objConsts.World, XMMatrixTranspose(rightSphereWorld));
        objConsts.TexTransform = FireFlame::Matrix4X4();
        objConsts.MaterialIndex = 2;
        rightSphereRitem.dataLen = sizeof(ObjectConsts);
        rightSphereRitem.data = &objConsts;
        mRenderItems[rightSphereRitem.name] = rightSphereRitem;
        mEngine.GetScene()->AddRenderItem
        (
            mMeshDescs["shapes"].name,
            mShaderDescs["main"].name,
            "default",
            rightSphereRitem
        );
    }
}

void CubeMapDynamicApp::OnKeyUp(WPARAM wParam, LPARAM lParam)
{
    FLEngineApp4::OnKeyUp(wParam, lParam);
}