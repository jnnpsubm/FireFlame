#include "TSPNTriangleApp.h"
#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "src\3rd_utils\tinyobjloader\tiny_obj_loader.h"
#include <fstream>
#include <assert.h>

void TSPNTriangleApp::PreInitialize() {}

void TSPNTriangleApp::Initialize()
{
    //LoadFBXModel("Models\\illidan.fbx");
    //LoadFBXModel("Models\\b.fbx");

    AddShaders();
    AddPSOs();
    AddTextures();
    AddMaterials();
    AddMeshs();
    AddRenderItems();

    mPasses.push_back("DefaultPass");
    mEngine.GetScene()->AddPass(mPasses[0]);
}

void TSPNTriangleApp::Update(float time_elapsed)
{
    FLEngineApp3::Update(time_elapsed);
}

void TSPNTriangleApp::UpdateMainPassCB(float time_elapsed)
{
    using namespace DirectX;

    // pass constants
    XMMATRIX view = XMLoadFloat4x4(&mView);
    XMMATRIX proj = XMLoadFloat4x4(&mProj);

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
    passCB.EyePosW = mEyePos;
    float clientWidth = (float)mEngine.GetWindow()->ClientWidth();
    float clientHeight = (float)mEngine.GetWindow()->ClientHeight();
    passCB.RenderTargetSize = XMFLOAT2(clientWidth, clientHeight);
    passCB.InvRenderTargetSize = XMFLOAT2(1.0f / clientWidth, 1.0f / clientHeight);
    passCB.NearZ = 1.0f;
    passCB.FarZ = 1000.0f;
    passCB.TotalTime = mEngine.TotalTime();
    passCB.DeltaTime = mEngine.DeltaTime();

    passCB.AmbientLight = { 0.25f, 0.25f, 0.35f, 1.0f };
    passCB.Lights[0].Direction = { 0.57735f, -0.57735f, -0.57735f };
    passCB.Lights[0].Strength = { 0.6f, 0.6f, 0.6f };
    passCB.Lights[1].Direction = { -0.57735f, -0.57735f, 0.57735f };
    passCB.Lights[1].Strength = { 0.3f, 0.3f, 0.3f };
    passCB.Lights[2].Direction = { 0.0f, -0.707f, -0.707f };
    passCB.Lights[2].Strength = { 0.15f, 0.15f, 0.15f };

    passCB.TessLod = mTessLod;
    mEngine.GetScene()->UpdateShaderPassCBData(mShaderDescs["main"].name, sizeof(PassConstants), &passCB);
    mEngine.GetScene()->UpdateShaderPassCBData(mShaderDescs["model_shader"].name, sizeof(PassConstants), &passCB);
}

void TSPNTriangleApp::LoadFBXModel(const std::string& fileName)
{
    mFBXModel.read(fileName);
    //mFBXModel.print();

    std::ofstream outFile("D:\\temp\\FBXFileInfo.tex");
    std::cout << "======================================model nodes===========================================" << std::endl;
    outFile << "version:" << mFBXModel.getVersion() << std::endl;
    for (const auto& node : mFBXModel.nodes)
    {
        PrintFBXModelInfo(outFile, "", node);
    }
    std::cout << "======================================end model nodes=======================================" << std::endl;
}

void TSPNTriangleApp::PrintFBXModelInfo(std::ofstream& out, const std::string& prefix, const fbx::FBXNode& node)
{
    out << prefix << "node:" << node.getName() << std::endl;
    for (const auto& property : node.getProperties())
    {
        auto strProperty = property.to_string();
        out << prefix+"    " << "type:" << property.getType()
            << " property:" << (strProperty.size()<128?strProperty:"too long......") << std::endl;
    }
    for (const auto& childNode : node.getChildren())
    {
        PrintFBXModelInfo(out, prefix + "    ", childNode);
    }
}

void TSPNTriangleApp::AddShaders()
{
    AddShaderOctahedron();
    AddShaderModel();
}

void TSPNTriangleApp::AddShaderOctahedron()
{
    using namespace FireFlame;

    auto& shaderDesc = mShaderDescs["main"];
    shaderDesc.name = "main";
    shaderDesc.objCBSize = sizeof(ObjectConsts);
    shaderDesc.passCBSize = sizeof(PassConstants);
    shaderDesc.materialCBSize = sizeof(MaterialConstants);
    shaderDesc.ParamDefault();

    std::wstring strHlslFile = L"Shaders\\Octahedron.hlsl";
    shaderDesc.AddVertexInput("POSITION", FireFlame::VERTEX_FORMAT_FLOAT3);
    shaderDesc.AddShaderStage(strHlslFile, Shader_Type::VS, "VS", "vs_5_0");
    shaderDesc.AddShaderStage(strHlslFile, Shader_Type::HS, "HS", "hs_5_0");
    shaderDesc.AddShaderStage(strHlslFile, Shader_Type::DS, "DS", "ds_5_0");
    shaderDesc.AddShaderStage(strHlslFile, Shader_Type::PS, "PS", "ps_5_0");

    mEngine.GetScene()->AddShader(shaderDesc);
}

void TSPNTriangleApp::AddShaderModel()
{
    using namespace FireFlame;

    auto& shaderDesc = mShaderDescs["model_shader"];
    shaderDesc.name = "model_shader";
    shaderDesc.objCBSize = sizeof(ObjectConsts);
    shaderDesc.passCBSize = sizeof(PassConstants);
    shaderDesc.materialCBSize = sizeof(MaterialConstants);
    shaderDesc.ParamDefault();

    std::wstring strHlslFile = L"Shaders\\Model.hlsl";
    shaderDesc.AddVertexInput("POSITION", FireFlame::VERTEX_FORMAT_FLOAT3);
    shaderDesc.AddVertexInput("NORMAL", FireFlame::VERTEX_FORMAT_FLOAT3);
    shaderDesc.AddVertexInput("TEXCOORD", FireFlame::VERTEX_FORMAT_FLOAT2);
    shaderDesc.AddShaderStage(strHlslFile, Shader_Type::VS, "VS", "vs_5_0");
    //shaderDesc.AddShaderStage(strHlslFile, Shader_Type::HS, "HS", "hs_5_0");
    //shaderDesc.AddShaderStage(strHlslFile, Shader_Type::DS, "DS", "ds_5_0");
    shaderDesc.AddShaderStage(strHlslFile, Shader_Type::PS, "PS", "ps_5_0");

    mEngine.GetScene()->AddShader(shaderDesc);
}

void TSPNTriangleApp::AddPSOs()
{
    using namespace FireFlame;

    PSODesc descDefault(mShaderDescs["main"].name);
    descDefault.topologyType = Primitive_Topology_Type::Patch;
    descDefault.cullMode = Cull_Mode::None;
    mEngine.GetScene()->AddPSO("default", descDefault);

    PSODesc descModel(mShaderDescs["model_shader"].name);
    descModel.topologyType = Primitive_Topology_Type::Triangle;
    descModel.cullMode = Cull_Mode::None;
    descModel.opaque = false;
    descModel.alpha2Coverage = true;
    mEngine.GetScene()->AddPSO("model_pso", descModel);
}

void TSPNTriangleApp::AddTextures()
{
    mEngine.GetScene()->AddTexture
    (
        "illidan_tex",
        L"Models\\illidan\\illidan_illidan2.dds"
    );
}

void TSPNTriangleApp::AddMaterials()
{
    auto& metal = mMaterials["metal"];
    metal.Name = "metal";
    metal.DiffuseAlbedo = FireFlame::Vector4f(0.2f, 0.5f, 0.5f, 1.0f);
    metal.FresnelR0 = FireFlame::Vector3f(0.35f, 0.75f, 0.75f);
    metal.Roughness = 0.725f;
    mEngine.GetScene()->AddMaterial
    (
        metal.Name,
        mShaderDescs["main"].name, "",
        sizeof(MaterialConstants), &metal
    );

    auto& model_mat = mMaterials["model_mat"];
    model_mat.Name = "model_mat";
    model_mat.DiffuseAlbedo = FireFlame::Vector4f(1.5f, 1.5f, 1.5f, 1.0f);
    model_mat.FresnelR0 = FireFlame::Vector3f(0.75f, 0.75f, 0.75f);
    model_mat.Roughness = 0.725f;
    mEngine.GetScene()->AddMaterial
    (
        model_mat.Name,
        mShaderDescs["model_shader"].name, "illidan_tex",
        sizeof(MaterialConstants), &model_mat
    );
}

void TSPNTriangleApp::AddMeshs()
{
    AddMeshOctahedron();
    AddMeshObjFromFile("Models\\illidan\\illidan.obj");
}

void TSPNTriangleApp::AddMeshOctahedron()
{
    using namespace FireFlame;

    GeometryGenerator geoGen;
    //auto mesh = geoGen.CreateIcosahedron();
    auto mesh = geoGen.CreateOctahedron();
    std::vector<FLVertex> vertices(mesh.Vertices.size());
    for (size_t i = 0; i < mesh.Vertices.size(); ++i)
    {
        auto& p = mesh.Vertices[i].Position;
        vertices[i].Pos = p;
    }
    std::vector<std::uint16_t> indices = mesh.GetIndices16();

    auto& meshDesc = mMeshDescs["quad"];
    meshDesc.name = "quad";
    meshDesc.indexCount = (unsigned int)indices.size();
    meshDesc.indexFormat = Index_Format::UINT16;
    meshDesc.indices = indices.data();

    meshDesc.vertexData.push_back(vertices.data());
    meshDesc.vertexDataCount.push_back((unsigned)vertices.size());
    meshDesc.vertexDataSize.push_back(sizeof(FLVertex));

    // sub meshes
    meshDesc.subMeshs.emplace_back("All", (UINT)indices.size());
    mEngine.GetScene()->AddPrimitive(meshDesc);
}

void TSPNTriangleApp::AddMeshObjFromFile(const std::string& fileName)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string err;
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, fileName.c_str());
    if (!err.empty()) { // `err` may contain warning message.
        std::cerr << err << std::endl;
        spdlog::get("console")->warn(err);
    }
    if (!ret) {
        std::cerr << "cannot load obj file:" << fileName << std::endl;
        spdlog::get("console")->error("cannot load obj file:{0}", fileName);
        return;
    }

    assert(attrib.vertices.size() % 3 == 0);
    std::vector<FireFlame::FLVertexNormalTex> vertices(attrib.vertices.size() / 3);
    for (size_t i = 0; i < vertices.size(); i++)
    {
        vertices[i].Pos = { attrib.vertices[3 * i + 0], attrib.vertices[3 * i + 1],attrib.vertices[3 * i + 2] };
        vertices[i].Normal = { attrib.normals[3 * i + 0],  attrib.normals[3 * i + 1], attrib.normals[3 * i + 2] };
        vertices[i].Tex = { attrib.texcoords[2 * i + 0],-attrib.texcoords[2 * i + 1] };
    }
    std::vector<std::uint32_t> indices;
    std::vector<std::uint32_t> subMeshIndexCount;

    // Loop over shapes
    for (size_t s = 0; s < shapes.size(); s++) {
        for (size_t i = 0; i < shapes[s].mesh.indices.size(); i++)
        {
            auto idx = shapes[s].mesh.indices[i];
            assert(idx.vertex_index == idx.normal_index);
            assert(idx.normal_index == idx.texcoord_index);
            indices.push_back(idx.vertex_index);
        }
        subMeshIndexCount.push_back((std::uint32_t)shapes[s].mesh.indices.size());
    }

    using namespace FireFlame;

    auto& meshDesc = mMeshDescs["model1"];
    meshDesc.name = "model1";
    meshDesc.indexCount = (unsigned int)indices.size();
    meshDesc.indexFormat = Index_Format::UINT32;
    meshDesc.indices = indices.data();

    meshDesc.vertexData.push_back(vertices.data());
    meshDesc.vertexDataCount.push_back((unsigned)vertices.size());
    meshDesc.vertexDataSize.push_back(sizeof(FLVertexNormalTex));

    // sub meshes
    /*for (auto indexCount : subMeshIndexCount)
    {
        meshDesc.subMeshs.emplace_back
        (
            std::string("part_") + std::to_string(indexCount),
            (UINT)indexCount,
        );
    }*/
    meshDesc.subMeshs.emplace_back("All", (UINT)indices.size());
    mEngine.GetScene()->AddPrimitive(meshDesc);
}

void TSPNTriangleApp::AddRenderItems()
{
    //AddRenderItemOctahedron();
    AddRenderItemModel();
}

void TSPNTriangleApp::AddRenderItemOctahedron()
{
    using namespace DirectX;

    FireFlame::stRenderItemDesc RItem("land", mMeshDescs["quad"].subMeshs[0]);
    RItem.topology = FireFlame::Primitive_Topology::CONTROL_POINT_PATCHLIST_3;
    XMFLOAT4X4 trans[1];
    XMStoreFloat4x4
    (
        &trans[0],
        XMMatrixTranspose(XMMatrixScaling(4.f, 4.f, 4.f))
    );
    RItem.dataLen = sizeof(XMFLOAT4X4)*_countof(trans);
    RItem.data = &trans[0];
    RItem.mat = "metal";
    mRenderItems[RItem.name] = RItem;
    mEngine.GetScene()->AddRenderItem
    (
        mMeshDescs["quad"].name,
        mShaderDescs["main"].name,
        "default",
        0,
        RItem
    );
}

void TSPNTriangleApp::AddRenderItemModel()
{
    using namespace DirectX;

    FireFlame::stRenderItemDesc RItem("model1", mMeshDescs["model1"].subMeshs[0]);
    RItem.mat = "model_mat";
    XMFLOAT4X4 trans[2];
    XMStoreFloat4x4
    (
        &trans[0],
        XMMatrixTranspose(XMMatrixTranslation(0.0f, 0.0f, 0.0f))
    );
    XMStoreFloat4x4
    (
        &trans[1],
        XMMatrixTranspose(XMMatrixScaling(1.0f, 1.0f, 1.0f))
    );
    RItem.dataLen = sizeof(XMFLOAT4X4)*_countof(trans);
    RItem.data = &trans[0];
    mRenderItems[RItem.name] = RItem;
    mEngine.GetScene()->AddRenderItem
    (
        mMeshDescs["model1"].name,
        mShaderDescs["model_shader"].name,
        "model_pso",
        0,
        RItem
    );
}

void TSPNTriangleApp::OnKeyUp(WPARAM wParam, LPARAM lParam)
{
    FLEngineApp3::OnKeyUp(wParam, lParam);
    if (wParam == 'L')
    {
        if (mTessLod < 64.f)
        {
            mTessLod += 1.f;
        }
        else
        {
            mTessLod = 1.f;
        }
    }
}