#pragma once
#include "FireFlameHeader.h"
#include <DirectXMath.h>

struct ObjectConsts 
{
    DirectX::XMFLOAT4X4 World = FireFlame::Matrix4X4();
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


class FLEngineApp
{
public:
    typedef std::vector<FireFlame::stRenderItemDesc> VecRItem;

public:
    FLEngineApp(FireFlame::Engine& engine);
    virtual ~FLEngineApp();

    virtual void Initialize() = 0;

    const FireFlame::stShaderDescription& GetShaderDesc()     const { return mShaderDesc; }
    const FireFlame::stRawMesh&           GetMeshDesc()       const { return mMeshDesc; }
    const VecRItem& GetVecRenderItemDesc()                    const { return mRenderItems; }

    virtual void Update(float time_elapsed);
    virtual void OnGameWindowResized(int w, int h);
    virtual void OnMouseDown(WPARAM btnState, int x, int y);
    virtual void OnMouseUp(WPARAM btnState, int x, int y);
    virtual void OnMouseMove(WPARAM btnState, int x, int y);

    virtual void OnKeyUp(WPARAM wParam, LPARAM lParam) {}

    virtual void BuildUpVP(DirectX::XMMATRIX& m);
    virtual void UpdateCamera(float time_elapsed);

protected:
    FireFlame::Engine& mEngine;

    FireFlame::stShaderDescription mShaderDesc;
    FireFlame::stRawMesh           mMeshDesc;
    VecRItem                       mRenderItems;
    std::vector<std::string>       mPasses;

    PassConstants       mMainPassCB;

    DirectX::XMFLOAT3   mEyePos = { 0.0f, 0.0f, 0.0f };
    DirectX::XMFLOAT4X4 mView = FireFlame::Matrix4X4();
    DirectX::XMFLOAT4X4 mProj = FireFlame::Matrix4X4();

    float mTheta = 1.5f*DirectX::XM_PI;
    float mPhi = 0.2f*DirectX::XM_PI;
    float mRadius = 15.0f;

    POINT mLastMousePos;
};

