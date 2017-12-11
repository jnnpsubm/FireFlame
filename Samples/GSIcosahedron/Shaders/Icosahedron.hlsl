#define MAX_DIRECTIONAL_LIGHTS 3

cbuffer ObjectConsts : register(b0)
{
    float4x4 gWorldTrans;
    float4x4 gTexTrans;

    int      gSubdivideLevel = 0;
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
    float3 PosL    : POSITION;
};

struct VertexOut
{
    float3 PosL    : POSITION;
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

/*
                        1
                      /  \
                     /    \
                  m0/------\m1
                   / \   /  \
                  /   \ /    \
                 0------------2
                       m2

*/
void subdivide(in VertexOut v0, in VertexOut v1, in VertexOut v2, out VertexOut vout[6])
{
    float3 m0 = 0.5f*(v0.PosL + v1.PosL);
    float3 m1 = 0.5f*(v1.PosL + v2.PosL);
    float3 m2 = 0.5f*(v2.PosL + v0.PosL);
    vout[0].PosL = normalize(v0.PosL);
    vout[1].PosL = normalize(m0);
    vout[2].PosL = normalize(m2);
    vout[3].PosL = normalize(m1);
    vout[4].PosL = normalize(v2.PosL);
    vout[5].PosL = normalize(v1.PosL);
}

void vertex2stream(VertexOut vertexSubdivide[6], inout TriangleStream<GeoOut> triStream)
{
    GeoOut gout[6];
    [unroll]
    for (int i = 0; i < 6; ++i)
    {
        gout[i].PosW = mul(float4(vertexSubdivide[i].PosL, 1.f), gWorldTrans).xyz;
        gout[i].PosH = mul(float4(gout[i].PosW, 1.f), gViewProjTrans);
        gout[i].NormalW = mul(vertexSubdivide[i].PosL, (float3x3)gWorldTrans);
    }
    [unroll]
    for (int j = 0; j < 5; ++j)
    {
        triStream.Append(gout[j]);
    }
    triStream.RestartStrip();
    triStream.Append(gout[1]);
    triStream.Append(gout[5]);
    triStream.Append(gout[3]);
}

void subdivide1(VertexOut vin[3], inout TriangleStream<GeoOut> triStream)
{
    VertexOut vertexSubdivide[6];
    subdivide(vin[0], vin[1], vin[2], vertexSubdivide);
    vertex2stream(vertexSubdivide, triStream);
}

void subdivide1(VertexOut v0, VertexOut v1, VertexOut v2, inout TriangleStream<GeoOut> triStream)
{
    VertexOut vertexSubdivide[6];
    subdivide(v0, v1, v2, vertexSubdivide);
    vertex2stream(vertexSubdivide, triStream);
}

void subdivide2(VertexOut vin[3], inout TriangleStream<GeoOut> triStream)
{
    VertexOut vertexSubdivide1[6];
    subdivide(vin[0], vin[1], vin[2], vertexSubdivide1);

    subdivide1(vertexSubdivide1[0], vertexSubdivide1[1], vertexSubdivide1[2], triStream);
    subdivide1(vertexSubdivide1[1], vertexSubdivide1[2], vertexSubdivide1[3], triStream);
    subdivide1(vertexSubdivide1[2], vertexSubdivide1[3], vertexSubdivide1[4], triStream);
    subdivide1(vertexSubdivide1[1], vertexSubdivide1[5], vertexSubdivide1[3], triStream);
}

void subdivide2(VertexOut v0, VertexOut v1, VertexOut v2, inout TriangleStream<GeoOut> triStream)
{
    VertexOut vertexSubdivide1[6];
    subdivide(v0, v1, v2, vertexSubdivide1);

    subdivide1(vertexSubdivide1[0], vertexSubdivide1[1], vertexSubdivide1[2], triStream);
    subdivide1(vertexSubdivide1[1], vertexSubdivide1[2], vertexSubdivide1[3], triStream);
    subdivide1(vertexSubdivide1[2], vertexSubdivide1[3], vertexSubdivide1[4], triStream);
    subdivide1(vertexSubdivide1[1], vertexSubdivide1[5], vertexSubdivide1[3], triStream);
}

void subdivide3(VertexOut vin[3], inout TriangleStream<GeoOut> triStream)
{
    VertexOut vertexSubdivide1[6];
    subdivide(vin[0], vin[1], vin[2], vertexSubdivide1);

    subdivide2(vertexSubdivide1[0], vertexSubdivide1[1], vertexSubdivide1[2], triStream);
    subdivide2(vertexSubdivide1[1], vertexSubdivide1[2], vertexSubdivide1[3], triStream);
    subdivide2(vertexSubdivide1[2], vertexSubdivide1[3], vertexSubdivide1[4], triStream);
    subdivide2(vertexSubdivide1[1], vertexSubdivide1[5], vertexSubdivide1[3], triStream);
}

[maxvertexcount(32)]
void GS(triangle VertexOut gin[3], inout TriangleStream<GeoOut> triStream)
{
    //subdivide1(gin, triStream);
    //subdivide2(gin, triStream);
    //subdivide3(gin, triStream);
    int subdivideLevel = gSubdivideLevel;

    float3 v = gEyePos;
    float distance = length(v);
    if (distance < 15.f)
        subdivideLevel = 2;
    else if (distance < 30.f)
        subdivideLevel = 1;
    else
        subdivideLevel = 0;

    if (subdivideLevel == 1)
    {
        subdivide1(gin, triStream);
    }
    else if (subdivideLevel == 2)
    {
        subdivide2(gin, triStream);
    }
    else
    {
        [unroll]
        for (int i = 0; i < 3; ++i)
        {
            GeoOut gout;
            gout.PosW = mul(float4(gin[i].PosL,1.f), gWorldTrans).xyz;
            gout.PosH = mul(float4(gout.PosW, 1.f), gViewProjTrans);
            gout.NormalW = mul(normalize(gin[i].PosL), (float3x3)gWorldTrans);
            triStream.Append(gout);
        }
    }

    // pass through
    /*[unroll]
    for (int i = 0; i < 3; ++i)
    {
        GeoOut gout;
        gout.PosW = mul(gin[i].PosL, gWorldTrans);
        gout.PosH = mul(float4(gout.PosW,1.f), gViewProjTrans);
        gout.NormalW = mul(normalize(gin[i].PosL), (float3x3)gWorldTrans);
        triStream.Append(gout);
    }*/
}

float4 PS(GeoOut gout) : SV_Target
{
    float4 color;

    float3 v = normalize(gEyePos - gout.PosW);
    float3 n = normalize(gout.NormalW);

    float4 ambientLight = gDiffuseAlbedo * gAmbientLight;

    float3 directLight = 0.f;
    [unroll]
    for (int i = 0; i < MAX_DIRECTIONAL_LIGHTS; ++i)
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