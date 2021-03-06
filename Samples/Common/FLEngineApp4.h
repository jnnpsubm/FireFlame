#pragma once
#include "FireFlameHeader.h"
#include <DirectXMath.h>

struct ObjectConsts 
{
    DirectX::XMFLOAT4X4 World = FireFlame::Matrix4X4();
    DirectX::XMFLOAT4X4 TexTransform = FireFlame::Matrix4X4();
    unsigned            MaterialIndex = 0;
    unsigned            ObjectPad0;
    unsigned            ObjectPad1;
    unsigned            ObjectPad2;
};

struct MaterialConstants
{
    FireFlame::Vector4f DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
    FireFlame::Vector3f FresnelR0 = { 0.01f, 0.01f, 0.01f };
    float Roughness = 0.25f;

    // Used in texture mapping.
    FireFlame::Matrix4X4 MatTransform = FireFlame::Matrix4X4();

    UINT DiffuseMapIndex = 0;
    UINT UseTexture = 1;
    UINT CubeMapIndex = 0;
    UINT NormalMapIndex = 0;
};

struct NamedMaterialConstants : MaterialConstants
{
    std::string Name;
};

struct PassConstantsVP
{
    DirectX::XMFLOAT4X4 View = FireFlame::Matrix4X4();
    DirectX::XMFLOAT4X4 Proj = FireFlame::Matrix4X4();
    DirectX::XMFLOAT4X4 ViewProj = FireFlame::Matrix4X4();
};

struct PassConstantsInvVP : PassConstantsVP
{
    DirectX::XMFLOAT4X4 InvView = FireFlame::Matrix4X4();
    DirectX::XMFLOAT4X4 InvProj = FireFlame::Matrix4X4();
    DirectX::XMFLOAT4X4 InvViewProj = FireFlame::Matrix4X4();
};

struct PassConstantsBase : PassConstantsInvVP
{
    DirectX::XMFLOAT3 EyePosW = { 0.0f, 0.0f, 0.0f };
    float cbPerObjectPad1 = 0.0f;
    DirectX::XMFLOAT2 RenderTargetSize = { 0.0f, 0.0f };
    DirectX::XMFLOAT2 InvRenderTargetSize = { 0.0f, 0.0f };
    float NearZ = 0.0f;
    float FarZ = 0.0f;
    float TotalTime = 0.0f;
    float DeltaTime = 0.0f;
};

struct PassConstantsLight : PassConstantsBase
{
    DirectX::XMFLOAT4 AmbientLight = { 0.1f, 0.1f, 0.1f, 1.0f };

    // Indices [0, NUM_DIR_LIGHTS) are directional lights;
    // indices [NUM_DIR_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHTS) are point lights;
    // indices [NUM_DIR_LIGHTS+NUM_POINT_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHT+NUM_SPOT_LIGHTS)
    // are spot lights for a maximum of MaxLights per object.
    FireFlame::Light Lights[FireFlame::Light::MaxLights];
};

struct PassConstants : PassConstantsLight
{
    float FogColor[4]{ 0.7f, 0.7f, 0.7f, 1.0f };
    float FogStart = 5.f;
    float FogRange = 150.f;
    float TessLod;
    float Pad2;
};

class FLEngineApp4
{
public:
    typedef std::unordered_map<std::string, FireFlame::stRawMesh>           RawMeshMap;
    typedef std::unordered_map<std::string, FireFlame::stRenderItemDesc>    RItemMap;
    typedef std::unordered_map<std::string, FireFlame::ShaderDescription>   ShaderMap;
    typedef std::unordered_map<std::string, PassConstants>                  PassCBMap;
    typedef std::unordered_map<std::string, NamedMaterialConstants>         MaterialMap;

public:
    FLEngineApp4(FireFlame::Engine& engine);
    virtual ~FLEngineApp4();

    virtual void PreInitialize() {}
    virtual void Initialize() = 0;

    virtual void Update(float time_elapsed);
    virtual void UpdateMainPassCB(float time_elapsed);

    virtual void OnGameWindowResized(int w, int h);
    virtual void OnMouseDown(WPARAM btnState, int x, int y);
    virtual void OnMouseUp(WPARAM btnState, int x, int y);
    virtual void OnMouseMove(WPARAM btnState, int x, int y);

    virtual void OnKeyUp(WPARAM wParam, LPARAM lParam);
    virtual void OnKeyboardInput(float time_elapsed);

protected:
    FireFlame::Engine& mEngine;

    ShaderMap                      mShaderDescs;
    RawMeshMap                     mMeshDescs;
    MaterialMap                    mMaterials;
    RItemMap                       mRenderItems;
    std::vector<std::string>       mPasses;

    PassCBMap                      mPassCBs;

    std::unordered_map<std::string, std::string> mShaderMacrosVS;
    std::unordered_map<std::string, std::string> mShaderMacrosPS;

    FireFlame::D3DCamera mCamera;
    POINT mLastMousePos;
};

