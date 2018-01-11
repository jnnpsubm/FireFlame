#include "FLEngineApp4.h"
#include <string>

FLEngineApp4::FLEngineApp4(FireFlame::Engine& engine) :
    mEngine(engine)
{
    mCamera.SetPosition(0.0f, 2.0f, -15.0f);
}
FLEngineApp4::~FLEngineApp4() {}

void FLEngineApp4::OnGameWindowResized(int w, int h) {
    float aspectRatio = static_cast<float>(w) / h;
    mCamera.SetLens(0.25f*FireFlame::MathHelper::FL_PI, aspectRatio, 1.0f, 1000.0f);
}
void FLEngineApp4::Update(float time_elapsed) 
{
    OnKeyboardInput(time_elapsed);
    UpdateMainPassCB(time_elapsed);
}

void FLEngineApp4::UpdateMainPassCB(float time_elapsed)
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
    passCB.Lights[0].Direction = { 0.57735f, -0.57735f, -0.57735f };
    passCB.Lights[0].Strength = { 0.6f, 0.6f, 0.6f };
    passCB.Lights[1].Direction = { -0.57735f, -0.57735f, 0.57735f };
    passCB.Lights[1].Strength = { 0.3f, 0.3f, 0.3f };
    passCB.Lights[2].Direction = { 0.0f, -0.707f, -0.707f };
    passCB.Lights[2].Strength = { 0.15f, 0.15f, 0.15f };

    mEngine.GetScene()->UpdateShaderPassCBData(mShaderDescs["main"].name, sizeof(PassConstants), &passCB);
}

void FLEngineApp4::OnMouseDown(WPARAM btnState, int x, int y) {
    mLastMousePos.x = x;
    mLastMousePos.y = y;
    SetCapture(mEngine.GetWindow()->MainWnd());
}
void FLEngineApp4::OnMouseUp(WPARAM btnState, int x, int y) {
    ReleaseCapture();
}
void FLEngineApp4::OnMouseMove(WPARAM btnState, int x, int y) {
    if ((btnState & MK_LBUTTON) != 0){
        // Make each pixel correspond to a quarter of a degree.
        float dx = FireFlame::MathHelper::ToRadius(0.25f*static_cast<float>(x - mLastMousePos.x));
        float dy = FireFlame::MathHelper::ToRadius(0.25f*static_cast<float>(y - mLastMousePos.y));
        mCamera.Pitch(dy);
        mCamera.RotateY(dx);
    }
    mLastMousePos.x = x;
    mLastMousePos.y = y;
}

void FLEngineApp4::OnKeyUp(WPARAM wParam, LPARAM lParam) {
    
}

void FLEngineApp4::OnKeyboardInput(float time_elapsed)
{
    const float dt = mEngine.DeltaTime();
    if (GetAsyncKeyState('W') & 0x8000)
        mCamera.Walk(10.0f*dt);
    if (GetAsyncKeyState('S') & 0x8000)
        mCamera.Walk(-10.0f*dt);
    if (GetAsyncKeyState('A') & 0x8000)
        mCamera.Strafe(-10.0f*dt);
    if (GetAsyncKeyState('D') & 0x8000)
        mCamera.Strafe(10.0f*dt);
    if (GetAsyncKeyState(VK_LEFT) & 0x8000)
        mCamera.Roll(10.0f*dt);
    if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
        mCamera.Roll(-10.0f*dt);
    if (GetAsyncKeyState(VK_SPACE) & 0x8000)
    {
        if (GetAsyncKeyState(VK_LSHIFT) & 0x8000)
            mCamera.Descend(5.f*dt);
        else
            mCamera.Ascend(5.f*dt);
    }
    mCamera.UpdateViewMatrix();

    if (GetAsyncKeyState(VK_F8) & 0x8000)
        mEngine.GetScene()->PrintScene();
}