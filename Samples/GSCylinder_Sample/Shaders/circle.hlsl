cbuffer cbPerObject : register(b0)
{
    float4x4 gWorld;
    float4x4 gTexTransform;
};

// Constant data that varies per material.
cbuffer cbPass : register(b1)
{
    float4x4 gView;
    float4x4 gProj;
    float4x4 gViewProj;
};

struct VertexIn
{
    float3 PosL    : POSITION;
    float3 Color   : COLOR;
};

struct VertexOut
{
    float4 PosH    : SV_POSITION;
    float3 Color   : COLOR;
};

VertexOut VS(VertexIn vin)
{
    VertexOut vout = (VertexOut)0.0f;

    float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
    vout.PosH = mul(posW, gViewProj);
    vout.Color = vin.Color;
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    return float4(pin.Color,1.0f);
}


