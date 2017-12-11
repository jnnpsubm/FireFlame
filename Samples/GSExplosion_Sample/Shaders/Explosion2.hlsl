#define MAX_DIRECTIONAL_LIGHTS 3

Texture2D    gDiffuseMap : register(t0);
Texture2D    gDiffuseMap1 : register(t1);
Texture2D    gDiffuseMap2 : register(t2);

SamplerState gsamPointWrap        : register(s0);
SamplerState gsamPointClamp       : register(s1);
SamplerState gsamLinearWrap       : register(s2);
SamplerState gsamLinearClamp      : register(s3);
SamplerState gsamAnisotropicWrap  : register(s4);
SamplerState gsamAnisotropicClamp : register(s5);

cbuffer ObjectConsts : register(b0)
{
    float4x4 gWorldTrans;
    float4x4 gTexTrans;

    int     gStartExplosion = 0;
    float    gStartExplosionTime = 0.f;
}

cbuffer MaterialConsts : register(b1)
{
    float4   gDiffuseAlbedo;
    float3   gFresnelR0;
    float    gRoughness;
    float4x4 gMatTrans;

    int      gUseTexture;
    int      gUseSpecularMap;
}

struct Light
{
    float3 Strength;
    float  FalloffStart;
    float3 Direction;
    float  FalloffEnd;
    float3 Position;
    float  SpotPower;
};

cbuffer PassConsts : register(b2)
{
    float4x4 gViewTrans;
    float4x4 gProjTrans;
    float4x4 gViewProjTrans;
    float4x4 gInvViewTrans;
    float4x4 gInvProjTrans;
    float4x4 gInvViewProjTrans;

    float3   gEyePos;
    float2   gRenderTargetSize;
    float2   gInvRenderTargetSize;
    float    gNearZ;
    float    gFarZ;
    float    gTotalTime;
    float    gDeltaTime;

    float4   gAmbientLight;

    Light    gLights[16];
}

struct VertexIn
{
    float3 PosL    : POSITION;
    float3 NormalL : NORMAL;
    float2 Tex     : TEXTURE;
};

struct VertexOut
{
    float3 PosL    : POSITION;
    float3 NormalL : NORMAL;
    float2 Tex     : TEXTURE;
};

struct GeoOut
{
    float4 PosH    : SV_POSITION;
    float3 PosW    : POSITION;
    float3 NormalW : NORMAL;
    float2 TexC    : TEXTURE;
    float  Alpha   : ALPHA;
};

VertexOut VS(VertexIn vin)
{
    VertexOut vout;
    vout.PosL = vin.PosL;
    vout.NormalL = vin.NormalL;
    vout.Tex = vin.Tex;
    return vout;
}

[maxvertexcount(3)]
void GS(triangle VertexOut gin[3], inout TriangleStream<GeoOut> triStream)
{
    // pass through
    if (gStartExplosion == 1)
    {
        float3 faceNormal = gin[0].NormalL + gin[1].NormalL + gin[2].NormalL;
        float t = (gTotalTime - gStartExplosionTime)*1000.f;
        faceNormal *= t*t*t*t * 0.0000000001f;
        [unroll]
        for (int i = 0; i < 3; ++i)
        {
            // offset to explode
            gin[i].PosL += faceNormal;

            GeoOut gout;
            gout.PosW = mul(float4(gin[i].PosL, 1.f), gWorldTrans).xyz;
            gout.PosH = mul(float4(gout.PosW, 1.f), gViewProjTrans);
            gout.NormalW = mul(gin[i].NormalL, (float3x3)gWorldTrans);
            gout.TexC = gin[i].Tex;
            gout.Alpha = 15.f / length(faceNormal);
            triStream.Append(gout);
        }
    }
    else
    {
        [unroll]
        for (int i = 0; i < 3; ++i)
        {
            GeoOut gout;
            gout.PosW = mul(float4(gin[i].PosL, 1.f), gWorldTrans).xyz;
            gout.PosH = mul(float4(gout.PosW, 1.f), gViewProjTrans);
            gout.NormalW = mul(gin[i].NormalL, (float3x3)gWorldTrans);
            gout.TexC = gin[i].Tex;
            gout.Alpha = 1.f;
            triStream.Append(gout);
        }
    }
}

float4 PS(GeoOut gout) : SV_Target
{
    float4 diffuseAlbedo = gDiffuseAlbedo/20.f;
    float roughness = gRoughness;
    if (gUseTexture == 1) {
        diffuseAlbedo *= gDiffuseMap.Sample(gsamAnisotropicWrap, gout.TexC);
        if (gUseSpecularMap) {
            roughness = gDiffuseMap1.Sample(gsamAnisotropicWrap, gout.TexC).r;
            roughness = 1.f - roughness;
        }
    }
    else if (gUseTexture == 2) {
        diffuseAlbedo = float4(0.5f, 0.5f, 0.5f, 1.0f);
    }
    else if (gUseTexture == 3) {
        roughness = gDiffuseMap1.Sample(gsamAnisotropicWrap, gout.TexC).r;
        roughness = 1.f - roughness;

        diffuseAlbedo = gDiffuseMap.Sample(gsamAnisotropicWrap, gout.TexC);
        float diffuse = gDiffuseMap2.Sample(gsamAnisotropicWrap, gout.TexC).r;
        diffuseAlbedo += float4(diffuse, diffuse, diffuse, diffuse);
        diffuseAlbedo *= gDiffuseAlbedo;
    }
    else if (gUseTexture == 4) {
        diffuseAlbedo = gDiffuseMap.Sample(gsamAnisotropicWrap, gout.TexC);

        float add1 = gDiffuseMap1.Sample(gsamAnisotropicWrap, gout.TexC).r;
        diffuseAlbedo += float4(add1, add1, add1, 0.f);
        roughness = 1 - add1;

        diffuseAlbedo *= gDiffuseAlbedo;
    }
    else if (gUseTexture == 5) {
        diffuseAlbedo = gDiffuseMap.Sample(gsamAnisotropicWrap, gout.TexC);

        float add1 = gDiffuseMap1.Sample(gsamAnisotropicWrap, gout.TexC).r;
        diffuseAlbedo += float4(add1, add1, add1, 0.f);

        roughness = 1 - add1;

        diffuseAlbedo *= gDiffuseAlbedo;
    }
    else if (gUseTexture == 6) {
        diffuseAlbedo = gDiffuseMap.Sample(gsamAnisotropicWrap, gout.TexC);

        float add1 = gDiffuseMap1.Sample(gsamAnisotropicWrap, gout.TexC).r;
        diffuseAlbedo += float4(add1, add1, add1, 0.f);
        float add2 = gDiffuseMap2.Sample(gsamAnisotropicWrap, gout.TexC).r;
        diffuseAlbedo += float4(add2, add2, add2, 0.f);
        diffuseAlbedo *= gDiffuseAlbedo;
    }
    else if (gUseTexture == 7) {
        diffuseAlbedo = gDiffuseMap.Sample(gsamAnisotropicWrap, gout.TexC);

        float add1 = gDiffuseMap1.Sample(gsamAnisotropicWrap, gout.TexC).r;
        diffuseAlbedo += float4(add1, add1, add1, 0.f);
        float add2 = gDiffuseMap2.Sample(gsamAnisotropicWrap, gout.TexC).r;
        diffuseAlbedo += float4(add2, add2, add2, 0.f);
        roughness = 0.f;
        diffuseAlbedo *= gDiffuseAlbedo;
    }

    float4 color;

    float3 v = normalize(gEyePos - gout.PosW);
    float3 n = normalize(gout.NormalW);

    float4 ambientLight = diffuseAlbedo * gAmbientLight;

    float3 directLight = 0.f;
    [unroll]
    for (int i = 0; i < MAX_DIRECTIONAL_LIGHTS; ++i)
    {
        float3 l = -gLights[i].Direction;
        float ndotl = clamp(dot(n, l), 0.f, 1.0f);
        float3 lightCosined = ndotl * gLights[i].Strength;
        directLight += diffuseAlbedo.xyz * lightCosined;

        float3 fresnelEffect = gFresnelR0 + (1 - gFresnelR0)*pow(1.0f - ndotl, 5);
        float m = (1.0f - roughness) * 256;
        float3 h = normalize(v + l);

        float ndoth = clamp(dot(n, h), 0.f, 1.0f);
        float3 specular = (m + 8.f) / 8.f * lightCosined * fresnelEffect * pow(ndoth, m);
        directLight += specular;
    }

    color = ambientLight + float4(directLight,1.0f);
    color.a = gout.Alpha;
    return color;
}