#include "FFCommon.hlsli"

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	vout.PosH = float4(vin.PosH, 1.f);
    vout.TexC = vin.TexC*gTexScale;
    return vout;
}




