Texture2D    gDiffuseMap : register(t0);


SamplerState gsamPointWrap        : register(s0);
SamplerState gsamPointClamp       : register(s1);
SamplerState gsamLinearWrap       : register(s2);
SamplerState gsamLinearClamp      : register(s3);
SamplerState gsamAnisotropicWrap  : register(s4);
SamplerState gsamAnisotropicClamp : register(s5);

cbuffer cbPerObject : register(b0)
{
    float3   gColor;
};

cbuffer cbMaterial : register(b1)
{
    float4   gDiffuseAlbedo;
    float3   gFresnelR0;
    float    gRoughness;
    float4x4 gMatTransform;
    int      gUseTexture;
};

struct VertexIn
{
    float3 PosH : POSITION;
    float2 TexC : TEXTURE;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float2 TexC : TEXTURE;
};

VertexOut VS(VertexIn vin)
{
    VertexOut vout;
    vout.PosH = float4(vin.PosH, 1.f);
    vout.TexC = vin.TexC;
    return vout;
}

float4 PS(VertexOut pin) : SV_TARGET
{
    float4 color = gDiffuseMap.Sample(gsamAnisotropicWrap, pin.TexC);
    return color;
}
