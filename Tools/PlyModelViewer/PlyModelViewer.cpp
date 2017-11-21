#include "PlyModelViewer.h"
#include <string>

PlyModelViewer::PlyModelViewer(FireFlame::Engine& engine) :
    mEngine(engine)
{}
PlyModelViewer::~PlyModelViewer() {}

void PlyModelViewer::Initialize(const char* fileName)
{
    AddShaders();
    AddMaterials();
    AddFloorMesh();
    AddPlyMesh(fileName);
    AddRenderItems();

    mPasses.push_back("DefaultPass");
    mEngine.GetScene()->AddPass(mShaderDesc.name, mPasses[0]);
}

void PlyModelViewer::AddRenderItems()
{
    using namespace DirectX;

    auto darkMat = mMaterials["dark"];
    for (const auto& mesh : mMeshDesc)
    {
        FireFlame::stRenderItemDesc RItem(mesh.name, mesh.subMeshs[0]);
        RItem.mat = darkMat.name;
        XMFLOAT4X4 worldTrans = FireFlame::Matrix4X4();
        RItem.dataLen = sizeof(XMFLOAT4X4);
        RItem.data = &worldTrans;
        mRenderItems.emplace_back(RItem);
        mEngine.GetScene()->AddRenderItem
        (
            mesh.name,
            mShaderDesc.name,
            RItem
        );
    }
}

void PlyModelViewer::AddFloorMesh()
{
    using namespace FireFlame;

    GeometryGenerator geoGen;
    GeometryGenerator::MeshData grid = geoGen.CreateGrid(400.0f, 600.0f, 60, 40);

    std::vector<FireFlame::FLVertexNormal> vertices(grid.Vertices.size());
    for (size_t i = 0; i < grid.Vertices.size(); ++i)
    {
        vertices[i].Pos = grid.Vertices[i].Position;
        vertices[i].Normal = grid.Vertices[i].Normal;
    }

    std::vector<std::uint32_t> indices = grid.Indices32;

    mMeshDesc.emplace_back();
    mMeshDesc.back().name = "Floor";
    mMeshDesc.back().primitiveTopology = Primitive_Topology::TriangleList;
    mMeshDesc.back().indexCount = (unsigned int)indices.size();
    mMeshDesc.back().indexFormat = Index_Format::UINT32;
    mMeshDesc.back().indices = indices.data();

    mMeshDesc.back().vertexDataCount.push_back((unsigned int)vertices.size());
    mMeshDesc.back().vertexDataSize.push_back(sizeof(FLVertexNormal));
    mMeshDesc.back().vertexData.push_back(vertices.data());

    // sub meshes
    mMeshDesc.back().subMeshs.emplace_back("All", (UINT)indices.size());
    mEngine.GetScene()->AddPrimitive(mMeshDesc.back());
}

void PlyModelViewer::AddPlyMesh(const char* fileName)
{
    std::vector<FireFlame::FLVertexNormal> vertices;
    std::vector<std::uint32_t>             indices;
    if (FireFlame::PLYLoader::Load(fileName, vertices, indices))
    {
        mMeshDesc.emplace_back();
        mMeshDesc.back().name = fileName;
        mMeshDesc.back().indexCount = (unsigned int)indices.size();
        mMeshDesc.back().indexFormat = FireFlame::Index_Format::UINT32;
        mMeshDesc.back().indices = indices.data();

        mMeshDesc.back().vertexDataCount.push_back((unsigned int)vertices.size());
        mMeshDesc.back().vertexDataSize.push_back(sizeof(FireFlame::FLVertexNormal));
        mMeshDesc.back().vertexData.push_back(vertices.data());

        // sub meshes
        mMeshDesc.back().subMeshs.emplace_back("All", (UINT)indices.size());
        mEngine.GetScene()->AddPrimitive(mMeshDesc.back());

        std::cout << "vertex count:" << vertices.size() << std::endl;
        std::cout << "face count:" << indices.size() / 3 << std::endl;

        // decide camera position
        /*float maxX = 0.f, maxY = 0.f, maxZ = 0.f;
        for (size_t i = 0; i < vertices.size(); ++i)
        {
            if (std::abs(vertices[i].Pos.x) > maxX)
                maxX = std::abs(vertices[i].Pos.x);
            if (std::abs(vertices[i].Pos.y) > maxY)
                maxY = std::abs(vertices[i].Pos.y);
            if (std::abs(vertices[i].Pos.z) > maxZ)
                maxZ = std::abs(vertices[i].Pos.z);
        }
        mRadius = FireFlame::Vector3f(maxX, maxY, maxZ).Length();
        mRadius += mRadius / 5.f;
        mMinRadius = mRadius;
        mMaxRadius = mMinRadius*50.f;*/
    }
}

void PlyModelViewer::AddMaterials()
{
    auto& darkMat = mMaterials["dark"];
    darkMat.name = "dark";
    darkMat.DiffuseAlbedo = { 0.2f, 0.2f, 0.2f, 1.0f };
    darkMat.FresnelR0 = { 0.6f,0.6f,0.6f };
    darkMat.Roughness = 0.3f;
    mEngine.GetScene()->AddMaterial("dark", mShaderDesc.name, sizeof(MaterialConstants), &darkMat);
}

void PlyModelViewer::AddShaders()
{
    using namespace FireFlame;
    mShaderDesc.name = "PlyModelViewer";
    mShaderDesc.objCBSize = sizeof(ObjectConsts);
    mShaderDesc.passCBSize = sizeof(PassConstants);
    mShaderDesc.materialCBSize = sizeof(MaterialConstants);
    mShaderDesc.materialRegister = 1;
    mShaderDesc.passRegister = 2;
    mShaderDesc.AddVertexInput("POSITION", FireFlame::VERTEX_FORMAT_FLOAT3);
    mShaderDesc.AddVertexInput("NORMAL", FireFlame::VERTEX_FORMAT_FLOAT3);
    mShaderDesc.AddShaderStage(L"Shaders\\ViewerShader.hlsl", Shader_Type::VS, "VS", "vs_5_0");
    mShaderDesc.AddShaderStage(L"Shaders\\ViewerShader.hlsl", Shader_Type::PS, "PS", "ps_5_0");
    mEngine.GetScene()->AddShader(mShaderDesc);
}

void PlyModelViewer::OnGameWindowResized(int w, int h) {
    // The window resized, so update the aspect ratio and recompute the projection matrix.
    DirectX::XMMATRIX P = DirectX::XMMatrixPerspectiveFovLH
    (
        0.25f*DirectX::XMVectorGetX(DirectX::g_XMPi),
        (float)w / h, 1.0f, 1000.0f
    );
    DirectX::XMStoreFloat4x4(&mProj, P);
}
void PlyModelViewer::Update(float time_elapsed) {
    using namespace DirectX;

    OnKeyboardInput(time_elapsed);

    UpdateCamera(time_elapsed);

    // pass constants
    XMMATRIX view = XMLoadFloat4x4(&mView);
    XMMATRIX proj = XMLoadFloat4x4(&mProj);

    XMMATRIX viewProj = XMMatrixMultiply(view, proj);
    XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
    XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
    XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);

    XMStoreFloat4x4(&mMainPassCB.View, XMMatrixTranspose(view));
    XMStoreFloat4x4(&mMainPassCB.InvView, XMMatrixTranspose(invView));
    XMStoreFloat4x4(&mMainPassCB.Proj, XMMatrixTranspose(proj));
    XMStoreFloat4x4(&mMainPassCB.InvProj, XMMatrixTranspose(invProj));
    XMStoreFloat4x4(&mMainPassCB.ViewProj, XMMatrixTranspose(viewProj));
    XMStoreFloat4x4(&mMainPassCB.InvViewProj, XMMatrixTranspose(invViewProj));
    mMainPassCB.EyePosW = mEyePos;
    float clientWidth = (float)mEngine.GetWindow()->ClientWidth();
    float clientHeight = (float)mEngine.GetWindow()->ClientHeight();
    mMainPassCB.RenderTargetSize = XMFLOAT2(clientWidth, clientHeight);
    mMainPassCB.InvRenderTargetSize = XMFLOAT2(1.0f / clientWidth, 1.0f / clientHeight);
    mMainPassCB.NearZ = 1.0f;
    mMainPassCB.FarZ = 1000.0f;
    mMainPassCB.TotalTime = mEngine.TotalTime();
    mMainPassCB.DeltaTime = mEngine.DeltaTime();

    mMainPassCB.AmbientLight = { 0.25f, 0.25f, 0.35f, 1.0f };
    
    // above light
    mMainPassCB.Lights[0].Direction = -FireFlame::MathHelper::SphericalToCartesian
    (
        1.0f,
        0.f,
        0.f
    );
    mMainPassCB.Lights[0].Strength = { 0.8f, 0.8f, 0.8f };

    // right light
    mMainPassCB.Lights[1].Direction = -FireFlame::MathHelper::SphericalToCartesian
    (
        1.0f,
        0.f,
        FireFlame::MathHelper::FL_PIDIV4
    );
    mMainPassCB.Lights[1].Strength = { 0.6f, 0.3f, 0.2f };

    // left
    mMainPassCB.Lights[2].Direction = -FireFlame::MathHelper::SphericalToCartesian
    (
        1.0f,
        FireFlame::MathHelper::FL_PI,
        FireFlame::MathHelper::FL_PIDIV4
    );
    mMainPassCB.Lights[2].Strength = { 0.6f, 0.3f, 0.2f };

    mMainPassCB.Lights[3].Direction = -FireFlame::MathHelper::SphericalToCartesian
    (
        1.0f,
        FireFlame::MathHelper::FL_PIDIV2,
        FireFlame::MathHelper::FL_PIDIV2
    );
    mMainPassCB.Lights[3].Strength = { 0.3f, 0.5f, 0.5f };

    FireFlame::Vector3f sum(0.f, 0.f, 0.f);
    for (size_t i = 0; i < 4; ++i)
    {
        sum += mMainPassCB.Lights[i].Strength;
    }
    for (size_t i = 0; i < 4; ++i)
    {
        mMainPassCB.Lights[i].Strength = mMainPassCB.Lights[i].Strength / sum;
    }

    if (mPasses.size())
        mEngine.GetScene()->UpdatePassCBData(mPasses[0], sizeof(PassConstants), &mMainPassCB);
}

void PlyModelViewer::UpdateCamera(float time_elapsed)
{
    // Convert Spherical to Cartesian coordinates.
    mEyePos.x = mRadius*sinf(mPhi)*cosf(mTheta);
    mEyePos.z = mRadius*sinf(mPhi)*sinf(mTheta);
    mEyePos.y = mRadius*cosf(mPhi);

    // Build the view matrix.
    DirectX::XMVECTOR pos = DirectX::XMVectorSet(mEyePos.x, mEyePos.y, mEyePos.z, 1.0f);
    DirectX::XMVECTOR target = DirectX::XMVectorZero();
    DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(pos, target, up);
    DirectX::XMStoreFloat4x4(&mView, view);
}

void PlyModelViewer::OnMouseDown(WPARAM btnState, int x, int y) {
    mLastMousePos.x = x;
    mLastMousePos.y = y;
    SetCapture(mEngine.GetWindow()->MainWnd());
}
void PlyModelViewer::OnMouseUp(WPARAM btnState, int x, int y) {
    ReleaseCapture();
}
void PlyModelViewer::OnMouseMove(WPARAM btnState, int x, int y) {
    if ((btnState & MK_LBUTTON) != 0) {
        // Make each pixel correspond to a quarter of a degree.
        float dx = DirectX::XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
        float dy = DirectX::XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

        // Update angles based on input to orbit camera around box.
        mTheta += dx;
        mPhi += dy;

        // Restrict the angle mPhi.

        mPhi = FireFlame::MathHelper::Clamp(mPhi, 0.1f, DirectX::XMVectorGetX(DirectX::g_XMPi) - 0.1f);
    }
    else if ((btnState & MK_RBUTTON) != 0)
    {
        // Make each pixel correspond to 0.005 unit in the scene.
        float dx = mPixelStep*static_cast<float>(x - mLastMousePos.x);
        float dy = mPixelStep*static_cast<float>(y - mLastMousePos.y);

        // Update the camera radius based on input.
        mRadius += dx - dy;

        // Restrict the radius.
        mRadius = FireFlame::MathHelper::Clamp(mRadius, mMinRadius, mMaxRadius);
    }

    mLastMousePos.x = x;
    mLastMousePos.y = y;
}

void PlyModelViewer::OnKeyboardInput(float time_elapsed)
{
    if (GetAsyncKeyState(VK_LEFT) & 0x8000)
        mSunTheta -= 1.0f*time_elapsed;

    if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
        mSunTheta += 1.0f*time_elapsed;

    if (GetAsyncKeyState(VK_UP) & 0x8000)
        mSunPhi -= 1.0f*time_elapsed;

    if (GetAsyncKeyState(VK_DOWN) & 0x8000)
        mSunPhi += 1.0f*time_elapsed;
    mSunPhi = FireFlame::MathHelper::Clamp(mSunPhi, 0.1f, FireFlame::MathHelper::FL_PIDIV2);

    if (GetAsyncKeyState('W') & 0x8000) 
    {
        if (mEngine.GetFillMode() == FireFlame::Fill_Mode::Solid) 
        {
            mEngine.SetFillMode(FireFlame::Fill_Mode::Wireframe);
        }
        else 
        {
            mEngine.SetFillMode(FireFlame::Fill_Mode::Solid);
        }
    }
    else if (GetAsyncKeyState('N') & 0x8000)
    {
        mEngine.SetCullMode(FireFlame::Cull_Mode::None);
    }
    else if (GetAsyncKeyState('F') & 0x8000)
    {
        mEngine.SetCullMode(FireFlame::Cull_Mode::Front);
    }
    else if (GetAsyncKeyState('B') & 0x8000)
    {
        mEngine.SetCullMode(FireFlame::Cull_Mode::Back);
    }
}