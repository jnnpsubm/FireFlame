#pragma once
#include <DirectXMath.h>
#include "FireFlameHeader.h"

struct ObjectConstants
{
    ObjectConstants() = default;
    DirectX::XMFLOAT4X4 WorldViewProj = FireFlame::Matrix4X4();
    int gMaxIters = 1000;
    double uTS = 1.;       // texture coordinate scaling
    float uCS = 3.f;       // color scaling
    double uS0 = 0.01;      // starting texture value in S
    double uT0 = 0.01;      // starting texture value in T
    double uLimit = 10.0; // how large before stop iterations
    DirectX::XMFLOAT3 convergeColor = DirectX::XMFLOAT3(0.5f, 0.9f, 0.8f);
    DirectX::XMFLOAT3 divergeColor1 = DirectX::XMFLOAT3(0.8f, 0.3f, 0.0f);
    DirectX::XMFLOAT3 divergeColor2 = DirectX::XMFLOAT3(0.9f, 0.6f, 0.4f);
};

class Game {
public:
    Game(FireFlame::Engine& engine);

    void Update(float time_elapsed);
    void SetRenderItem(const std::string& name) {
        mRenderItem = name;
    }
    void SetMainWnd(HWND hwnd) { mhMainWnd = hwnd; }

    void OnGameWindowResized(int w, int h);
    void OnMouseDown(WPARAM btnState, int x, int y);
    void OnMouseUp(WPARAM btnState, int x, int y);
    void OnMouseMove(WPARAM btnState, int x, int y);
    void OnKeyUp(WPARAM wParam, LPARAM lParam);

private:
    FireFlame::Engine& mEngine;

    HWND        mhMainWnd;
    std::string mRenderItem;

    ObjectConstants mShaderConstants;

    DirectX::XMFLOAT4X4 mWorld = FireFlame::Matrix4X4();
    DirectX::XMFLOAT4X4 mView  = FireFlame::Matrix4X4();
    DirectX::XMFLOAT4X4 mProj  = FireFlame::Matrix4X4();

    float mTheta  = 1.5f*DirectX::XM_PI;
    float mPhi    = DirectX::XM_PIDIV4;
    float mRadius = 5.0f;

    POINT mLastMousePos;
};
