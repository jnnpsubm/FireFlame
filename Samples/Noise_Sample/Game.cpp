#include "Game.h"
#include <fstream>

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
    DirectX::XMStoreFloat4x4(&mShaderConstants.WorldViewProj, XMMatrixTranspose(worldViewProj));
    //mObjectCB->CopyData(0, objConstants);

    

    mEngine.GetScene()->UpdateShaderCBData(mCurrShader, 0, mShaderConstants);
}
void Game::OnMouseDown(WPARAM btnState, int x, int y) {
    mLastMousePos.x = x;
    mLastMousePos.y = y;

    SetCapture(mhMainWnd);
}
void Game::OnMouseUp(WPARAM btnState, int x, int y) {
    ReleaseCapture();
}
void Game::OnMouseMove(WPARAM btnState, int x, int y) {
    if ((btnState & MK_LBUTTON) != 0){
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
        float dx = 0.005f*static_cast<float>(x - mLastMousePos.x);
        float dy = 0.005f*static_cast<float>(y - mLastMousePos.y);

        // Update the camera radius based on input.
        mRadius += dx - dy;

        // Restrict the radius.
        mRadius = FireFlame::MathHelper::Clamp(mRadius, 1.5f, 150.0f);
    }

    mLastMousePos.x = x;
    mLastMousePos.y = y;
}

void Game::OnKeyUp(WPARAM wParam, LPARAM lParam) {
   
}
void Game::OnKeyDown(WPARAM wParam, LPARAM lParam) {
	std::wstring strDebug(L"================");
    if ((int)wParam == 'A')
    {
        mShaderConstants.TexScale += 1.0f;
		strDebug += L"TexScale = " + std::to_wstring(mShaderConstants.TexScale);
    }
    else if ((int)wParam == 'J')
    {
        mShaderConstants.TexScale -= 1.0f;
		strDebug += L"TexScale = " + std::to_wstring(mShaderConstants.TexScale);
    }
    else if ((int)wParam == 'B')
    {
        mShaderConstants.Turbulence = !mShaderConstants.Turbulence;
		strDebug += L"Turbulence Mode = " + std::to_wstring(mShaderConstants.Turbulence);
    }
    else if ((int)wParam == 'C')
    {
        mShaderConstants.Turbulence2 = !mShaderConstants.Turbulence2;
		strDebug += L"Turbulence Modulate Mode = " + std::to_wstring(mShaderConstants.Turbulence2);
    }
    else if ((int)wParam == 'M')
    {
        mShaderConstants.ManualOctave = !mShaderConstants.ManualOctave;
		strDebug += L"ManualOctave Mode = " + std::to_wstring(mShaderConstants.ManualOctave);
    }
    else if ((int)wParam == 'O')
    {
		if (mShaderConstants.Octave <= 0) mShaderConstants.Octave = 1;
        mShaderConstants.Octave *= 2;
		strDebug += L"Octave = " + std::to_wstring(mShaderConstants.Octave);
    }
	else if ((int)wParam == 'P')
	{
		if (mShaderConstants.Octave > 0) mShaderConstants.Octave /= 2;
		strDebug += L"Octave = " + std::to_wstring(mShaderConstants.Octave);
	}
    else if ((int)wParam == 'E')
    {
        mShaderConstants.Erosion = !mShaderConstants.Erosion;
    }
	strDebug += L"================\n";
	OutputDebugString(strDebug.c_str());
}