#include "Game.h"

Game::Game(FireFlame::Engine& engine) :mEngine(engine) {

}

void Game::Update(float time_elapsed) {
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

    DirectX::XMMATRIX world = XMLoadFloat4x4(&mWorld);
    DirectX::XMMATRIX proj = XMLoadFloat4x4(&mProj);
    DirectX::XMMATRIX worldViewProj = world*view*proj;

    // Update the constant buffer with the latest worldViewProj matrix.
    ObjectConstants objConstants;
    DirectX::XMStoreFloat4x4(&objConstants.WorldViewProj, XMMatrixTranspose(worldViewProj));
    //mObjectCB->CopyData(0, objConstants);
    mEngine.GetScene()->UpdateShaderCBData(mCurrShader, 0, objConstants);
}