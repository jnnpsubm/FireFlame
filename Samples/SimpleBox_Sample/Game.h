#pragma once
#include <DirectXMath.h>
#include "FireFlameHeader.h"

struct ObjectConstants
{
    DirectX::XMFLOAT4X4 WorldViewProj = FireFlame::Matrix4X4();
};

struct PassConstants
{
    DirectX::XMFLOAT4X4 View = FireFlame::Matrix4X4();
    DirectX::XMFLOAT4X4 InvView = FireFlame::Matrix4X4();
    DirectX::XMFLOAT4X4 Proj = FireFlame::Matrix4X4();
    DirectX::XMFLOAT4X4 InvProj = FireFlame::Matrix4X4();
    DirectX::XMFLOAT4X4 ViewProj = FireFlame::Matrix4X4();
    DirectX::XMFLOAT4X4 InvViewProj = FireFlame::Matrix4X4();
    DirectX::XMFLOAT3 EyePosW = { 0.0f, 0.0f, 0.0f };
    float cbPerObjectPad1 = 0.0f;
    DirectX::XMFLOAT2 RenderTargetSize = { 0.0f, 0.0f };
    DirectX::XMFLOAT2 InvRenderTargetSize = { 0.0f, 0.0f };
    float NearZ = 0.0f;
    float FarZ = 0.0f;
    float TotalTime = 0.0f;
    float DeltaTime = 0.0f;
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

private:
    FireFlame::Engine& mEngine;

    HWND        mhMainWnd;
    std::string mRenderItem;

    DirectX::XMFLOAT4X4 mWorld = FireFlame::Matrix4X4();
    DirectX::XMFLOAT4X4 mView  = FireFlame::Matrix4X4();
    DirectX::XMFLOAT4X4 mProj  = FireFlame::Matrix4X4();

    float mTheta  = 1.5f*DirectX::XM_PI;
    float mPhi    = DirectX::XM_PIDIV4;
    float mRadius = 5.0f;

    POINT mLastMousePos;
};
