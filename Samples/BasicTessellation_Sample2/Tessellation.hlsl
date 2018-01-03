// basic tessellation sample

// Constant data that varies per frame.
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
};

cbuffer cbMaterial : register(b2)
{
    float4   gDiffuseAlbedo;
    float3   gFresnelR0;
    float    gRoughness;
    float4x4 gMatTransform;
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

PatchTess ConstantHS(InputPatch<VertexOut, 4> patch, uint patchID : SV_PrimitiveID)
{
    PatchTess patchTess;

    float3 centerL = 0.25f*(patch[0].PosL + patch[1].PosL + patch[2].PosL + patch[3].PosL);
    float3 centerW = mul(float4(centerL, 1.f), gWorld).xyz;
    float d = distance(centerW, gEyePosW);

    // Tessellate the patch based on distance from the eye such that
    // the tessellation is 0 if d >= d1 and 64 if d <= d0.  The interval
    // [d0, d1] defines the range we tessellate in.
    const float d0 = 20.0f;
    const float d1 = 100.0f;
    float tess = 64.0f*saturate((d1 - d) / (d1 - d0));

    // Uniformly tessellate the patch.
    patchTess.EdgeTess[0] = tess;
    patchTess.EdgeTess[1] = tess;
    patchTess.EdgeTess[2] = tess;
    patchTess.EdgeTess[3] = tess;
    patchTess.InsideTess[0] = tess;
    patchTess.InsideTess[1] = tess;
    return patchTess;
}

struct HullOut
{
    float3 PosL : POSITION;
};

[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("ConstantHS")]
[maxtessfactor(64.f)]
HullOut HS(InputPatch<VertexOut, 4> p, uint i : SV_OutputControlPointID, uint patchID : SV_PrimitiveID)
{
    HullOut hout;
    hout.PosL = p[i].PosL;
    return hout;
}

struct DomainOut
{
    float4 PosH : SV_POSITION;
};

DomainOut DS(PatchTess patchTess, float2 uv : SV_DomainLocation, const OutputPatch<HullOut, 4> quad)
{
    DomainOut dout;

    float3 p1 = lerp(quad[0].PosL, quad[1].PosL, uv.x);
    float3 p2 = lerp(quad[2].PosL, quad[3].PosL, uv.x);
    float3 p = lerp(p1, p2, uv.y);

    // Displacement mapping
    p.y = 0.3f*(p.z*sin(p.x) + p.x*cos(p.z));

    float 4 PosW = mul(float4(p, 1.f), gWorld);
    dout.PosH = mul(PosW, gViewProj);
    return dout;
}

float4 PS(DomainOut pin) : SV_Target
{
    return float4(1.f,1.f,1.f,1.f);
}