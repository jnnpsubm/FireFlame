// basic tessellation sample

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
    float EdgeTess[3]   : SV_TessFactor;
    float InsideTess[1] : SV_InsideTessFactor;
};

PatchTess ConstantHS(InputPatch<VertexOut, 3> patch, uint patchID : SV_PrimitiveID)
{
    PatchTess patchTess;

    float3 centerL = 1.f/3.f*(patch[0].PosL + patch[1].PosL + patch[2].PosL);
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
    patchTess.InsideTess[0] = tess;
    return patchTess;
}

struct HullOut
{
    float3 PosL : POSITION;
};

[domain("tri")]
//[partitioning("integer")]
//[partitioning("fractional_odd")]
[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("ConstantHS")]
[maxtessfactor(64.f)]
HullOut HS(InputPatch<VertexOut, 3> p, uint i : SV_OutputControlPointID, uint patchID : SV_PrimitiveID)
{
    HullOut hout;
    hout.PosL = p[i].PosL;
    return hout;
}

struct DomainOut
{
    float4 PosH : SV_POSITION;
};

[domain("tri")]
DomainOut DS(PatchTess patchTess, float3 uvw : SV_DomainLocation, const OutputPatch<HullOut, 3> tri)
{
    DomainOut dout;

    float3 p = uvw.x*tri[0].PosL + uvw.y*tri[1].PosL + uvw.z*tri[2].PosL;

    // Displacement mapping
    p.y = 0.3f*(p.z*sin(p.x) + p.x*cos(p.z));

    float4 PosW = mul(float4(p, 1.f), gWorld);
    dout.PosH = mul(PosW, gViewProj);
    return dout;
}

float4 PS(DomainOut pin) : SV_Target
{
    return float4(1.f,1.f,1.f,1.f);
}