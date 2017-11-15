#pragma once
#include "FireFlameHeader.h"
#include <DirectXMath.h>

struct ObjectConsts {
    DirectX::XMFLOAT4X4 WorldViewProj = FireFlame::Matrix4X4();
};

class Demo
{
public:
    Demo(FireFlame::Engine& engine, const std::string& renderItemName);
    virtual ~Demo();

    const FireFlame::stShaderDescription& GetShaderDesc()     const { return mShaderDesc; }
    const FireFlame::stRawMesh&           GetMeshDesc()       const { return mMeshDesc; }
    const FireFlame::stRenderItemDesc&    GetRenderItemDesc() const { return mRenderItem; }

    virtual void Update(float time_elapsed);
    virtual void OnGameWindowResized(int w, int h);
    virtual void OnMouseDown(WPARAM btnState, int x, int y);
    virtual void OnMouseUp(WPARAM btnState, int x, int y);
    virtual void OnMouseMove(WPARAM btnState, int x, int y);

    virtual void OnKeyUp(WPARAM wParam, LPARAM lParam) {}

    virtual void BuildUpWVP(DirectX::XMMATRIX& m);

protected:
    FireFlame::Engine& mEngine;

    FireFlame::stShaderDescription mShaderDesc;
    FireFlame::stRawMesh           mMeshDesc;
    FireFlame::stRenderItemDesc    mRenderItem;

    DirectX::XMFLOAT4X4 mWorld = FireFlame::Matrix4X4();
    DirectX::XMFLOAT4X4 mView  = FireFlame::Matrix4X4();
    DirectX::XMFLOAT4X4 mProj  = FireFlame::Matrix4X4();

    float mTheta  = 1.5f*DirectX::XM_PI;
    float mPhi    = DirectX::XM_PIDIV4;
    float mRadius = 5.0f;

    POINT mLastMousePos;
};

