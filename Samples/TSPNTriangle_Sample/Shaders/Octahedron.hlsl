// PN triangle tessellation sample

// Defaults for number of lights.
#ifndef NUM_DIR_LIGHTS
#define NUM_DIR_LIGHTS 3
#endif

#ifndef NUM_POINT_LIGHTS
#define NUM_POINT_LIGHTS 0
#endif

#ifndef NUM_SPOT_LIGHTS
#define NUM_SPOT_LIGHTS 0
#endif

// Include structures and functions for lighting.
#include "..\..\Common\LightingUtil.hlsli"

// Constant data that varies per frame.
cbuffer cbPerObject : register(b0)
{
    float4x4 gWorld;
    float4x4 gTexTransform;
};

cbuffer cbMaterial : register(b1)
{
    float4   gDiffuseAlbedo;
    float3   gFresnelR0;
    float    gRoughness;
    float4x4 gMatTransform;
};

// Constant data that varies per material.
cbuffer cbPass : register(b2)
{
    float4x4 gView;
    float4x4 gProj;
    float4x4 gViewProj;
    float4x4 gInvView;
    float4x4 gInvProj;
    float4x4 gInvViewProj;
    float3 gEyePosW;
    float cbPerObjectPad1;
    float2 gRenderTargetSize;
    float2 gInvRenderTargetSize;
    float gNearZ;
    float gFarZ;
    float gTotalTime;
    float gDeltaTime;

    float4 gAmbientLight;
    // Indices [0, NUM_DIR_LIGHTS) are directional lights;
    // indices [NUM_DIR_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHTS) are point lights;
    // indices [NUM_DIR_LIGHTS+NUM_POINT_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHT+NUM_SPOT_LIGHTS)
    // are spot lights for a maximum of MaxLights per object.
    Light gLights[MaxLights];

    float4 gFogColor;
    float  gFogStart;
    float  gFogRange;
    float  gTessLod;
    float  gReserve;
};

struct VertexIn
{
    float3 PosL    : POSITION;
};

struct VertexOut
{
    float3 PosL    : POSITION;
};

VertexOut VS(VertexIn vin)
{
    VertexOut vout;
    vout.PosL = vin.PosL;
    return vout;
}

struct PatchTess
{
    float EdgeTess[3]   : SV_TessFactor;
    float InsideTess[1] : SV_InsideTessFactor;

    float3 b300 : COEFFIENTP1;
    float3 b030 : COEFFIENTP2;
    float3 b003 : COEFFIENTP3;
    float3 b210 : COEFFIENTP4;
    float3 b120 : COEFFIENTP5;
    float3 b021 : COEFFIENTP6;
    float3 b012 : COEFFIENTP7;
    float3 b102 : COEFFIENTP8;
    float3 b201 : COEFFIENTP9;
    float3 b111 : COEFFIENTP10;

    float3 n200 : COEFFIENTN1;
    float3 n020 : COEFFIENTN2;
    float3 n002 : COEFFIENTN3;
    float3 n110 : COEFFIENTN4;
    float3 n011 : COEFFIENTN5;
    float3 n101 : COEFFIENTN6;
};

PatchTess ConstantHS(InputPatch<VertexOut, 3> patch, uint patchID : SV_PrimitiveID)
{
    PatchTess patchTess;

    float tess = min(64.f, gTessLod);
    tess = max(1.0f, tess);
    // Uniformly tessellate the patch.
    patchTess.EdgeTess[0] = tess;
    patchTess.EdgeTess[1] = tess;
    patchTess.EdgeTess[2] = tess;
    patchTess.InsideTess[0] = tess;

#define P1 (patch[0].PosL)
#define P2 (patch[1].PosL)
#define P3 (patch[2].PosL)
#define N1 (patch[0].PosL)
#define N2 (patch[1].PosL)
#define N3 (patch[2].PosL)
    float w12 = dot((P2 - P1), N1);
    float w21 = dot((P1 - P2), N2);
    float w23 = dot((P3 - P2), N2);
    float w32 = dot((P2 - P3), N3);
    float w31 = dot((P1 - P3), N3);
    float w13 = dot((P3 - P1), N1);
    patchTess.b300 = P1;
    patchTess.b030 = P2;
    patchTess.b003 = P3;
    patchTess.b210 = (2 * P1 + P2 - w12 * N1) / 3.f;
    patchTess.b120 = (2 * P2 + P1 - w21 * N2) / 3.f;
    patchTess.b021 = (2 * P2 + P3 - w23 * N2) / 3.f;
    patchTess.b012 = (2 * P3 + P2 - w32 * N3) / 3.f;
    patchTess.b102 = (2 * P3 + P1 - w31 * N3) / 3.f;
    patchTess.b201 = (2 * P1 + P3 - w13 * N1) / 3.f;
    float3 E = (patchTess.b210 + patchTess.b120 + patchTess.b021 + patchTess.b012 + patchTess.b102 + patchTess.b201) / 6.f;
    float3 V = (P1 + P2 + P3) / 3.f;
    patchTess.b111 = E + (E - V) / 2.f;

    patchTess.n200 = N1;
    patchTess.n020 = N2;
    patchTess.n002 = N3;
    float v12 = 2.f * dot(P2 - P1, N1 + N2) / dot(P2 - P1, P2 - P1);
    float v23 = 2.f * dot(P3 - P2, N2 + N3) / dot(P3 - P2, P3 - P2);
    float v31 = 2.f * dot(P1 - P3, N3 + N1) / dot(P1 - P3, P1 - P3);
    patchTess.n110 = normalize(N1 + N2 - v12 * (P2 - P1));
    patchTess.n011 = normalize(N2 + N3 - v23 * (P3 - P2));
    patchTess.n101 = normalize(N3 + N1 - v31 * (P1 - P3));
#undef P1
#undef P2
#undef P3
#undef N1
#undef N2
#undef N3

    return patchTess;
}

struct HullOut
{
    float3 PosL    : POSITION;
    float3 NormalL : NORMAL;
};

[domain("tri")]
//[partitioning("fractional_odd")]
[partitioning("integer")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(3)]
[patchconstantfunc("ConstantHS")]
[maxtessfactor(64.f)]
HullOut HS(InputPatch<VertexOut, 3> p, uint i : SV_OutputControlPointID, uint patchID : SV_PrimitiveID)
{
    HullOut hout;
    hout.PosL = p[i].PosL;
    hout.NormalL = p[i].PosL;
    return hout;
}

struct DomainOut
{
    float4 PosH : SV_POSITION;
    float3 PosW    : POSITION;
    float3 NormalW : NORMAL;
};

[domain("tri")]
DomainOut DS(PatchTess patch, float3 uvw : SV_DomainLocation, const OutputPatch<HullOut, 3> tri)
{
    DomainOut dout;

#define u (uvw.x)
#define v (uvw.y)
#define w (uvw.z)
    float3 p = patch.b300 * w * w * w + patch.b030 * u * u * u + patch.b003 * v * v * v
        + patch.b210 * 3.0 * w * w * u + patch.b120 * 3.0 * w * u * u + patch.b201 * 3.0 * w * w * v
        + patch.b021 * 3.0 * u * u * v + patch.b102 * 3.0 * w * v * v + patch.b012 * 3.0 * u * v * v
        + patch.b111 * 6.0 * w * u * v;
    float3 normal = patch.n200*w*w + patch.n020*u*u + patch.n002*v*v
        + patch.n110*w*u + patch.n011*u*v + patch.n101*w*v;
    normal = normalize(normal);
#undef u
#undef v
#undef w

    float4 PosW = mul(float4(p, 1.f), gWorld);
    dout.PosW = PosW.xyz;
    dout.NormalW = mul(normal, (float3x3)gWorld);
    dout.PosH = mul(PosW, gViewProj);
    return dout;
}

float4 PS(DomainOut pin) : SV_Target
{
    float4 diffuseAlbedo = gDiffuseAlbedo;

    // Interpolating normal can unnormalize it, so renormalize it.
    pin.NormalW = normalize(pin.NormalW);

    // Vector from point being lit to eye. 
    float3 toEyeW = gEyePosW - pin.PosW;
    float toEyeDis = distance(pin.PosW, gEyePosW);
    toEyeW /= toEyeDis;

    // Light terms.
    float4 ambient = gAmbientLight * diffuseAlbedo;

    const float shininess = 1.0f - gRoughness;
    Material mat = { diffuseAlbedo, gFresnelR0, shininess };
    float3 shadowFactor = 1.0f;
    float4 directLight = ComputeLighting(gLights, mat, pin.PosW,
        pin.NormalW, toEyeW, shadowFactor);

    float4 litColor = ambient + directLight;

    // Common convention to take alpha from diffuse albedo.
    litColor.a = diffuseAlbedo.a;

    return litColor;
}