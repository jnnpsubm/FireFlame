// basic tessellation sample

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
    float2 reserve;
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
    float EdgeTess[4]   : SV_TessFactor;
    float InsideTess[2] : SV_InsideTessFactor;
};

PatchTess ConstantHS(InputPatch<VertexOut, 16> patch, uint patchID : SV_PrimitiveID)
{
    PatchTess patchTess;
    patchTess.EdgeTess[0] = 64.f;
    patchTess.EdgeTess[1] = 64.f;
    patchTess.EdgeTess[2] = 64.f;
    patchTess.EdgeTess[3] = 64.f;
    patchTess.InsideTess[0] = 64.f;
    patchTess.InsideTess[1] = 64.f;
    return patchTess;
}

struct HullOut
{
    float3 PosL : POSITION;
};

[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(16)]
[patchconstantfunc("ConstantHS")]
[maxtessfactor(64.f)]
HullOut HS(InputPatch<VertexOut, 16> p, uint i : SV_OutputControlPointID, uint patchID : SV_PrimitiveID)
{
    HullOut hout;
    hout.PosL = p[i].PosL;
    return hout;
}

struct DomainOut
{
    float4 PosH    : SV_POSITION;
    float3 PosW    : POSITION;
    float3 NormalW : NORMAL;
};

float4 BernsteinBasis(float t)
{
    float invT = 1.0f - t;
    return float4(invT * invT * invT,
        3.0f * t * invT * invT,
        3.0f * t * t * invT,
        t * t * t);
}

float3 CubicBezierSum(const OutputPatch<HullOut, 16> bezpatch, float4 basisU, float4 basisV)
{
    float3 sum = float3(0.0f, 0.0f, 0.0f);
    sum = basisV.x * (basisU.x*bezpatch[0].PosL + basisU.y*bezpatch[1].PosL + basisU.z*bezpatch[2].PosL + basisU.w*bezpatch[3].PosL);
    sum += basisV.y * (basisU.x*bezpatch[4].PosL + basisU.y*bezpatch[5].PosL + basisU.z*bezpatch[6].PosL + basisU.w*bezpatch[7].PosL);
    sum += basisV.z * (basisU.x*bezpatch[8].PosL + basisU.y*bezpatch[9].PosL + basisU.z*bezpatch[10].PosL + basisU.w*bezpatch[11].PosL);
    sum += basisV.w * (basisU.x*bezpatch[12].PosL + basisU.y*bezpatch[13].PosL + basisU.z*bezpatch[14].PosL + basisU.w*bezpatch[15].PosL);

    return sum;
}

float4 dBernsteinBasis(float t)
{
    float invT = 1.0f - t;
    return float4(-3 * invT * invT,
        3 * invT * invT - 6 * t * invT,
        6 * t * invT - 3 * t * t,
        3 * t * t);
}

[domain("quad")]
DomainOut DS(PatchTess patchTess, float2 uv : SV_DomainLocation, const OutputPatch<HullOut, 16> bezPatch)
{
    DomainOut dout;

    float4 basisU = BernsteinBasis(uv.x);
    float4 basisV = BernsteinBasis(uv.y);

    float3 p = CubicBezierSum(bezPatch, basisU, basisV);

    float4 PosW = mul(float4(p, 1.0f), gWorld);
    dout.PosW = PosW.xyz;
    dout.PosH = mul(PosW, gViewProj);

    float4 dBasisU = dBernsteinBasis(uv.x);
    float4 dBasisV = dBernsteinBasis(uv.y);
    float3 dpdu = CubicBezierSum(bezPatch, dBasisU,  basisV);
    float3 dpdv = CubicBezierSum(bezPatch,  basisU, dBasisV);
    float3 NormalL = cross(dpdu, dpdv);
    dout.NormalW = mul(NormalL,(float3x3)gWorld);

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