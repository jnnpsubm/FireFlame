#include "GSCylinderApp.h"

void GSCylinderApp::PreInitialize()
{
    mEngine.SetDefaultClearColor({ 0.7f, 0.7f, 0.7f, 1.0f });
}

void GSCylinderApp::Initialize()
{
    AddShaders();
    AddPSOs();
    AddMaterials();
    AddMeshs();
    AddRenderItems();

    mPasses.push_back("DefaultPass");
    mEngine.GetScene()->AddPass(mPasses[0]);
}

void GSCylinderApp::Update(float time_elapsed)
{
    OnKeyboardInput(time_elapsed);
    UpdateCamera(time_elapsed);
    UpdateMainPassCB(time_elapsed);
}

void GSCylinderApp::UpdateMainPassCB(float time_elapsed)
{
    using namespace DirectX;

    // circle pass constants
    auto& passCB = mPassCBs["circle"];
    XMMATRIX view = XMLoadFloat4x4(&mView);
    XMMATRIX proj = XMLoadFloat4x4(&mProj);
    XMMATRIX viewProj = XMMatrixMultiply(view, proj);
    XMStoreFloat4x4(&passCB.View, XMMatrixTranspose(view));
    XMStoreFloat4x4(&passCB.Proj, XMMatrixTranspose(proj));
    XMStoreFloat4x4(&passCB.ViewProj, XMMatrixTranspose(viewProj));
    mEngine.GetScene()->UpdateShaderPassCBData(mShaderDescs["circle"].name, sizeof(PassConstantsVP), &passCB);
    
    // for cylinder
    XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
    XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
    XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);
    XMStoreFloat4x4(&passCB.InvView, XMMatrixTranspose(invView));
    XMStoreFloat4x4(&passCB.InvProj, XMMatrixTranspose(invProj));
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
    mEngine.GetScene()->UpdateShaderPassCBData(mShaderDescs["cylinder"].name, sizeof(PassConstantsLight), &passCB);
}

void GSCylinderApp::AddShaders()
{
    AddShaderCircle();
    AddShaderCylinder();
}

void GSCylinderApp::AddShaderCircle()
{
    using namespace FireFlame;

    auto& shaderDesc = mShaderDescs["circle"];
    shaderDesc.name = "circle_shader";
    shaderDesc.objCBSize = sizeof(ObjectConsts);
    shaderDesc.passCBSize = sizeof(PassConstantsVP);
    shaderDesc.objParamIndex = 1;
    shaderDesc.passParamIndex = 2;
    shaderDesc.AddVertexInput("POSITION", FireFlame::VERTEX_FORMAT_FLOAT3);
    shaderDesc.AddVertexInput("COLOR", FireFlame::VERTEX_FORMAT_FLOAT3);
    shaderDesc.AddShaderStage(L"Shaders\\circle.hlsl", Shader_Type::VS, "VS", "vs_5_0");
    shaderDesc.AddShaderStage(L"Shaders\\circle.hlsl", Shader_Type::PS, "PS", "ps_5_0");
    mEngine.GetScene()->AddShader(shaderDesc);
}

void GSCylinderApp::AddShaderCylinder()
{
    using namespace FireFlame;

    auto& shaderDesc = mShaderDescs["cylinder"];
    shaderDesc.name = "cylinder_shader";
    shaderDesc.objCBSize = sizeof(ObjectConsts);
    shaderDesc.materialCBSize = sizeof(MaterialConstants);
    shaderDesc.passCBSize = sizeof(PassConstantsLight);
    shaderDesc.objParamIndex = 1;
    shaderDesc.matParamIndex = 2;
    shaderDesc.passParamIndex = 3;
    shaderDesc.AddVertexInput("POSITION", FireFlame::VERTEX_FORMAT_FLOAT3);
    shaderDesc.AddVertexInput("COLOR", FireFlame::VERTEX_FORMAT_FLOAT3);
    shaderDesc.AddShaderStage(L"Shaders\\cylinder.hlsl", Shader_Type::VS, "VS", "vs_5_0");
    shaderDesc.AddShaderStage(L"Shaders\\cylinder.hlsl", Shader_Type::GS, "GS", "gs_5_0");
    shaderDesc.AddShaderStage(L"Shaders\\cylinder.hlsl", Shader_Type::PS, "PS", "ps_5_0");
    mEngine.GetScene()->AddShader(shaderDesc);
}

void GSCylinderApp::AddPSOs()
{
    using namespace FireFlame;

    PSODesc descCircle(mShaderDescs["circle"].name);
    descCircle.topologyType = Primitive_Topology_Type::Line;
    mEngine.GetScene()->AddPSO("circle_default", descCircle);

    PSODesc descCylinder(mShaderDescs["cylinder"].name);
    descCylinder.topologyType = Primitive_Topology_Type::Line;
    descCylinder.cullMode = Cull_Mode::None;
    mEngine.GetScene()->AddPSO("cylinder_default", descCylinder);
}

void GSCylinderApp::AddMaterials()
{
    auto& matCylinder = mMaterials["cylinder"];
    matCylinder.Name = "cylinder";
    matCylinder.DiffuseAlbedo = FireFlame::Vector4f(0.5f, 0.4f, 0.3f, 1.0f);
    matCylinder.FresnelR0 = FireFlame::Vector3f(0.8f, 0.8f, 0.8f);
    matCylinder.Roughness = 0.125f;
    mEngine.GetScene()->AddMaterial
    (
        matCylinder.Name,
        mShaderDescs["cylinder"].name,
        sizeof(MaterialConstants), &matCylinder
    );
}

void GSCylinderApp::AddMeshs()
{
    AddMeshCircle();
}

void GSCylinderApp::AddMeshCircle()
{
    using namespace FireFlame;
    
    std::array<FLVertexColorV3, 4> vertices
    {
        FLVertexColorV3(-1.0f,0.0f,-1.0f,1.0f,0.0f,0.0f),
        FLVertexColorV3(-1.0f,0.0f,+1.0f,1.0f,1.0f,0.0f),
        FLVertexColorV3(+1.0f,0.0f,+1.0f,1.0f,0.0f,1.0f),
        FLVertexColorV3(+1.0f,0.0f,-1.0f,1.0f,1.0f,1.0f)
    };
    std::array<std::uint16_t, 5> indices
    {
        0,1,2,3,0
    };

    auto& mesh = mMeshDescs["circle"];
    mesh.name = "circle";
    mesh.primitiveTopology = Primitive_Topology::LineStrip;
    mesh.indexFormat = Index_Format::UINT16;
    mesh.indexCount = (unsigned)indices.size();
    mesh.indices = indices.data();
    mesh.vertexDataSize.emplace_back((unsigned)sizeof(FLVertexColorV3));
    mesh.vertexDataCount.emplace_back((unsigned)vertices.size());
    mesh.vertexData.emplace_back(vertices.data());
    mesh.subMeshs.emplace_back("all", (unsigned)indices.size());

    mEngine.GetScene()->AddPrimitive(mesh);
}

void GSCylinderApp::AddRenderItems()
{
    AddRenderItemsCircle();
    AddRenderItemsCylinder();
}

void GSCylinderApp::AddRenderItemsCircle()
{
    using namespace DirectX;

    FireFlame::stRenderItemDesc RItem("circle01", mMeshDescs["circle"].subMeshs[0]);
    XMFLOAT4X4 trans[2];
    XMStoreFloat4x4
    (
        &trans[0],
        XMMatrixTranspose(XMMatrixTranslation(-3.0f, 0.0f, 0.0f))
    );
    XMStoreFloat4x4
    (
        &trans[1],
        XMMatrixIdentity()
    );
    RItem.dataLen = sizeof(XMFLOAT4X4)*_countof(trans);
    RItem.data = &trans[0];
    RItem.topology = FireFlame::Primitive_Topology::LineStrip;
    mEngine.GetScene()->AddRenderItem
    (
        mMeshDescs["circle"].name,
        mShaderDescs["circle"].name,
        "circle_default",
        RItem
    );
}

void GSCylinderApp::AddRenderItemsCylinder()
{
    using namespace DirectX;

    FireFlame::stRenderItemDesc RItem("cylinder01", mMeshDescs["circle"].subMeshs[0]);
    RItem.mat = "cylinder";
    XMFLOAT4X4 trans[2];
    XMStoreFloat4x4
    (
        &trans[0],
        XMMatrixTranspose(XMMatrixTranslation(+3.0f, 0.0f, 0.0f))
    );
    XMStoreFloat4x4
    (
        &trans[1],
        XMMatrixIdentity()
    );
    RItem.dataLen = sizeof(XMFLOAT4X4)*_countof(trans);
    RItem.data = &trans[0];
    RItem.topology = FireFlame::Primitive_Topology::LineStrip;
    mEngine.GetScene()->AddRenderItem
    (
        mMeshDescs["circle"].name,
        mShaderDescs["cylinder"].name,
        "cylinder_default",
        RItem
    );
}
