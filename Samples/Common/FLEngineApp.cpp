#include "FLEngineApp.h"
#include <string>

FLEngineApp::FLEngineApp(FireFlame::Engine& engine, float cameraMinDis, float cameraMaxDis) :
    mEngine(engine),
    mMinRadius(cameraMinDis),
    mMaxRadius(cameraMaxDis)
{}
FLEngineApp::~FLEngineApp() {}

void FLEngineApp::OnGameWindowResized(int w, int h) {
    // The window resized, so update the aspect ratio and recompute the projection matrix.
    DirectX::XMMATRIX P = DirectX::XMMatrixPerspectiveFovLH
    (
        0.25f*DirectX::XMVectorGetX(DirectX::g_XMPi),
        (float)w / h, 1.0f, 1000.0f
    );
    DirectX::XMStoreFloat4x4(&mProj, P);
}
void FLEngineApp::Update(float time_elapsed) {
    using namespace DirectX;

    OnKeyboardInput(time_elapsed);

    UpdateCamera(time_elapsed);

    /*ObjectConsts ObjConstants;
    for (const auto& RItem : mRenderItems) {
        mEngine.GetScene()->UpdateRenderItemCBData(RItem.name, sizeof(ObjectConsts), &ObjConstants);
    }*/

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
    mMainPassCB.Lights[0].Direction = -FireFlame::MathHelper::SphericalToCartesian(1.0f, mSunTheta, mSunPhi);
    mMainPassCB.Lights[0].Strength = { 1.0f, 1.0f, 0.9f };

    UpdateMainPassCB(time_elapsed);
    if (mPasses.size())
        mEngine.GetScene()->UpdatePassCBData(mPasses[0], sizeof(PassConstants), &mMainPassCB);
}

void FLEngineApp::UpdateCamera(float time_elapsed)
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
void FLEngineApp::BuildUpVP(DirectX::XMMATRIX& m) {
    using namespace DirectX;
    // Convert Spherical to Cartesian coordinates.
    float x = mRadius*sinf(mPhi)*cosf(mTheta);
    float z = mRadius*sinf(mPhi)*sinf(mTheta);
    float y = mRadius*cosf(mPhi);

    // Build the view matrix.
    DirectX::XMVECTOR pos = DirectX::XMVectorSet(x, y, z, 1.0f);
    DirectX::XMVECTOR target = DirectX::XMVectorZero();
    DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(pos, target, up);
    DirectX::XMStoreFloat4x4(&mView, view);

    DirectX::XMMATRIX proj = DirectX::XMLoadFloat4x4(&mProj);
    m = view*proj;
}
void FLEngineApp::OnMouseDown(WPARAM btnState, int x, int y) {
    mLastMousePos.x = x;
    mLastMousePos.y = y;
    SetCapture(mEngine.GetWindow()->MainWnd());
}
void FLEngineApp::OnMouseUp(WPARAM btnState, int x, int y) {
    ReleaseCapture();
}
void FLEngineApp::OnMouseMove(WPARAM btnState, int x, int y) {
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

void FLEngineApp::OnKeyUp(WPARAM wParam, LPARAM lParam) {
    if (wParam == 'W') {
        if (mEngine.GetFillMode() == FireFlame::Fill_Mode::Solid) {
            mEngine.SetFillMode(FireFlame::Fill_Mode::Wireframe);
        }
        else {
            mEngine.SetFillMode(FireFlame::Fill_Mode::Solid);
        }
    }
    else if (wParam == 'N')
    {
        mEngine.SetCullMode(FireFlame::Cull_Mode::None);
    }
    else if (wParam == 'F')
    {
        mEngine.SetCullMode(FireFlame::Cull_Mode::Front);
    }
    else if (wParam == 'B')
    {
        mEngine.SetCullMode(FireFlame::Cull_Mode::Back);
    }
}

void FLEngineApp::OnKeyboardInput(float time_elapsed)
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

    if (GetAsyncKeyState(VK_F5) & 0x8000)
        mEngine.GetScene()->PrintScene();
}