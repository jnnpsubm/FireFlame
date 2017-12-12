cbuffer cbPerObject : register(b0)
{
    float3   gColor;
};

struct VertexIn
{
    float3 PosH : POSITION;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
};

VertexOut VS(VertexIn vin)
{
    VertexOut vout;
    vout.PosH = float4(vin.PosH, 1.f);
    return vout;
}

float4 PS(VertexOut pin) : SV_TARGET
{
    float4 color;
    color = float4(gColor, 0.5f);
    return color;
}
