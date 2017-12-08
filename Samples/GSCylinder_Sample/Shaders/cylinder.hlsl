cbuffer ObjectConsts : register(b0)
{
    float4x4 gWorldTrans;
    float4x4 gTexTrans;
}

cbuffer MaterialConsts : register(b1)
{
    float4   gDiffuseAlbedo;
    float3   gFresnelR0;
    float    gRoughness;
    float4x4 gMatTrans;
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
    float3 PosL : POSITION;
    float3 Color: COLOR;
};

struct VertexOut
{
    float3 PosL : POSITION;
};

struct GeoOut
{
    float4 PosH    : SV_POSITION;
    float3 PosW    : POSITION;
    float3 NormalW : NORMAL;
};

VertexOut VS(VertexIn vin)
{
    VertexOut vout;
    vout.PosL = vin.PosL;
    return vout;
}

GeoOut GetGeoOut(float3 PosL)
{
    GeoOut gout;
    gout.PosW = mul(float4(PosL, 1.f), gWorldTrans).xyz;
    gout.NormalW = float3(PosL.x, 0.f, PosL.z);
    //gout.NormalW = PosL;
    gout.NormalW = mul(gout.NormalW, (float3x3)gWorldTrans);
    gout.NormalW = normalize(gout.NormalW);
    gout.PosH = mul(float4(gout.PosW, 1.f), gViewProjTrans);
    return gout;
}

void GSAppend(float3 pos0, float3 pos1, inout TriangleStream<GeoOut> triStream)
{
    GeoOut gout = GetGeoOut(pos0);
    triStream.Append(gout);

    gout = GetGeoOut(pos0 + float3(0.f, 3.f, 0.f));
    triStream.Append(gout);

    gout = GetGeoOut(pos1);
    triStream.Append(gout);

    gout = GetGeoOut(pos1 + float3(0.f, 3.f, 0.f));
    triStream.Append(gout);

    triStream.RestartStrip();
}

[maxvertexcount(100)]
void GS(line VertexOut gin[2], inout TriangleStream<GeoOut> triStream)
{
    for (int i = 0; i < 25; ++i)
    {
        float3 pos0 = gin[0].PosL + (gin[1].PosL - gin[0].PosL)*(float)i/ 25;
        pos0 = normalize(pos0);
        float3 pos1 = gin[0].PosL + (gin[1].PosL - gin[0].PosL)*(float)(i+1.f) / 25;
        pos1 = normalize(pos1);
        GSAppend(pos0, pos1, triStream);
    }

    /*float3 pos0 = gin[0].PosL;
    float3 pos1 = gin[1].PosL;
    GSAppend(pos0, pos1, triStream);*/
}

float4 PS(GeoOut gout) : SV_Target
{
    float4 color;

    float3 v = normalize(gEyePos - gout.PosW);
    float3 n = normalize(gout.NormalW);

    float4 ambientLight = gDiffuseAlbedo * gAmbientLight;

    float3 directLight = 0.f;
    [unroll]
    for (int i = 0; i < 3; ++i)
    {
        float3 l = -gLights[i].Direction;
        float ndotl = clamp(dot(n, l), 0.f, 1.0f);
        float3 lightCosined = ndotl * gLights[i].Strength;
        directLight += gDiffuseAlbedo.xyz * lightCosined;

        float3 fresnelEffect = gFresnelR0 + (1 - gFresnelR0)*pow(1.0f - ndotl, 5);
        float m = (1.0f - gRoughness) * 256;
        float3 h = normalize(v + l);

        float ndoth = clamp(dot(n, h), 0.f, 1.0f);
        float3 specular = (m + 8.f) / 8.f * lightCosined * fresnelEffect * pow(ndoth, m);
        directLight += specular;
    }

    color = ambientLight + float4(directLight,1.0f);

    return color;
}