#include "ViewerApp.h"

ViewerApp::ViewerApp(FireFlame::Engine& e) : ViewerAppBase(e) {}

void ViewerApp::Initialize()
{
    //hkxloader.load("D:\\DSIII_CHR\\c1400\\hkx\\skeleton.HKX");
    //hkxloader.load("D:\\DSIII_CHR\\c3040\\hkx\\skeleton.HKX");

    flverloader.load("D:\\DSIII_CHR\\c1400\\c1400.flver");
    //flverloader.load("D:\\DSIII_CHR\\c3040\\c3040.flver");

    DecideScaleAndTrans();
    //save_parts(flverloader);
    //save_whole(flverloader);

    AddShaders();
    AddPSOs();
    AddTextures();
    AddPasses();

    AddFloor();
    auto partCount = flverloader.get_part_count();
    for (size_t i = 0; i < 2; i++)
    {
        
    }
    AddHKXModel(0, true);
    AddHKXModel(1, true);
    AddHKXModel(2, true);
    AddHKXModel(3, true);
    AddHKXModel(4, true);
    AddHKXModel(5, true);
    AddHKXModel(6, true);
    AddHKXModel(7, true);
    AddHKXModel(8, true);
    AddHKXModel(9, true);
    AddHKXModel(10, true);
    AddHKXModel(11, true);
    AddHKXModel(12, true);
    AddHKXModel(13, true);
    AddHKXModel(14, true);
    AddHKXModel(15, true);
    AddHKXModel(16, true);
    AddHKXModel(17, false);
}

void ViewerApp::AddShaders()
{
    using namespace FireFlame;
    mShaderDesc.name = "Default";
    mShaderDesc.objCBSize = sizeof(ObjectConsts);
    mShaderDesc.passCBSize = sizeof(PassConstants);
    mShaderDesc.materialCBSize = sizeof(MaterialConstants);
    mShaderDesc.texSRVDescriptorTableSize = 2;
    mShaderDesc.ParamDefault();
    mShaderDesc.AddVertexInput("POSITION", FireFlame::VERTEX_FORMAT_FLOAT3);
    mShaderDesc.AddVertexInput("NORMAL", FireFlame::VERTEX_FORMAT_FLOAT3);
    mShaderDesc.AddVertexInput("TEXCOORD", FireFlame::VERTEX_FORMAT_FLOAT2);
    auto& vs = mShaderDesc.AddShaderStage(L"Shaders\\Default.hlsl", Shader_Type::VS, "VS", "vs_5_0");
    mShaderMacrosVS[""] = "";
    auto& ps = mShaderDesc.AddShaderStage(L"Shaders\\Default.hlsl", Shader_Type::PS, "PS", "ps_5_0");
    mShaderMacrosPS[""] = "";
    mEngine.GetScene()->AddShader(mShaderDesc);
}

void ViewerApp::AddPSOs()
{
    using namespace FireFlame;
    auto scene = Engine::GetEngine()->GetScene();

    PSODesc desc(mShaderDesc.name, mShaderMacrosVS[""], mShaderMacrosPS[""]);
    desc.cullMode = Cull_Mode::None;
    scene->AddPSO("default", desc);
}

void ViewerApp::AddTextures()
{
    mEngine.GetScene()->AddTexture
    (
        "arm",
        L"..\\..\\Resources\\Textures\\FireKeeper\\c1400_arm_a.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "arm_r",
        L"..\\..\\Resources\\Textures\\FireKeeper\\c1400_arm_r.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "body",
        L"..\\..\\Resources\\Textures\\FireKeeper\\c1400_body_a.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "body_r",
        L"..\\..\\Resources\\Textures\\FireKeeper\\c1400_body_r.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "hair",
        L"..\\..\\Resources\\Textures\\FireKeeper\\c1400_hair_a.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "hair_r",
        L"..\\..\\Resources\\Textures\\FireKeeper\\c1400_hair_r.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "head",
        L"..\\..\\Resources\\Textures\\FireKeeper\\c1400_head_a.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "head_r",
        L"..\\..\\Resources\\Textures\\FireKeeper\\c1400_head_r.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "leg",
        L"..\\..\\Resources\\Textures\\FireKeeper\\c1400_leg_a.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "leg_r",
        L"..\\..\\Resources\\Textures\\FireKeeper\\c1400_leg_r.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "manteau",
        L"..\\..\\Resources\\Textures\\FireKeeper\\c1400_manteau_a.dds"
        //L"D:\\DSIII_CHR\\c1400\\c1400\\c1400_manteau_a.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "manteau_r",
        L"..\\..\\Resources\\Textures\\FireKeeper\\c1400_manteau_r.dds"
        //L"D:\\DSIII_CHR\\c1400\\c1400\\c1400_manteau_r.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "skin",
        L"..\\..\\Resources\\Textures\\FireKeeper\\c1400_skin_a.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "skin_r",
        L"..\\..\\Resources\\Textures\\FireKeeper\\c1400_skin_r.dds"
    );
    mEngine.GetScene()->AddTexture
    (
        "unknown",
        L"D:\\DSIII_CHR\\c1400\\c1400\\c1400_v.dds"
    );

    //mTexMap[0] = "hair";
    //mTexMap[1] = "skin";
    //mTexMap[2] = "head"; // Í·¹Ú
    //mTexMap[3] = "skin";
    //mTexMap[4] = "body";
    //mTexMap[5] = "manteau";
    //mTexMap[6] = "body";
    //mTexMap[7] = "body";
    //mTexMap[8] = "leg";
    //mTexMap[9] = "body";
    //mTexMap[10] = "leg";
    //mTexMap[11] = "manteau";
    //mTexMap[12] = "body";
    //mTexMap[13] = "manteau";
    //mTexMap[14] = "manteau";
    //mTexMap[15] = "manteau";
    //mTexMap[16] = "body";
    //mTexMap[17] = "hair";

    mTexMap[0] = "";
    mTexMap[1] = "body";
    mTexMap[2] = "head"; // Í·¹Ú
    mTexMap[3] = "skin";
    mTexMap[4] = "skin";
    mTexMap[5] = "body";
    mTexMap[6] = "body";
    mTexMap[7] = "body";
    mTexMap[8] = "body";
    mTexMap[9] = "body";
    mTexMap[10] = "skin";
    mTexMap[11] = "leg";
    mTexMap[12] = "body";
    mTexMap[13] = "manteau";
    mTexMap[14] = "manteau";
    mTexMap[15] = "body";
    mTexMap[16] = "arm";
    mTexMap[17] = "hair";
}

void ViewerApp::AddPasses()
{
    mPasses.push_back("DefaultPass");
    mEngine.GetScene()->AddPass(mShaderDesc.name, mPasses[0]);
}

void ViewerApp::AddFloor()
{
    using namespace FireFlame;

    // Mesh
    GeometryGenerator geoGen;
    GeometryGenerator::MeshData grid = geoGen.CreateGrid(400.0f, 600.0f, 60, 40);

    std::vector<FireFlame::FLVertexNormalTex> vertices(grid.Vertices.size());
    for (size_t i = 0; i < grid.Vertices.size(); ++i)
    {
        vertices[i].Pos = grid.Vertices[i].Position;
        vertices[i].Normal = grid.Vertices[i].Normal;
        vertices[i].Tex = grid.Vertices[i].TexC;
    }

    std::vector<std::uint32_t> indices = grid.Indices32;

    auto& mesh = mMeshDesc["floor"];
    mesh.name = "floor";
    mesh.primitiveTopology = Primitive_Topology::TriangleList;
    mesh.indexCount = (unsigned int)indices.size();
    mesh.indexFormat = Index_Format::UINT32;
    mesh.indices = indices.data();

    mesh.vertexDataCount.push_back((unsigned int)vertices.size());
    mesh.vertexDataSize.push_back(sizeof(FLVertexNormalTex));
    mesh.vertexData.push_back(vertices.data());

    // sub meshes
    mesh.subMeshs.emplace_back("All", (UINT)indices.size());
    mEngine.GetScene()->AddPrimitive(mesh);

    // material
    auto& floor = mMaterials["floor"];
    floor.Name = "floor";
    floor.DiffuseAlbedo = { 0.2f, 0.2f, 0.2f, 1.0f };
    floor.FresnelR0 = { 0.6f,0.6f,0.6f };
    floor.Roughness = 0.3f;
    floor.UseTexture = 0;
    mEngine.GetScene()->AddMaterial
    (
        floor.Name,
        mShaderDesc.name, "",
        sizeof(MaterialConstants), &floor
    );

    // add render item
    using namespace DirectX;
    FireFlame::stRenderItemDesc RItem("floor", mMeshDesc["floor"].subMeshs[0]);
    RItem.mat = "floor";
    XMFLOAT4X4 trans[2];
    XMStoreFloat4x4
    (
        &trans[0],
        XMMatrixIdentity()
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
        mMeshDesc["floor"].name,
        mShaderDesc.name,
        "default",
        RItem
    );
}

void ViewerApp::AddHKXModel(size_t part, bool reverseNormal)
{
    using namespace FireFlame;

    // Mesh
    std::vector<FireFlame::FLVertexNormalTex> vertices;
    auto& rawUVs = flverloader.get_uvs();
    auto& rawVertices = flverloader.get_vertices();
    auto& rawIndices = flverloader.get_indices();

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

    std::string meshName = "model_" + std::to_string(part);
    auto& mesh = mMeshDesc[meshName];
    mesh.name = meshName;
    mesh.primitiveTopology = Primitive_Topology::TriangleList;
    mesh.indexCount = (unsigned int)indices.size();
    mesh.indexFormat = Index_Format::UINT32;
    mesh.indices = indices.data();

    mesh.vertexDataCount.push_back((unsigned int)vertices.size());
    mesh.vertexDataSize.push_back(sizeof(FLVertexNormalTex));
    mesh.vertexData.push_back(vertices.data());

    // sub meshes
    mesh.subMeshs.emplace_back("All", (UINT)indices.size());
    mEngine.GetScene()->AddPrimitive(mesh);

    // material
    std::string matName = "mat_" + std::to_string(part);
    auto& material = mMaterials[matName];
    material.Name = matName;
    material.DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
    material.FresnelR0 = { 0.05f,0.05f,0.05f };
    material.Roughness = 1.0f;

    std::string specularTex;
    if (mTexMap[part] == "")
    {
        material.UseTexture = 2;
    }
    else
    {
        material.UseTexture = 1;
        specularTex = mTexMap[part] + "_r";
    }
    
    mEngine.GetScene()->AddMaterial
    (
    {
        material.Name,
        mShaderDesc.name,{ mTexMap[part],specularTex },
        sizeof(MaterialConstants), &material
    }
    );

    // add render item
    using namespace DirectX;
    FireFlame::stRenderItemDesc RItem(meshName, mMeshDesc[meshName].subMeshs[0]);
    RItem.mat = material.Name;
    XMFLOAT4X4 trans[2];
    XMStoreFloat4x4
    (
        &trans[0],
        XMMatrixTranspose
        (
            XMMatrixScaling(mModelScale, mModelScale, mModelScale)*
            XMMatrixTranslation(0.0f, mModelTransY, 0.0f)
        )
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
        mMeshDesc[meshName].name,
        mShaderDesc.name,
        "default",
        RItem
    );
}

void ViewerApp::DecideScaleAndTrans()
{
    std::vector<FireFlame::FLVertexTex> vertices;
   
    auto& rawUVs = flverloader.get_uvs();
    auto& rawVertices = flverloader.get_vertices();
    auto& rawIndices = flverloader.get_indices();

    for (size_t part = 0; part < rawVertices.size(); part++)
    {
        for (size_t i = 0; i < rawVertices[part].size(); i++)
        {
            vertices.emplace_back
            (
                rawVertices[part][i].x, rawVertices[part][i].y, rawVertices[part][i].z,
                rawUVs[part][i].u, rawUVs[part][i].v
            );
        }
    }

    // decide scale
    float maxX = 0.f, maxY = 0.f, maxZ = 0.f;
    float minY = (std::numeric_limits<float>::max)();
    for (size_t i = 0; i < vertices.size(); ++i)
    {
        if (std::abs(vertices[i].Pos.x) > maxX)
            maxX = std::abs(vertices[i].Pos.x);
        if (std::abs(vertices[i].Pos.y) > maxY)
            maxY = std::abs(vertices[i].Pos.y);
        if (std::abs(vertices[i].Pos.z) > maxZ)
            maxZ = std::abs(vertices[i].Pos.z);

        if (vertices[i].Pos.y < minY)
            minY = vertices[i].Pos.y;
    }
    spdlog::get("console")->info
    (
        "mRadius:{0:f},maxX:{1:f},maxY:{2:f},maxZ:{3:f},minY:{4:f}",
        mRadius, maxX, maxY, maxZ, minY
    );
    mModelScale = mRadius / (((std::max)((std::max)(maxX, maxY), maxZ) - minY)*2.5f);
    mModelTransY = -minY*mModelScale;
}

void ViewerApp::save_parts(FireFlame::FLVERLoader& loader)
{
    std::vector<std::vector<FireFlame::FLVertexTex>> vertices;
    auto& rawUVs = loader.get_uvs();
    auto& rawVertices = loader.get_vertices();
    auto& rawIndices = loader.get_indices();
    {
        /*for (size_t i = 0; i < rawIndices.size(); i++)
        {
        std::ofstream out(std::string("D:\\rawIndices") + "_" + std::to_string(i) + ".txt");
        for (size_t j = 0; j < rawIndices[i].size(); j += 3)
        {
        out << "[" << rawIndices[i][j] << "," <<
        rawIndices[i][j + 1] << "," <<
        rawIndices[i][j + 2] << "]" << "\n";
        }
        }*/
    }
    vertices.resize(rawVertices.size());
    for (size_t part = 0; part < rawVertices.size(); part++)
    {
        vertices[part].reserve(rawVertices[part].size());
        for (size_t i = 0; i < rawVertices[part].size(); i++)
        {
            vertices[part].emplace_back
            (
                rawVertices[part][i].x, rawVertices[part][i].y, rawVertices[part][i].z,
                rawUVs[part][i].u, rawUVs[part][i].v
            );
        }
    }

    auto report = [](float percent) { printf("Save Ply Data:%.2lf%%\r", percent); };
    for (size_t i = 0; i < rawIndices.size(); i++)
    {
        std::string filename;
        filename += "D:\\test_part";
        filename += std::to_string(i);
        filename += ".ply";
        FireFlame::PLYLoader::Save(filename, vertices[i], rawIndices[i], report);
        std::cout << "Save To " << filename << " Completed!" << std::endl;
    }
}

void ViewerApp::save_whole(FireFlame::FLVERLoader& loader)
{
    std::vector<FireFlame::FLVertexTex> vertices;
    std::vector<std::uint32_t>          indices;

    auto& rawUVs = loader.get_uvs();
    auto& rawVertices = loader.get_vertices();
    auto& rawIndices = loader.get_indices();

    for (size_t part = 0; part < rawVertices.size(); part++)
    {
        for (size_t i = 0; i < rawVertices[part].size(); i++)
        {
            vertices.emplace_back
            (
                rawVertices[part][i].x, rawVertices[part][i].y, rawVertices[part][i].z,
                rawUVs[part][i].u, rawUVs[part][i].v
            );
        }
    }

    std::uint32_t offset = 0;
    for (size_t part = 0; part < rawIndices.size(); part++)
    {
        if (part > 0) offset += (std::uint32_t)rawVertices[part - 1].size();
        for (size_t i = 0; i < rawIndices[part].size(); i++)
        {
            indices.push_back(rawIndices[part][i] + offset);
        }
    }

    auto report = [](float percent) { printf("Save Ply Data:%.2lf%%\r", percent); };

    std::string filename("D:\\test_whole.ply");
    FireFlame::PLYLoader::Save(filename, vertices, indices, report);
    std::cout << "Save To " << filename << " Completed!" << std::endl;
}

void ViewerApp::OnKeyUp(WPARAM wParam, LPARAM lParam)
{
    if (wParam == 'M')
    {
        auto part_num = flverloader.get_part_count();
        std::string name("mat_");
        for (size_t i = 0; i < part_num; i++)
        {
            auto& mat = mMaterials[name + std::to_string(i)];
            mat.UseSpecularMap = 1;
            mEngine.GetScene()->UpdateMaterialCBData
            (
                name+std::to_string(i),sizeof(MaterialConstants),&mat
            );
        }
    }else if (wParam == 'N')
    {
        auto part_num = flverloader.get_part_count();
        std::string name("mat_");
        for (size_t i = 0; i < part_num; i++)
        {
            auto& mat = mMaterials[name + std::to_string(i)];
            mat.UseSpecularMap = 0;
            mEngine.GetScene()->UpdateMaterialCBData
            (
                name + std::to_string(i), sizeof(MaterialConstants), &mat
            );
        }
    }
}