#include "ShapesApp.h"
#include <fstream>

void ShapesApp::Initialize() 
{
    BuildShaders();
    AddMaterials();
    BuildMesh();
    BuildRenderItems();

    mPasses.push_back("DefaultPass");
    mEngine.GetScene()->AddPass(mShaderDesc.name, mPasses[0]);
}

void ShapesApp::UpdateMainPassCB(float time_elapsed)
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

    // test point lights
    static std::default_random_engine RNG;
    static std::uniform_real_distribution<float> DIST(0.1f, 0.6f);
    float red = std::sinf(totalTime);
    red = (red + 1.0f) / 2.0f;
    red = MathHelper::Clamp(red, 0.2f, 1.0f);
    float green = std::sinf(totalTime);
    green = (green + 1.0f) / 2.0f;
    green = MathHelper::Clamp(green, 0.2f, 1.0f);
    float blue = std::sinf(totalTime + DIST(RNG));
    blue = (blue + 1.0f) / 2.0f;
    blue = MathHelper::Clamp(blue, 0.2f, 1.0f);
    for (size_t i = 0; i < 4; ++i)
    {
        mMainPassCB.Lights[i].Strength = { 0.f,0.f,0.f };
    }
    const size_t dirLightsCount = 0;
    for (size_t i = 0; i < 10; ++i)
    {
        mMainPassCB.Lights[dirLightsCount + i].FalloffEnd = 100.f;
        mMainPassCB.Lights[dirLightsCount + i].FalloffStart = 1.f;
        mMainPassCB.Lights[dirLightsCount + i].Position = mSpherePos[i];
        //mMainPassCB.Lights[4 + i].Strength = { red,green,blue };
        mMainPassCB.Lights[dirLightsCount + i].Strength = { 0.4f,0.4f,0.4f };
    }
}

void ShapesApp::BuildShaders()
{
    using namespace FireFlame;
    mShaderDesc.name = "ShapesApp";
    mShaderDesc.objCBSize = sizeof(ObjectConsts);
    mShaderDesc.passCBSize = sizeof(PassConstants);
    mShaderDesc.materialCBSize = sizeof(MaterialConstants);
    mShaderDesc.materialRegister = 1;
    mShaderDesc.passRegister = 2;
    mShaderDesc.AddVertexInput("POSITION", FireFlame::VERTEX_FORMAT_FLOAT3);
    mShaderDesc.AddVertexInput("NORMAL", FireFlame::VERTEX_FORMAT_FLOAT3);
    mShaderDesc.AddShaderStage(L"Shaders\\ShapesShader.hlsl", Shader_Type::VS, "VS", "vs_5_0");
    mShaderDesc.AddShaderStage(L"Shaders\\ShapesShader.hlsl", Shader_Type::PS, "PS", "ps_5_0");

    mEngine.GetScene()->AddShader(mShaderDesc);
}

void ShapesApp::AddMaterials()
{
    auto& skull = mMaterials["skull"];
    skull.name = "skull";
    skull.DiffuseAlbedo = { 0.2f, 0.2f, 0.2f, 1.0f };
    skull.FresnelR0 = { 0.6f,0.6f,0.6f };
    skull.Roughness = 0.3f;
    mEngine.GetScene()->AddMaterial("skull", mShaderDesc.name, sizeof(MaterialConstants), &skull);

    auto& box = mMaterials["box"];
    box.name = "box";
    box.DiffuseAlbedo = { 0.3f, 0.3f, 0.4f, 1.0f };
    box.FresnelR0 = { 0.3f,0.3f,0.3f };
    box.Roughness = 0.3f;
    mEngine.GetScene()->AddMaterial("box", mShaderDesc.name, sizeof(MaterialConstants), &box);

    auto& sphere = mMaterials["sphere"];
    sphere.name = "sphere";
    sphere.DiffuseAlbedo = { 0.3f, 0.3f, 0.4f, 1.0f };
    sphere.FresnelR0 = { 0.5f,0.2f,0.2f };
    sphere.Roughness = 0.2f;
    mEngine.GetScene()->AddMaterial("sphere", mShaderDesc.name, sizeof(MaterialConstants), &sphere);

    auto& cyl = mMaterials["cyl"];
    cyl.name = "cyl";
    cyl.DiffuseAlbedo = { 0.3f, 0.3f, 0.1f, 1.0f };
    cyl.FresnelR0 = { 0.5f,0.8f,0.8f };
    cyl.Roughness = 0.1f;
    mEngine.GetScene()->AddMaterial("cyl", mShaderDesc.name, sizeof(MaterialConstants), &cyl);
}

void ShapesApp::BuildMesh()
{
    using namespace FireFlame;
    GeometryGenerator geoGen;
    GeometryGenerator::MeshData box = geoGen.CreateBox(1.5f, 0.5f, 1.5f, 3);
    GeometryGenerator::MeshData grid = geoGen.CreateGrid(200.0f, 300.0f, 60, 40);
    //GeometryGenerator::MeshData sphere = geoGen.CreateSphere(0.5f, 20, 20);
    //GeometryGenerator::MeshData sphere = geoGen.CreateGeosphere(0.5f, 0);
    //GeometryGenerator::MeshData sphere = geoGen.CreateGeosphere(0.5f, 1);
    //GeometryGenerator::MeshData sphere = geoGen.CreateGeosphere(0.5f, 2);
    GeometryGenerator::MeshData sphere = geoGen.CreateGeosphere(0.5f, 3);
    //GeometryGenerator::MeshData sphere = geoGen.CreateGeosphere(0.5f, 4);
    //GeometryGenerator::MeshData sphere = geoGen.CreateGeosphere(0.5f, 5);
    //GeometryGenerator::MeshData sphere = geoGen.CreateGeosphere(0.5f, 6);
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
    std::vector<FireFlame::FLVertexNormal> vertices(totalVertexCount);
    UINT k = 0;
    for (size_t i = 0; i < box.Vertices.size(); ++i, ++k)
    {
        vertices[k].Pos = box.Vertices[i].Position;
        vertices[k].Normal = box.Vertices[i].Normal;
    }
    for (size_t i = 0; i < grid.Vertices.size(); ++i, ++k)
    {
        vertices[k].Pos = grid.Vertices[i].Position;
        vertices[k].Normal = grid.Vertices[i].Normal;
    }
    for (size_t i = 0; i < sphere.Vertices.size(); ++i, ++k)
    {
        vertices[k].Pos = sphere.Vertices[i].Position;
        vertices[k].Normal = sphere.Vertices[i].Normal;
    }
    for (size_t i = 0; i < cylinder.Vertices.size(); ++i, ++k)
    {
        vertices[k].Pos = cylinder.Vertices[i].Position;
        vertices[k].Normal = cylinder.Vertices[i].Normal;
    }

    std::vector<std::uint16_t> indices;
    indices.insert(indices.end(), std::begin(box.GetIndices16()), std::end(box.GetIndices16()));
    indices.insert(indices.end(), std::begin(grid.GetIndices16()), std::end(grid.GetIndices16()));
    indices.insert(indices.end(), std::begin(sphere.GetIndices16()), std::end(sphere.GetIndices16()));
    indices.insert(indices.end(), std::begin(cylinder.GetIndices16()), std::end(cylinder.GetIndices16()));

    mMeshDesc.emplace_back();
    mMeshDesc[0].name = "Shapes";
    mMeshDesc[0].primitiveTopology = Primitive_Topology::TriangleList;
    mMeshDesc[0].indexCount = (unsigned int)indices.size();
    mMeshDesc[0].indexFormat = Index_Format::UINT16;
    mMeshDesc[0].indices = indices.data();

    mMeshDesc[0].vertexDataCount.push_back((unsigned int)vertices.size());
    mMeshDesc[0].vertexDataSize.push_back(sizeof(FLVertexNormal));
    mMeshDesc[0].vertexData.push_back(vertices.data());

    // sub meshes
    mMeshDesc[0].subMeshs.emplace_back
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
    mEngine.GetScene()->AddPrimitive(mMeshDesc[0]);

    // Load Skull
    LoadSkull("Resources\\skull.txt");
}

void ShapesApp::LoadSkull(const std::string& filePath)
{
    std::ifstream inFile(filePath);
    std::string nouse;
    size_t vertexCount = 0;
    size_t indexCount = 0;
    inFile >> nouse;             // VertexCount:
    inFile >> vertexCount;
    inFile >> nouse;             // TriangleCount:
    inFile >> indexCount; 
    inFile >> nouse;             // VertexList
    inFile >> nouse;             // (pos,
    inFile >> nouse;             // normal)
    inFile >> nouse;             // {

    indexCount *= 3;

    std::vector<FireFlame::FLVertexNormal> vertices;
    vertices.reserve(vertexCount);
    for (size_t i = 0; i < vertexCount; i++)
    {
        float x, y, z, u, v, w;
        inFile >> x >> y >> z >> u >> v >> w;
        vertices.emplace_back(x, y, z, u, v, w);
    }

    inFile >> nouse >> nouse >> nouse;
    std::vector<std::uint16_t> indices;
    indices.reserve(indexCount);
    for (size_t i = 0; i < indexCount; i++)
    {
        std::uint16_t index = 0;
        inFile >> index;
        indices.push_back(index);
    }
    inFile.close();

    mMeshDesc.emplace_back();
    mMeshDesc[1].name = "Skull";
    mMeshDesc[1].indexCount = (unsigned int)indices.size();
    mMeshDesc[1].indexFormat = FireFlame::Index_Format::UINT16;
    mMeshDesc[1].indices = indices.data();

    mMeshDesc[1].vertexDataCount.push_back((unsigned int)vertices.size());
    mMeshDesc[1].vertexDataSize.push_back(sizeof(FireFlame::FLVertexNormal));
    mMeshDesc[1].vertexData.push_back(vertices.data());

    // sub meshes
    mMeshDesc[1].subMeshs.emplace_back("All", (UINT)indices.size());
    mEngine.GetScene()->AddPrimitive(mMeshDesc[1]);
}

void ShapesApp::BuildRenderItems() 
{
    using namespace DirectX;

    auto skullMat = mMaterials["skull"];
    auto boxMat = mMaterials["skull"];
    auto sphereMat = mMaterials["skull"];
    auto cylMat = mMaterials["skull"];
    FireFlame::stRenderItemDesc RItem("Box1", mMeshDesc[0].subMeshs[0]);
    RItem.mat = boxMat.name;
    XMFLOAT4X4 worldTrans;
    XMStoreFloat4x4
    (
        &worldTrans, 
        XMMatrixTranspose(XMMatrixScaling(2.0f, 2.0f, 2.0f)*XMMatrixTranslation(0.0f, 0.5f, 0.0f))
    );
    RItem.dataLen = sizeof(XMFLOAT4X4);
    RItem.data = &worldTrans;
    mRenderItems.emplace_back(RItem);
    mEngine.GetScene()->AddRenderItem
    (
        mMeshDesc[0].name,
        mShaderDesc.name,
        RItem
    );

    // Skull
    RItem.name = "Skull";
    RItem.mat = skullMat.name;
    RItem.subMesh = mMeshDesc[1].subMeshs[0];
    worldTrans = FireFlame::Matrix4X4();
    XMStoreFloat4x4
    (
        &worldTrans,
        XMMatrixTranspose(XMMatrixScaling(0.5f, 0.5f, 0.5f)*XMMatrixTranslation(0.0f, 1.5f, 0.0f))
    );
    mRenderItems.emplace_back(RItem);
    mEngine.GetScene()->AddRenderItem
    (
        mMeshDesc[1].name,
        mShaderDesc.name,
        RItem
    );

    RItem.name = "Grid1";
    RItem.subMesh = mMeshDesc[0].subMeshs[1];
    worldTrans = FireFlame::Matrix4X4();
    mRenderItems.emplace_back(RItem);
    mEngine.GetScene()->AddRenderItem
    (
        mMeshDesc[0].name,
        mShaderDesc.name,
        RItem
    );
    
    for (int i = 0; i < 5; ++i)
    {
        XMMATRIX leftCylWorld = XMMatrixTranslation(-5.0f, 1.5f, -10.0f + i*5.0f);
        XMMATRIX rightCylWorld = XMMatrixTranslation(+5.0f, 1.5f, -10.0f + i*5.0f);
        XMMATRIX leftSphereWorld = XMMatrixTranslation(-5.0f, 3.5f, -10.0f + i*5.0f);
        XMMATRIX rightSphereWorld = XMMatrixTranslation(+5.0f, 3.5f, -10.0f + i*5.0f);

        mSpherePos.emplace_back(-5.0f, 3.5f, -10.0f + i*5.0f);
        mSpherePos.emplace_back(+5.0f, 3.5f, -10.0f + i*5.0f);

        RItem.mat = cylMat.name;
        XMStoreFloat4x4(&worldTrans, XMMatrixTranspose(rightCylWorld));
        RItem.name = "RightCyl" + std::to_string(i);
        RItem.subMesh = mMeshDesc[0].subMeshs[3];
        mRenderItems.emplace_back(RItem);
        mEngine.GetScene()->AddRenderItem
        (
            mMeshDesc[0].name,
            mShaderDesc.name,
            RItem
        );

        XMStoreFloat4x4(&worldTrans, XMMatrixTranspose(leftCylWorld));
        RItem.name = "LeftCyl" + std::to_string(i);
        //RItem.subMesh = mMeshDesc.subMeshs[3];
        mRenderItems.emplace_back(RItem);
        mEngine.GetScene()->AddRenderItem
        (
            mMeshDesc[0].name,
            mShaderDesc.name,
            RItem
        );

        RItem.mat = sphereMat.name;
        XMStoreFloat4x4(&worldTrans, XMMatrixTranspose(leftSphereWorld));
        RItem.name = "LeftSphere" + std::to_string(i);
        RItem.subMesh = mMeshDesc[0].subMeshs[2];
        mRenderItems.emplace_back(RItem);
        mEngine.GetScene()->AddRenderItem
        (
            mMeshDesc[0].name,
            mShaderDesc.name,
            RItem
        );

        XMStoreFloat4x4(&worldTrans, XMMatrixTranspose(rightSphereWorld));
        RItem.name = "RightSphere" + std::to_string(i);
        //RItem.subMesh = mMeshDesc.subMeshs[2];
        mRenderItems.emplace_back(RItem);
        mEngine.GetScene()->AddRenderItem
        (
            mMeshDesc[0].name,
            mShaderDesc.name,
            RItem
        );
    }
}