#pragma once
#include "..\..\Matrix\FLMatrix4X4.h"
#include "..\..\Light\FLLight.h"

namespace FireFlame {

struct ObjectConstBuffer
{

};

struct MultiObjectConstBuffer
{
    MultiObjectConstBuffer() = default;
    explicit MultiObjectConstBuffer(const std::string& name, const std::string& shaderName, unsigned int CBIndex = 0)
        :name(name), shaderName(shaderName), CBIndex(CBIndex) {}
    std::string name;
    std::string shaderName;
    unsigned int CBIndex = -1;
};

struct PassConstBuffer
{
    PassConstBuffer() = default;
    explicit PassConstBuffer(const std::string& name, const std::string& shaderName, unsigned int CBIndex = 0)
        :name(name), shaderName(shaderName), CBIndex(CBIndex) {}
    std::string name;
    std::string shaderName;
    unsigned int CBIndex = -1;
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
} // end namespace