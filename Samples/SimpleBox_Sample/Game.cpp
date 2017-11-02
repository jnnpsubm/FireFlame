#include "Game.h"

Game::Game(FireFlame::Engine& engine) :mEngine(engine) {

}
void Game::OnGameWindowResized(int w, int h) {
    // The window resized, so update the aspect ratio and recompute the projection matrix.
    DirectX::XMMATRIX P = DirectX::XMMatrixPerspectiveFovLH
    (
        0.25f*DirectX::XMVectorGetX(DirectX::g_XMPi),
        (float)w/h, 1.0f, 1000.0f
    );
    DirectX::XMStoreFloat4x4(&mProj, P);
}
void Game::Update(float time_elapsed) {
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

    DirectX::XMMATRIX world = DirectX::XMLoadFloat4x4(&mWorld);
    DirectX::XMMATRIX proj = DirectX::XMLoadFloat4x4(&mProj);
    DirectX::XMMATRIX worldViewProj = world*view*proj;

    // Update the constant buffer with the latest worldViewProj matrix.
    ObjectConstants objConstants;
    DirectX::XMStoreFloat4x4(&objConstants.WorldViewProj, XMMatrixTranspose(worldViewProj));
    //mObjectCB->CopyData(0, objConstants);
    mEngine.GetScene()->UpdateShaderCBData(mCurrShader, 0, objConstants);
}