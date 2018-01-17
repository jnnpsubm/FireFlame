#include "CubeMapApp.h"
#include <fstream>
#include <assert.h>

void CubeMapApp::PreInitialize() {}

void CubeMapApp::Initialize()
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

void CubeMapApp::Update(float time_elapsed)
{
    FLEngineApp4::Update(time_elapsed);
}

void CubeMapApp::UpdateMainPassCB(float time_elapsed)
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
}

void CubeMapApp::AddShaders()
{
    using namespace FireFlame;

    auto& shaderDesc = mShaderDescs["main"];
    shaderDesc.name = "main";
    shaderDesc.objCBSize = sizeof(ObjectConsts);
    shaderDesc.materialCBSize = sizeof(MaterialConstants);
    shaderDesc.passCBSize = sizeof(PassConstantsLight);
    shaderDesc.objParamIndex = 0;
    shaderDesc.passParamIndex = 1;
    shaderDesc.texSRVDescriptorTableSize = 4;
    shaderDesc.useDynamicMat = true;

    std::wstring strHlslFile = L"Shaders\\Main.hlsl";
    shaderDesc.AddVertexInput("POSITION", FireFlame::VERTEX_FORMAT_FLOAT3);
    shaderDesc.AddVertexInput("NORMAL", FireFlame::VERTEX_FORMAT_FLOAT3);
    shaderDesc.AddVertexInput("TEXCOORD", FireFlame::VERTEX_FORMAT_FLOAT2);
    shaderDesc.AddShaderStage(strHlslFile, Shader_Type::VS, "VS", "vs_5_1");
    shaderDesc.AddShaderStage(strHlslFile, Shader_Type::PS, "PS", "ps_5_1");

    mEngine.GetScene()->AddShader(shaderDesc);
}

void CubeMapApp::AddPSOs()
{
    using namespace FireFlame;

    PSODesc descDefault(mShaderDescs["main"].name);
    mEngine.GetScene()->AddPSO("default", descDefault);
}

void CubeMapApp::AddTextures()
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
        L"../../Resources/Textures/grasscube1024.dds"
    );
    mEngine.GetScene()->AddTextureGroup
    (
        "main",
        { 
            FireFlame::TEX("bricksDiffuseMap"),
            FireFlame::TEX("tileDiffuseMap"),
            FireFlame::TEX("defaultDiffuseMap"),
            FireFlame::TEX("skyCubeMap")
        }
    );
}

void CubeMapApp::AddMaterials()
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
}

void CubeMapApp::AddMeshs()
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

void CubeMapApp::AddRenderItems()
{
    using namespace DirectX;

    FireFlame::stRenderItemDesc boxRitem("box", mMeshDescs["shapes"].subMeshs[0]);
    ObjectConsts objConsts;
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

void CubeMapApp::OnKeyUp(WPARAM wParam, LPARAM lParam)
{
    FLEngineApp4::OnKeyUp(wParam, lParam);
}