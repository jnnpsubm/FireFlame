

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

Texture2D    gDiffuseMap : register(t0);
Texture2D    gDiffuseMap1 : register(t1);
Texture2D    gDiffuseMap2 : register(t2);
Texture2D    gDiffuseMap3 : register(t3);

SamplerState gsamPointWrap        : register(s0);
SamplerState gsamPointClamp       : register(s1);
SamplerState gsamLinearWrap       : register(s2);
SamplerState gsamLinearClamp      : register(s3);
SamplerState gsamAnisotropicWrap  : register(s4);
SamplerState gsamAnisotropicClamp : register(s5);

// Constant data that varies per frame.
cbuffer cbPerObject : register(b0)
{
    float4x4 gWorld;
    float4x4 gTexTransform;
};

cbuffer cbMultiObjects : register(b3)
{
    Light gLights2[MaxLights];
};

cbuffer cbMaterial : register(b1)
{
    float4   gDiffuseAlbedo;
    float3   gFresnelR0;
    float    gRoughness;
    float4x4 gMatTransform;

    int      gUseTexture;
    int      gUseSpecularMap;
};

// Constant data that varies per material.
cbuffer cbPass : register(b2)
{
    float4x4 gView;
    float4x4 gInvView;
    float4x4 gProj;
    float4x4 gInvProj;
    float4x4 gViewProj;
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
    float3 NormalL : NORMAL;
    float2 TexC    : TEXCOORD;
};

struct VertexOut
{
    float4 PosH    : SV_POSITION;
    float3 PosW    : POSITION;
    float3 NormalW : NORMAL;
    float2 TexC    : TEXCOORD;
    float2 TexC2   : TEXCOORD2;
};

VertexOut VS(VertexIn vin)
{
    VertexOut vout = (VertexOut)0.0f;

    // Transform to world space.
    float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
    vout.PosW = posW.xyz;

    // Assumes nonuniform scaling; otherwise, need to use inverse-transpose of world matrix.
    vout.NormalW = mul(vin.NormalL, (float3x3)gWorld);

    // Transform to homogeneous clip space.
    vout.PosH = mul(posW, gViewProj);

    // Output vertex attributes for interpolation across triangle.
    vout.TexC2 = vin.TexC;
    float4 texC = mul(float4(vin.TexC, 0.0f, 1.0f), gTexTransform);
    vout.TexC = mul(texC, gMatTransform).xy;

    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    float roughness = gRoughness;
    float4 AmbientLight = gAmbientLight;

#ifdef TERRAIN
    float4 diffuseAlbedo;// = gDiffuseMap3.Sample(gsamAnisotropicWrap, pin.TexC) * gDiffuseAlbedo;
    float4 diffuseAlbedo1 = gDiffuseMap1.Sample(gsamAnisotropicWrap, pin.TexC) * gDiffuseAlbedo;
    float4 diffuseAlbedo2 = gDiffuseMap2.Sample(gsamAnisotropicWrap, pin.TexC) * gDiffuseAlbedo;
    float4 diffuseAlbedo3 = gDiffuseMap3.Sample(gsamAnisotropicWrap, pin.TexC) * gDiffuseAlbedo;
    float height = gDiffuseMap.Sample(gsamAnisotropicWrap, pin.TexC2).r;
    if (height < 0)
    {
        height = smoothstep(-1.0f, 0.0f, height);
        diffuseAlbedo = lerp(diffuseAlbedo1, diffuseAlbedo2, height);
    }
    else
    {
        height = smoothstep(0.0f, 1.0f, height);
        diffuseAlbedo = lerp(diffuseAlbedo2, diffuseAlbedo3, height);
    }
#else
    float4 diffuseAlbedo = gDiffuseAlbedo;
    if (gUseTexture == 1) {
        diffuseAlbedo *= gDiffuseMap.Sample(gsamAnisotropicWrap, pin.TexC);
        if (gUseSpecularMap) {
            roughness = gDiffuseMap1.Sample(gsamAnisotropicWrap, pin.TexC).r;
            roughness = 1.f - roughness;
            //diffuseAlbedo = float4(roughness, roughness, roughness, roughness);
        }
        //diffuseAlbedo = clamp(diffuseAlbedo, 0.8f, 1.0f);
    }
    else if (gUseTexture == 2) {
        diffuseAlbedo = float4(0.5f, 0.5f, 0.5f, 1.0f);
    }
    else if (gUseTexture == 3) {
        roughness = gDiffuseMap1.Sample(gsamAnisotropicWrap, pin.TexC).r;
        roughness = 1.f - roughness;

        diffuseAlbedo = gDiffuseMap.Sample(gsamAnisotropicWrap, pin.TexC);
        float diffuse = gDiffuseMap2.Sample(gsamAnisotropicWrap, pin.TexC).r;
        diffuseAlbedo += float4(diffuse, diffuse, diffuse, diffuse);
        //diffuseAlbedo *= diffuse*255.f;

        diffuseAlbedo *= gDiffuseAlbedo;
    }
    else if (gUseTexture == 4) {
        diffuseAlbedo = gDiffuseMap.Sample(gsamAnisotropicWrap, pin.TexC);

        float add1 = gDiffuseMap1.Sample(gsamAnisotropicWrap, pin.TexC).r;
        diffuseAlbedo += float4(add1, add1, add1, 0.f);
        //diffuseAlbedo *= diffuse*255.f;

        roughness = 1 - add1;

        diffuseAlbedo *= gDiffuseAlbedo;
    }
    else if (gUseTexture == 5) {
        diffuseAlbedo = gDiffuseMap.Sample(gsamAnisotropicWrap, pin.TexC);

        float add1 = gDiffuseMap1.Sample(gsamAnisotropicWrap, pin.TexC).r;
        diffuseAlbedo += float4(add1, add1, add1, 0.f);
        //float add2 = gDiffuseMap2.Sample(gsamAnisotropicWrap, pin.TexC).r;
        //diffuseAlbedo += float4(add2, add2, add2, 0.f);

        roughness = 1 - add1;

        diffuseAlbedo *= gDiffuseAlbedo;
    }
    else if (gUseTexture == 6) {
        diffuseAlbedo = gDiffuseMap.Sample(gsamAnisotropicWrap, pin.TexC);

        float add1 = gDiffuseMap1.Sample(gsamAnisotropicWrap, pin.TexC).r;
        diffuseAlbedo += float4(add1, add1, add1, 0.f);
        float add2 = gDiffuseMap2.Sample(gsamAnisotropicWrap, pin.TexC).r;
        diffuseAlbedo += float4(add2, add2, add2, 0.f);

        //roughness = abs(1.f - add1);
        //roughness = add1;
        //roughness = 0.f;

        //pin.NormalW = -pin.NormalW;
        //AmbientLight = float4(0.f, 0.f, 0.f, 0.f);

        diffuseAlbedo *= gDiffuseAlbedo;
        //diffuseAlbedo = abs(diffuseAlbedo);
        //diffuseAlbedo = float4(10.f, 10.f, 10.f, 10.f);
    }
    else if (gUseTexture == 7) {
        diffuseAlbedo = gDiffuseMap.Sample(gsamAnisotropicWrap, pin.TexC);

        float add1 = gDiffuseMap1.Sample(gsamAnisotropicWrap, pin.TexC).r;
        diffuseAlbedo += float4(add1, add1, add1, 0.f);
        float add2 = gDiffuseMap2.Sample(gsamAnisotropicWrap, pin.TexC).r;
        diffuseAlbedo += float4(add2, add2, add2, 0.f);

        //roughness = abs(1.f - add1);
        //roughness = add1;
        roughness = 0.f;

        //pin.NormalW = -pin.NormalW;
        //AmbientLight = float4(0.f, 0.f, 0.f, 0.f);

        diffuseAlbedo *= gDiffuseAlbedo;
        //diffuseAlbedo = abs(diffuseAlbedo);
        //diffuseAlbedo = float4(10.f, 10.f, 10.f, 10.f);
    }
#endif

#ifdef ALPHA_CLIP
    clip(diffuseAlbedo.a - 0.1f);
#endif

    // Interpolating normal can unnormalize it, so renormalize it.
    pin.NormalW = normalize(pin.NormalW);

    // Vector from point being lit to eye. 
    float3 toEyeW = gEyePosW - pin.PosW;
    float toEyeDis = distance(pin.PosW, gEyePosW);
    toEyeW /= toEyeDis;

    // Light terms.
    float4 ambient = AmbientLight*diffuseAlbedo;

    const float shininess = 1.0f - roughness;
    Material mat = { diffuseAlbedo, gFresnelR0, shininess };
    float3 shadowFactor = 1.0f;
    float4 directLight = ComputeLighting(gLights2, mat, pin.PosW,
    pin.NormalW, toEyeW, shadowFactor);

    float4 litColor = ambient + directLight;

#ifdef FOG
    float step = smoothstep(gFogStart, gFogStart + gFogRange, toEyeDis);
    litColor = lerp(litColor, gFogColor, step);
#endif

    // Common convention to take alpha from diffuse albedo.
    litColor.a = diffuseAlbedo.a;

    return litColor;
}


