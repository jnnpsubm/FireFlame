#pragma once
#include "FireFlameHeader.h"
#include <DirectXMath.h>

class Demo
{
public:
    Demo(FireFlame::Engine& engine);
    virtual ~Demo();

    const FireFlame::stShaderDescription& GetShaderDesc() const { return mShaderDesc; }
    const FireFlame::stRawMesh&           GetMeshDesc()   const { return mMeshDesc; }

    virtual void Update(float time_elapsed);
    virtual void OnGameWindowResized(int w, int h);
    virtual void OnMouseDown(WPARAM btnState, int x, int y);
    virtual void OnMouseUp(WPARAM btnState, int x, int y);
    virtual void OnMouseMove(WPARAM btnState, int x, int y);

protected:
    FireFlame::Engine& mEngine;

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

