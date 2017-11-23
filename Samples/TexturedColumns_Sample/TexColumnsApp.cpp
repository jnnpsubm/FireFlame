#include "TexColumnsApp.h"

void TexColumnsApp::Initialize()
{
    AddShaders();
    AddTextures();
    AddMaterials();
    AddGeoMeshs();
    AddRenderItems();

    mPasses.push_back("DefaultPass");
    mEngine.GetScene()->AddPass(mShaderDesc.name, mPasses[0]);
}

void TexColumnsApp::UpdateMainPassCB(float time_elapsed)
{
    using namespace FireFlame;
    float totalTime = mEngine.TotalTime();

    mMainPassCB.AmbientLight = { 0.25f, 0.25f, 0.35f, 1.0f };
    mMainPassCB.Lights[0].Direction = { 0.57735f, -0.57735f, 0.57735f };
    mMainPassCB.Lights[0].Strength = { 0.8f, 0.8f, 0.8f };
    mMainPassCB.Lights[1].Direction = { -0.57735f, -0.57735f, 0.57735f };
    mMainPassCB.Lights[1].Strength = { 0.4f, 0.4f, 0.4f };
    mMainPassCB.Lights[2].Direction = { 0.0f, -0.707f, -0.707f };
    mMainPassCB.Lights[2].Strength = { 0.2f, 0.2f, 0.2f };
}

void TexColumnsApp::AddShaders()
{
    using namespace FireFlame;
    mShaderDesc.name = "TexColApp";
    mShaderDesc.objCBSize = sizeof(ObjectConsts);
    mShaderDesc.passCBSize = sizeof(PassConstants);
    mShaderDesc.materialCBSize = sizeof(MaterialConstants);
    mShaderDesc.ParamDefault();
    mShaderDesc.AddVertexInput("POSITION", FireFlame::VERTEX_FORMAT_FLOAT3);
    mShaderDesc.AddVertexInput("NORMAL", FireFlame::VERTEX_FORMAT_FLOAT3);
    mShaderDesc.AddVertexInput("TEXCOORD", FireFlame::VERTEX_FORMAT_FLOAT2);
    mShaderDesc.AddShaderStage(L"Shaders\\TexColApp.hlsl", Shader_Type::VS, "VS", "vs_5_0");
    mShaderDesc.AddShaderStage(L"Shaders\\TexColApp.hlsl", Shader_Type::PS, "PS", "ps_5_0");

    mEngine.GetScene()->AddShader(mShaderDesc);
}

void TexColumnsApp::AddTextures()
{
    mEngine.GetScene()->AddTexture
    (
        "bricksTex",
        L"..\\..\\Resources\\Textures\\bricks.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "stoneTex",
        L"..\\..\\Resources\\Textures\\stone.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "tileTex",
        L"..\\..\\Resources\\Textures\\tile.dds"
    );
}

void TexColumnsApp::AddMaterials()
{
    auto& bricks0 = mMaterials["bricks0"];
    bricks0.name = "bricks0";
    bricks0.DiffuseAlbedo = FireFlame::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    bricks0.FresnelR0 = FireFlame::Vector3f(0.02f, 0.02f, 0.02f);
    bricks0.Roughness = 0.1f;
    mEngine.GetScene()->AddMaterial
    (
        bricks0.name,
        mShaderDesc.name, "bricksTex",
        sizeof(MaterialConstants), &bricks0
    );

    auto& stone0 = mMaterials["stone0"];
    stone0.name = "stone0";
    stone0.DiffuseAlbedo = FireFlame::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    stone0.FresnelR0 = FireFlame::Vector3f(0.05f, 0.05f, 0.05f);
    stone0.Roughness = 0.3f;
    mEngine.GetScene()->AddMaterial
    (
        stone0.name,
        mShaderDesc.name, "stoneTex",
        sizeof(MaterialConstants), &stone0
    );

    auto& tile0 = mMaterials["tile0"];
    tile0.name = "tile0";
    tile0.DiffuseAlbedo = FireFlame::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    tile0.FresnelR0 = FireFlame::Vector3f(0.02f, 0.02f, 0.02f);
    tile0.Roughness = 0.3f;
    mEngine.GetScene()->AddMaterial
    (
        tile0.name,
        mShaderDesc.name, "tileTex",
        sizeof(MaterialConstants), &tile0
    );
}

void TexColumnsApp::AddGeoMeshs()
{
    using namespace FireFlame;
   
    GeometryGenerator geoGen;
    GeometryGenerator::MeshData box = geoGen.CreateBox(1.f, 1.f, 1.f, 3);
    GeometryGenerator::MeshData grid = geoGen.CreateGrid(200.0f, 300.0f, 60, 40);
    GeometryGenerator::MeshData sphere = geoGen.CreateGeosphere(0.5f, 4);
    GeometryGenerator::MeshData cylinder = geoGen.CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20);

    // Cache the vertex offsets to each object in the concatenated vertex buffer.
    UINT boxVertexOffset = 0;
    UINT gridVertexOffset = (UINT)box.Vertices.size();
    UINT sphereVertexOffset = gridVertexOffset + (UINT)grid.Vertices.size();
    UINT cylinderVertexOffset = sphereVertexOffset + (UINT)sphere.Vertices.size();
    // Cache the starting index for each object in the concatenated index buffer.
    UINT boxIndexOffset = 0;
    UINT gridIndexOffset = (UINT)box.Indices32.size();
    UINT sphereIndexOffset = gridIndexOffset + (UINT)grid.Indices32.size();
    UINT cylinderIndexOffset = sphereIndexOffset + (UINT)sphere.Indices32.size();

    // put together vertices
    auto totalVertexCount =
        box.Vertices.size() +
        grid.Vertices.size() +
        sphere.Vertices.size() +
        cylinder.Vertices.size();
    std::vector<FireFlame::FLVertexNormalTex> vertices(totalVertexCount);
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

    std::vector<std::uint32_t> indices;
    indices.insert(indices.end(), std::begin(box.Indices32), std::end(box.Indices32));
    indices.insert(indices.end(), std::begin(grid.Indices32), std::end(grid.Indices32));
    indices.insert(indices.end(), std::begin(sphere.Indices32), std::end(sphere.Indices32));
    indices.insert(indices.end(), std::begin(cylinder.Indices32), std::end(cylinder.Indices32));

    mMeshDesc.emplace_back();
    mMeshDesc.back().name = "Shapes";
    mMeshDesc.back().primitiveTopology = Primitive_Topology::TriangleList;
    mMeshDesc.back().indexCount = (unsigned int)indices.size();
    mMeshDesc.back().indexFormat = Index_Format::UINT32;
    mMeshDesc.back().indices = indices.data();

    mMeshDesc.back().vertexDataCount.push_back((unsigned int)vertices.size());
    mMeshDesc.back().vertexDataSize.push_back(sizeof(FLVertexNormalTex));
    mMeshDesc.back().vertexData.push_back(vertices.data());

    // sub meshes
    mMeshDesc.back().subMeshs.emplace_back
    (
        "Box", (UINT)box.Indices32.size(), boxIndexOffset, boxVertexOffset
    );
    mMeshDesc[0].subMeshs.emplace_back
    (
        "Grid", (UINT)grid.Indices32.size(), gridIndexOffset, gridVertexOffset
    );
    mMeshDesc[0].subMeshs.emplace_back
    (
        "Sphere", (UINT)sphere.Indices32.size(), sphereIndexOffset, sphereVertexOffset
    );
    mMeshDesc[0].subMeshs.emplace_back
    (
        "Cylinder", (UINT)cylinder.Indices32.size(), cylinderIndexOffset, cylinderVertexOffset
    );
    mEngine.GetScene()->AddPrimitive(mMeshDesc.back());
}

void TexColumnsApp::AddRenderItems()
{
    using namespace DirectX;

    FireFlame::stRenderItemDesc RItem("Box1", mMeshDesc[0].subMeshs[0]);
    RItem.mat = "stone0";
    XMFLOAT4X4 trans[2];
    XMStoreFloat4x4
    (
        &trans[0],
        XMMatrixTranspose(XMMatrixScaling(2.0f, 2.0f, 2.0f)*XMMatrixTranslation(0.0f, 1.0f, 0.0f))
    );
    XMStoreFloat4x4
    (
        &trans[1],
        XMMatrixTranspose(XMMatrixScaling(1.0f, 1.0f, 1.0f))
    );
    RItem.dataLen = sizeof(XMFLOAT4X4)*_countof(trans);
    RItem.data = &trans[0];
    mRenderItems.emplace_back(RItem);
    mEngine.GetScene()->AddRenderItem
    (
        mMeshDesc[0].name,
        mShaderDesc.name,
        RItem
    );

    RItem.name = "Grid1";
    RItem.mat = "tile0";
    RItem.subMesh = mMeshDesc[0].subMeshs[1];
    XMStoreFloat4x4
    (
        &trans[0],
        XMMatrixTranspose(XMMatrixIdentity())
    );
    XMStoreFloat4x4
    (
        &trans[1],
        XMMatrixTranspose(XMMatrixScaling(80.f, 80.f, 1.0f))
    );
    mRenderItems.emplace_back(RItem);
    mEngine.GetScene()->AddRenderItem
    (
        mMeshDesc[0].name,
        mShaderDesc.name,
        RItem
    );

    XMStoreFloat4x4
    (
        &trans[1],
        XMMatrixTranspose(XMMatrixScaling(1.f, 1.f, 1.f))
    );
    for (int i = 0; i < 5; ++i)
    {
        XMMATRIX leftCylWorld = XMMatrixTranslation(-5.0f, 1.5f, -10.0f + i*5.0f);
        XMMATRIX rightCylWorld = XMMatrixTranslation(+5.0f, 1.5f, -10.0f + i*5.0f);
        XMMATRIX leftSphereWorld = XMMatrixTranslation(-5.0f, 3.5f, -10.0f + i*5.0f);
        XMMATRIX rightSphereWorld = XMMatrixTranslation(+5.0f, 3.5f, -10.0f + i*5.0f);

        RItem.mat = "bricks0";
        XMStoreFloat4x4(&trans[0], XMMatrixTranspose(rightCylWorld));
        RItem.name = "RightCyl" + std::to_string(i);
        RItem.subMesh = mMeshDesc[0].subMeshs[3];
        mRenderItems.emplace_back(RItem);
        mEngine.GetScene()->AddRenderItem
        (
            mMeshDesc[0].name,
            mShaderDesc.name,
            RItem
        );

        XMStoreFloat4x4(&trans[0], XMMatrixTranspose(leftCylWorld));
        RItem.name = "LeftCyl" + std::to_string(i);
        RItem.subMesh = mMeshDesc[0].subMeshs[3];
        mRenderItems.emplace_back(RItem);
        mEngine.GetScene()->AddRenderItem
        (
            mMeshDesc[0].name,
            mShaderDesc.name,
            RItem
        );

        RItem.mat = "stone0";
        XMStoreFloat4x4(&trans[0], XMMatrixTranspose(leftSphereWorld));
        RItem.name = "LeftSphere" + std::to_string(i);
        RItem.subMesh = mMeshDesc[0].subMeshs[2];
        mRenderItems.emplace_back(RItem);
        mEngine.GetScene()->AddRenderItem
        (
            mMeshDesc[0].name,
            mShaderDesc.name,
            RItem
        );

        XMStoreFloat4x4(&trans[0], XMMatrixTranspose(rightSphereWorld));
        RItem.name = "RightSphere" + std::to_string(i);
        RItem.subMesh = mMeshDesc[0].subMeshs[2];
        mRenderItems.emplace_back(RItem);
        mEngine.GetScene()->AddRenderItem
        (
            mMeshDesc[0].name,
            mShaderDesc.name,
            RItem
        );
    }
}

void TexColumnsApp::OnKeyUp(WPARAM wParam, LPARAM lParam)
{
    
}