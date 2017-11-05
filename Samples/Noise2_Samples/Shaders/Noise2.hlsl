cbuffer cbPerObject : register(b0)
{
    float    gTexScale;
    int      gTurbulence;
    int      gManualOctave;
    int      gOctave;
	int      gUse2DNoise;
	int      gUseRandomGrad;
};

float NoiseWeight(float t) {
	float t3 = t * t * t;
	float t4 = t3 * t;
	return 6 * t4 * t - 15 * t4 + 10 * t3;
}

#include "PNDataPerm.hlsli"
#include "Noise3D.hlsli"
#include "Noise2D.hlsli"

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

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	vout.PosH = float4(vin.PosH, 1.f);
    vout.TexC = vin.TexC*gTexScale;
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    float3 color = float3(230.f/255.f, 192.f/255.f, 143.f/255.f);
	float val = 0.f;

	if (gUse2DNoise) {
		float2 p = float2(pin.TexC.x, pin.TexC.y);
		float2 ddxTex = float2(ddx_fine(pin.TexC));
		float2 ddyTex = float2(ddy_fine(pin.TexC));
		val = GetNoise2D(p, ddxTex, ddyTex);
	}
	else {
		float3 p = float3(pin.TexC.x, pin.TexC.y, .5f);
		float3 ddxTex = float3(ddx_fine(pin.TexC), 0.0f);
		float3 ddyTex = float3(ddy_fine(pin.TexC), 0.0f);
		val = GetNoise3D(p, ddxTex, ddyTex);
	}
	
    return float4(val*color,1.0);
}


