#include "ShapesApp.h"
#include <fstream>

void ShapesApp::Initialize() 
{
    BuildShaders();
    BuildMesh();
    BuildRenderItems();

    mPasses.push_back("DefaultPass");
    mEngine.GetScene()->AddPass(mShaderDesc.name, mPasses[0]);
}

void ShapesApp::BuildShaders()
{
    using namespace FireFlame;
    mShaderDesc.name = "ShapesApp";
    mShaderDesc.objCBSize = sizeof(ObjectConsts);
    mShaderDesc.passCBSize = sizeof(PassConstants);
    mShaderDesc.AddVertexInput("POSITION", FireFlame::VERTEX_FORMAT_FLOAT3);
    mShaderDesc.AddVertexInput("COLOR", FireFlame::VERTEX_FORMAT_FLOAT4);
    //mShaderDesc.AddVertexInput("COLOR", FireFlame::VERTEX_FORMAT_A8R8G8B8_UNORM);
    mShaderDesc.AddShaderStage(L"Shaders\\ShapesShader.hlsl", Shader_Type::VS, "VS", "vs_5_0");
    mShaderDesc.AddShaderStage(L"Shaders\\ShapesShader.hlsl", Shader_Type::PS, "PS", "ps_5_0");

    mEngine.GetScene()->AddShader(mShaderDesc);
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
    std::vector<FireFlame::FLVertex> vertices(totalVertexCount);
    UINT k = 0;
    for (size_t i = 0; i < box.Vertices.size(); ++i, ++k)
    {
        vertices[k].Pos = box.Vertices[i].Position;
        vertices[k].Color = { 0.000000000f, 0.392156899f, 0.000000000f, 1.000000000f };
    }
    for (size_t i = 0; i < grid.Vertices.size(); ++i, ++k)
    {
        vertices[k].Pos = grid.Vertices[i].Position;
        vertices[k].Color = { 0.133333340f, 0.545098066f, 0.133333340f, 1.000000000f };
    }
    for (size_t i = 0; i < sphere.Vertices.size(); ++i, ++k)
    {
        vertices[k].Pos = sphere.Vertices[i].Position;
        vertices[k].Color = { 0.862745166f, 0.078431375f, 0.235294133f, 1.000000000f };
    }
    for (size_t i = 0; i < cylinder.Vertices.size(); ++i, ++k)
    {
        vertices[k].Pos = cylinder.Vertices[i].Position;
        vertices[k].Color = { 0.274509817f, 0.509803951f, 0.705882370f, 1.000000000f };
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
    mMeshDesc[0].vertexDataSize.push_back(sizeof(FLVertex));
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

    std::vector<FireFlame::FLVertex> vertices;
    vertices.reserve(vertexCount);
    for (size_t i = 0; i < vertexCount; i++)
    {
        float x, y, z, u, v, w;
        inFile >> x >> y >> z >> u >> v >> w;
        vertices.emplace_back(x, y, z);
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

    for (size_t i = 0; i < vertexCount; ++i)
    {
        vertices[i].Color = { 1.0f, 1.0f, 1.0f, 1.0f };
    }

    mMeshDesc.emplace_back();
    mMeshDesc[1].name = "Skull";
    mMeshDesc[1].indexCount = (unsigned int)indices.size();
    mMeshDesc[1].indexFormat = FireFlame::Index_Format::UINT16;
    mMeshDesc[1].indices = indices.data();

    mMeshDesc[1].vertexDataCount.push_back((unsigned int)vertices.size());
    mMeshDesc[1].vertexDataSize.push_back(sizeof(FireFlame::FLVertex));
    mMeshDesc[1].vertexData.push_back(vertices.data());

    // sub meshes
    mMeshDesc[1].subMeshs.emplace_back("All", (UINT)indices.size());
    mEngine.GetScene()->AddPrimitive(mMeshDesc[1]);
}

void ShapesApp::BuildRenderItems() 
{
    using namespace DirectX;

    FireFlame::stRenderItemDesc RItem("Box1", mMeshDesc[0].subMeshs[0]);
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