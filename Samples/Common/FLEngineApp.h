#pragma once
#include "FireFlameHeader.h"
#include <DirectXMath.h>

struct ObjectConsts 
{
    DirectX::XMFLOAT4X4 World = FireFlame::Matrix4X4();
    DirectX::XMFLOAT4X4 TexTransform = FireFlame::Matrix4X4();
};

struct MaterialConstants
{
    FireFlame::Vector4f DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
    FireFlame::Vector3f FresnelR0 = { 0.01f, 0.01f, 0.01f };
    float Roughness = 0.25f;

    // Used in texture mapping.
    FireFlame::Matrix4X4 MatTransform = FireFlame::Matrix4X4();
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

    DirectX::XMFLOAT4 AmbientLight = { 0.1f, 0.1f, 0.1f, 1.0f };

    // Indices [0, NUM_DIR_LIGHTS) are directional lights;
    // indices [NUM_DIR_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHTS) are point lights;
    // indices [NUM_DIR_LIGHTS+NUM_POINT_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHT+NUM_SPOT_LIGHTS)
    // are spot lights for a maximum of MaxLights per object.
    FireFlame::Light Lights[FireFlame::Light::MaxLights];

    float FogColor[4]{ 0.7f, 0.7f, 0.7f, 1.0f };
    float FogStart = 5.f;
    float FogRange = 150.f;
    float minHeight;
    float maxHeight;
};


class FLEngineApp
{
public:
    typedef std::vector<FireFlame::stRenderItemDesc> VecRItem;
    typedef std::vector<FireFlame::stRawMesh>        VecRawMesh;

public:
    FLEngineApp(FireFlame::Engine& engine, float cameraMinDis = 3.0f, float cameraMaxDis = 150.f);
    virtual ~FLEngineApp();

    virtual void PreInitialize() {}
    virtual void Initialize() = 0;

    const FireFlame::ShaderDescription&   GetShaderDesc()        const { return mShaderDesc; }
    const VecRawMesh&                     GetMeshDesc()          const { return mMeshDesc; }
    const VecRItem&                       GetVecRenderItemDesc() const { return mRenderItems; }

    virtual void Update(float time_elapsed);
    virtual void UpdateMainPassCB(float time_elapsed) {}

    virtual void OnGameWindowResized(int w, int h);
    virtual void OnMouseDown(WPARAM btnState, int x, int y);
    virtual void OnMouseUp(WPARAM btnState, int x, int y);
    virtual void OnMouseMove(WPARAM btnState, int x, int y);

    virtual void OnKeyUp(WPARAM wParam, LPARAM lParam);
    virtual void OnKeyboardInput(float time_elapsed);

    virtual void BuildUpVP(DirectX::XMMATRIX& m);
    virtual void UpdateCamera(float time_elapsed);

protected:
    FireFlame::Engine& mEngine;

    FireFlame::ShaderDescription   mShaderDesc;
    VecRawMesh                     mMeshDesc;
    VecRItem                       mRenderItems;
    std::vector<std::string>       mPasses;

    PassConstants       mMainPassCB;

    DirectX::XMFLOAT3   mEyePos = { 0.0f, 0.0f, 0.0f };
    DirectX::XMFLOAT4X4 mView = FireFlame::Matrix4X4();
    DirectX::XMFLOAT4X4 mProj = FireFlame::Matrix4X4();

    float mTheta = 1.5f*DirectX::XM_PI;
    float mPhi = 0.2f*DirectX::XM_PI;
    float mRadius = 15.0f;

    float mMinRadius = 3.0f;
    float mMaxRadius = 150.f;

    float mPixelStep = 0.005f;

    float mSunTheta = 1.25f*FireFlame::MathHelper::FL_PI;
    float mSunPhi = FireFlame::MathHelper::FL_PIDIV4;

    POINT mLastMousePos;
};

