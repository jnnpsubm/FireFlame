#pragma once
#include "PointList.h"
#include "FireFlameHeader.h"
#include <DirectXMath.h>

struct ShaderConsts {
    DirectX::XMFLOAT4X4 WorldViewProj = FireFlame::Matrix4X4();
};

class Demo
{
public:
    Demo(FireFlame::Engine& engine);
    ~Demo();

    const FireFlame::stShaderDescription& GetShaderDesc() const { return mShaderDesc; }
    const FireFlame::stRawMesh&           GetMeshDesc()   const { return mMeshDesc; }

    void Update(float time_elapsed);
    void OnGameWindowResized(int w, int h);
    void OnMouseDown(WPARAM btnState, int x, int y);
    void OnMouseUp(WPARAM btnState, int x, int y);
    void OnMouseMove(WPARAM btnState, int x, int y);

private:
    FireFlame::Engine& mEngine;
    PointList mPointList;

    FireFlame::stShaderDescription mShaderDesc;
    FireFlame::stRawMesh           mMeshDesc;

    DirectX::XMFLOAT4X4 mWorld = FireFlame::Matrix4X4();
    DirectX::XMFLOAT4X4 mView  = FireFlame::Matrix4X4();
    DirectX::XMFLOAT4X4 mProj  = FireFlame::Matrix4X4();

    float mTheta  = 1.5f*DirectX::XM_PI;
    float mPhi    = DirectX::XM_PIDIV4;
    float mRadius = 5.0f;

    POINT mLastMousePos;
};

