#pragma once
#include "..\..\Matrix\FLMatrix4X4.h"

namespace FireFlame {
struct ObjectConstants
{
    DirectX::XMFLOAT4X4 World = Matrix4X4();
};

struct PassConstants
{
    DirectX::XMFLOAT4X4 View = Matrix4X4();
    DirectX::XMFLOAT4X4 InvView = Matrix4X4();
    DirectX::XMFLOAT4X4 Proj = Matrix4X4();
    DirectX::XMFLOAT4X4 InvProj = Matrix4X4();
    DirectX::XMFLOAT4X4 ViewProj = Matrix4X4();
    DirectX::XMFLOAT4X4 InvViewProj = Matrix4X4();
    DirectX::XMFLOAT3 EyePosW = { 0.0f, 0.0f, 0.0f };
    float cbPerObjectPad1 = 0.0f;
    DirectX::XMFLOAT2 RenderTargetSize = { 0.0f, 0.0f };
    DirectX::XMFLOAT2 InvRenderTargetSize = { 0.0f, 0.0f };
    float NearZ = 0.0f;
    float FarZ = 0.0f;
    float TotalTime = 0.0f;
    float DeltaTime = 0.0f;
};
} // end namespace