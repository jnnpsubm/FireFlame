#pragma once
#include <DirectXMath.h>
#include "FireFlameHeader.h"

struct ObjectConstants
{
    DirectX::XMFLOAT4X4 WorldViewProj = FireFlame::Matrix4X4();
};

class Game {
public:
    Game(FireFlame::Engine& engine);

    void Update(float time_elapsed);
    void UseShader(const std::string& name) {
        mCurrShader = name;
    }

    void OnGameWindowResized(int w, int h);

private:
    FireFlame::Engine& mEngine;

    std::string mCurrShader;

    DirectX::XMFLOAT4X4 mWorld = FireFlame::Matrix4X4();
    DirectX::XMFLOAT4X4 mView  = FireFlame::Matrix4X4();
    DirectX::XMFLOAT4X4 mProj  = FireFlame::Matrix4X4();

    float mTheta  = 1.5f*DirectX::XM_PI;
    float mPhi    = DirectX::XM_PIDIV4;
    float mRadius = 5.0f;

    POINT mLastMousePos;
};
