cbuffer cbPerObject : register(b0)
{
    float4x4 gViewProj;
    float3   gLineColor;
    float    gTexScale;
    float3   gBackdropColor;
    float    gWidth;

    float    g_a;
};

struct VertexIn
{
    float3 PosH : POSITION;
    float2 TexC : TEXCOORD;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float2 TexC : TEXCOORD;
};