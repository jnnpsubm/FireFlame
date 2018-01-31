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
    mSilverKnightLoader.load("Models\\silverknight\\silverknight.flver");

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
    mEngine.GetScene()->UpdateShaderPassCBData(mShaderDescs["model_shader_tess"].name, sizeof(PassConstants), &passCB);
    mEngine.GetScene()->UpdateShaderPassCBData(mShaderDescs["ds_model_shader_tess"].name, sizeof(PassConstants), &passCB);
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
    AddShaderModelTess();
    AddShaderDSModel();
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
    shaderDesc.AddShaderStage(strHlslFile, Shader_Type::PS, "PS", "ps_5_0");

    mEngine.GetScene()->AddShader(shaderDesc);
}

void TSPNTriangleApp::AddShaderModelTess()
{
    using namespace FireFlame;

    auto& shaderDesc = mShaderDescs["model_shader_tess"];
    shaderDesc.name = "model_shader_tess";
    shaderDesc.objCBSize = sizeof(ObjectConsts);
    shaderDesc.passCBSize = sizeof(PassConstants);
    shaderDesc.materialCBSize = sizeof(MaterialConstants);
    shaderDesc.ParamDefault();

    std::wstring strHlslFile = L"Shaders\\ModelTess.hlsl";
    shaderDesc.AddVertexInput("POSITION", FireFlame::VERTEX_FORMAT_FLOAT3);
    shaderDesc.AddVertexInput("NORMAL", FireFlame::VERTEX_FORMAT_FLOAT3);
    shaderDesc.AddVertexInput("TEXCOORD", FireFlame::VERTEX_FORMAT_FLOAT2);
    shaderDesc.AddShaderStage(strHlslFile, Shader_Type::VS, "VS", "vs_5_0");
    shaderDesc.AddShaderStage(strHlslFile, Shader_Type::HS, "HS", "hs_5_0");
    shaderDesc.AddShaderStage(strHlslFile, Shader_Type::DS, "DS", "ds_5_0");
    shaderDesc.AddShaderStage(strHlslFile, Shader_Type::PS, "PS", "ps_5_0");

    mEngine.GetScene()->AddShader(shaderDesc);
}

void TSPNTriangleApp::AddShaderDSModel()
{
    using namespace FireFlame;

    auto& shaderDesc = mShaderDescs["ds_model_shader_tess"];
    shaderDesc.name = "ds_model_shader_tess";
    shaderDesc.objCBSize = sizeof(ObjectConsts);
    shaderDesc.passCBSize = sizeof(PassConstants);
    shaderDesc.materialCBSize = sizeof(MaterialConstants);
    shaderDesc.texSRVDescriptorTableSize = 4;
    shaderDesc.ParamDefault();

    std::wstring strHlslFile = L"Shaders\\DSModelTess.hlsl";
    shaderDesc.AddVertexInput("POSITION", FireFlame::VERTEX_FORMAT_FLOAT3);
    shaderDesc.AddVertexInput("NORMAL", FireFlame::VERTEX_FORMAT_FLOAT3);
    shaderDesc.AddVertexInput("TANGENT", FireFlame::VERTEX_FORMAT_FLOAT3);
    shaderDesc.AddVertexInput("TEXCOORD", FireFlame::VERTEX_FORMAT_FLOAT2);
    shaderDesc.AddShaderStage(strHlslFile, Shader_Type::VS, "VS", "vs_5_0");
    shaderDesc.AddShaderStage(strHlslFile, Shader_Type::HS, "HS", "hs_5_0");
    shaderDesc.AddShaderStage(strHlslFile, Shader_Type::DS, "DS", "ds_5_0");
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

    PSODesc descModelTess(mShaderDescs["model_shader_tess"].name);
    descModelTess.topologyType = Primitive_Topology_Type::Patch;
    descModelTess.cullMode = Cull_Mode::None;
    descModelTess.opaque = false;
    descModelTess.alpha2Coverage = true;
    mEngine.GetScene()->AddPSO("model_pso_tess", descModelTess);

    PSODesc descDSModelTess(mShaderDescs["ds_model_shader_tess"].name);
    descDSModelTess.topologyType = Primitive_Topology_Type::Patch;
    descDSModelTess.cullMode = Cull_Mode::None;
    //descDSModelTess.opaque = false;
    //descDSModelTess.alpha2Coverage = true;
    mEngine.GetScene()->AddPSO("ds_model_pso_tess", descDSModelTess);
}

void TSPNTriangleApp::AddTextures()
{
    mEngine.GetScene()->AddTexture
    (
        "illidan_tex",
        //L"Models\\illidan\\illidan_illidan2.dds"
        L"Models\\illidananims\\illidan_illidan.dds"
        //L"Models\\lobster\\lobster_lobstrok2_red.dds"
    );

    mSKTextures.push_back("head");
    mSKTexFiles.push_back(L"c1410_HD8500_");
    mSKTextures.push_back("body1");
    mSKTexFiles.push_back(L"c1410_BD8500_1_");
    mSKTextures.push_back("body2");
    mSKTexFiles.push_back(L"c1410_BD8500_2_");
    mSKTextures.push_back("leg");
    mSKTexFiles.push_back(L"c1410_LG8500_");
    mSKTextures.push_back("armor");
    mSKTexFiles.push_back(L"c1410_AM8500_");
    mSKTextures.push_back("shield");
    mSKTexFiles.push_back(L"c1410_shield_");
    mSKTextures.push_back("spear");
    mSKTexFiles.push_back(L"c1410_spear_");
    mSKTextures.push_back("sword");
    mSKTexFiles.push_back(L"c1410_sword_");
    mSKTextures.push_back("weapon");
    mSKTexFiles.push_back(L"c1410_WP1360_");
    mSKTextures.push_back("arrow");
    mSKTexFiles.push_back(L"c1410_arrow_");
    for (size_t i = 0; i < mSKTextures.size(); i++)
    {
        mEngine.GetScene()->AddTexture
        (
            mSKTextures[i]+"_a",
            std::wstring(L"Models\\silverKnight\\textures\\") + mSKTexFiles[i]+L"a.dds"
        );
        mEngine.GetScene()->AddTexture
        (
            mSKTextures[i] + "_r",
            std::wstring(L"Models\\silverKnight\\textures\\") + mSKTexFiles[i] + L"r.dds"
        );
        mEngine.GetScene()->AddTexture
        (
            mSKTextures[i] + "_n",
            std::wstring(L"Models\\silverKnight\\textures\\") + mSKTexFiles[i] + L"n.dds"
        );
    }
}

void TSPNTriangleApp::AddMaterials()
{
    auto& metal = mMaterials["metal"];
    metal.Name = "metal";
    metal.DiffuseAlbedo = FireFlame::Vector4f(0.2f, 0.5f, 0.5f, 1.0f);
    metal.FresnelR0 = FireFlame::Vector3f(0.75f, 0.75f, 0.75f);
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

    auto& model_mat_tess = mMaterials["model_mat_tess"];
    model_mat_tess.Name = "model_mat_tess";
    model_mat_tess.DiffuseAlbedo = FireFlame::Vector4f(1.5f, 1.5f, 1.5f, 1.0f);
    model_mat_tess.FresnelR0 = FireFlame::Vector3f(0.05f, 0.05f, 0.05f);
    model_mat_tess.Roughness = 0.05f;
    mEngine.GetScene()->AddMaterial
    (
        model_mat_tess.Name,
        mShaderDescs["model_shader_tess"].name, "illidan_tex",
        sizeof(MaterialConstants), &model_mat_tess
    );

    for (size_t i = 0; i < mSKTextures.size(); i++)
    {
        std::string matName = std::string("ds_model_mat_tess_") + std::to_string(i);
        auto& ds_model_mat_tess = mMaterials[matName];
        ds_model_mat_tess.Name = matName;
        ds_model_mat_tess.DiffuseAlbedo = FireFlame::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
        ds_model_mat_tess.FresnelR0 = FireFlame::Vector3f(0.85f, 0.85f, 0.85f);
        ds_model_mat_tess.Roughness = 0.85f;
        mEngine.GetScene()->AddMaterial
        (
            {
                ds_model_mat_tess.Name,
                mShaderDescs["ds_model_shader_tess"].name, 
                {
                    { mSKTextures[i]+"_a", FireFlame::SRV_DIMENSION::TEXTURE2D },
                    { mSKTextures[i]+"_r", FireFlame::SRV_DIMENSION::TEXTURE2D },
                    { mSKTextures[i]+"_n", FireFlame::SRV_DIMENSION::TEXTURE2D }
                },
                sizeof(MaterialConstants), &ds_model_mat_tess
            }
        );
    }
    std::string matName = std::string("ds_model_mat_defalt");
    auto& ds_model_defalt = mMaterials[matName];
    ds_model_defalt.Name = matName;
    ds_model_defalt.DiffuseAlbedo = FireFlame::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    ds_model_defalt.FresnelR0 = FireFlame::Vector3f(0.85f, 0.85f, 0.85f);
    ds_model_defalt.Roughness = 0.85f;
    ds_model_defalt.UseTexture = 0;
    mEngine.GetScene()->AddMaterial
    (
        ds_model_defalt.Name,
        mShaderDescs["ds_model_shader_tess"].name, "",
        sizeof(MaterialConstants), &ds_model_defalt
    );
}

void TSPNTriangleApp::AddMeshs()
{
    AddMeshOctahedron();
    //AddMeshObjFromFile("Models\\illidan\\illidan.obj");
    AddMeshObjFromFile("Models\\illidananims\\illidan.obj");
    //AddMeshObjFromFile("Models\\lobster\\lobster.obj");
    AddMeshDSModel();
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

void TSPNTriangleApp::AddMeshDSModel()
{
    auto parts = mSilverKnightLoader.get_part_count();
    for (size_t i = 0; i < parts; i++)
    {
        AddMeshDSModelPart(i, true);
    }
}

void TSPNTriangleApp::AddMeshDSModelPart(size_t part, bool reverseNormal)
{
    using namespace FireFlame;

    std::vector<FireFlame::FLVertexNormalTangentTex> vertices;
    auto& rawUVs = mSilverKnightLoader.get_uvs();
    auto& rawVertices = mSilverKnightLoader.get_vertices();
    auto& rawIndices = mSilverKnightLoader.get_indices();

    std::vector<std::uint32_t> indices;
    for (const auto& index : rawIndices[part])
    {
        indices.push_back(index);
    }

    vertices.reserve(rawVertices[part].size());
    for (size_t i = 0; i < rawVertices[part].size(); i++)
    {
        vertices.emplace_back
        (
            rawVertices[part][i].x, rawVertices[part][i].y, rawVertices[part][i].z,
            0.f, 0.f, 0.f,
            rawUVs[part][i].u, -rawUVs[part][i].v
        );
    }

    // begin normals
    for (size_t i = 0; i < indices.size() / 3; i++)
    {
        auto& v0 = vertices[indices[i * 3 + 0]];
        auto& v1 = vertices[indices[i * 3 + 1]];
        auto& v2 = vertices[indices[i * 3 + 2]];
        auto e0 = v1.Pos - v0.Pos;
        auto e1 = v2.Pos - v0.Pos;
        auto normal = FireFlame::Vector3Cross(reverseNormal ? e1 : e0, reverseNormal ? e0 : e1);

        v0.Normal += normal;
        v1.Normal += normal;
        v2.Normal += normal;
    }
    for (auto& vertex : vertices)
    {
        vertex.Normal.Normalize();
    }
    // end normals
    // begin tangents
    /*for (size_t i = 0; i < indices.size() / 3; i++)
    {
        auto& p0 = vertices[indices[i * 3 + 0]];
        auto& p1 = vertices[indices[i * 3 + 1]];
        auto& p2 = vertices[indices[i * 3 + 2]];
        auto Q0 = p1.Pos - p0.Pos;
        auto Q1 = p2.Pos - p0.Pos;
        DirectX::XMFloat
        auto normal = FireFlame::Vector3Cross(reverseNormal ? e1 : e0, reverseNormal ? e0 : e1);

        v0.Normal += normal;
        v1.Normal += normal;
        v2.Normal += normal;
    }
    for (auto& vertex : vertices)
    {
        vertex.Normal.Normalize();
    }*/
    // end tangents

    std::string meshName = "ds_model_" + std::to_string(part);
    auto& mesh = mMeshDescs[meshName];
    mesh.name = meshName;
    mesh.indexCount = (unsigned int)indices.size();
    mesh.indexFormat = Index_Format::UINT32;
    mesh.indices = indices.data();

    mesh.vertexDataCount.push_back((unsigned int)vertices.size());
    mesh.vertexDataSize.push_back(sizeof(FLVertexNormalTangentTex));
    mesh.vertexData.push_back(vertices.data());

    // sub meshes
    mesh.subMeshs.emplace_back("All", (UINT)indices.size());
    mEngine.GetScene()->AddPrimitive(mesh);
}

void TSPNTriangleApp::AddRenderItems()
{
    //AddRenderItemOctahedron();
    //AddRenderItemModel();
    AddRenderItemModelTess();
    //AddRenderItemDSModel();
}

void TSPNTriangleApp::AddRenderItemOctahedron()
{
    using namespace DirectX;

    FireFlame::stRenderItemDesc RItem("Octahedron", mMeshDescs["quad"].subMeshs[0]);
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
        XMMatrixTranspose(XMMatrixTranslation(-6.0f, 0.0f, 0.0f))
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

void TSPNTriangleApp::AddRenderItemModelTess()
{
    using namespace DirectX;

    FireFlame::stRenderItemDesc RItem("model2Tess", mMeshDescs["model1"].subMeshs[0]);
    RItem.topology = FireFlame::Primitive_Topology::CONTROL_POINT_PATCHLIST_3;
    RItem.mat = "model_mat_tess";
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
        mShaderDescs["model_shader_tess"].name,
        "model_pso_tess",
        //mShaderDescs["main"].name,
        //"default",
        0,
        RItem
    );
}

void TSPNTriangleApp::AddRenderItemDSModel()
{
    using namespace DirectX;

    auto parts = mSilverKnightLoader.get_part_count();
    for (size_t part = 0; part < parts; part++) // 9
    {
        //if (part == 9) continue;

        std::string matName = "ds_model_mat_defalt";
        std::string meshName = "ds_model_" + std::to_string(part);

        // add render item
        FireFlame::stRenderItemDesc RItem(meshName, mMeshDescs[meshName].subMeshs[0]);
        RItem.topology = FireFlame::Primitive_Topology::CONTROL_POINT_PATCHLIST_3;
        RItem.mat = matName;
        XMFLOAT4X4 trans[2];
        DirectX::XMMATRIX fkRotate = XMMatrixRotationY(0/*FireFlame::MathHelper::FL_PI*/);
        DirectX::XMMATRIX fkScale = XMMatrixScaling(0.03f, 0.03f, 0.03f);
        DirectX::XMMATRIX fkOffset = XMMatrixTranslation(0.0f, -4.0f, -4.0f);
        DirectX::XMMATRIX fkWorld = fkRotate * fkScale*fkOffset;
        XMStoreFloat4x4
        (
            &trans[0],
            XMMatrixTranspose(fkWorld)
        );
        XMStoreFloat4x4
        (
            &trans[1],
            XMMatrixIdentity()
        );
        RItem.dataLen = sizeof(XMFLOAT4X4)*_countof(trans);
        RItem.data = &trans[0];
        mRenderItems[RItem.name] = RItem;
        mEngine.GetScene()->AddRenderItem
        (
            mMeshDescs[meshName].name,
            "ds_model_shader_tess",
            "ds_model_pso_tess",
            RItem
        );
    }
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
    }else if (wParam == 'Z')
    {
        mTessLod = 1.f;
    }
    else if (wParam == 'M')
    {
        mTessLod = 64.f;
    }

    bool bDirty = false;
    if (wParam >= '0' && wParam <= '9')
    {
        mCurrDSModelPart = "ds_model_" + std::to_string(wParam - '0');
        bDirty = true;
    }
    else if (wParam == 'P')
    {
        mCurrDSModelPart = "ds_model_10";
        bDirty = true;
    }
    else if (wParam == 'O')
    {
        mCurrDSModelPart = "ds_model_11";
        bDirty = true;
    }
    else if (wParam == 'I')
    {
        mCurrDSModelPart = "ds_model_12";
        bDirty = true;
    }
    else if (wParam == 'R')
    {
        ++mCurrDSMaterial;
        if (mCurrDSMaterial >= (int)mSKTextures.size())
        {
            mCurrDSMaterial = 0;
        }
        bDirty = true;
    }
    else if (wParam == 'T')
    {
        --mCurrDSMaterial;
        if (mCurrDSMaterial < 0)
        {
            mCurrDSMaterial = 0;
        }
        bDirty = true;
    }
    else if (wParam == 'S')
    {
        for (const auto& pairPM : mMapPartMat)
        {
            std::cout << "part:" << pairPM.first << " mat:" << pairPM.second << std::endl;
        }
    }
    if (bDirty)
    {
        mEngine.GetScene()->RenderItemChangeMaterial
        (
            mCurrDSModelPart,
            std::string("ds_model_mat_tess_") + std::to_string(mCurrDSMaterial)
        );
        std::cout << "part:" << mCurrDSModelPart << " material:" << mCurrDSMaterial << std::endl;
        mMapPartMat[mCurrDSModelPart] = mCurrDSMaterial;
    }
}