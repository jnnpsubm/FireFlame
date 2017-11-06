#include "FFCommon.hlsli"

static const float XM_2PI = 6.283185307f;

float4 main(VertexOut pin) : SV_TARGET
{
    float y = pin.TexC.y;
    float x = pin.TexC.x;

    // pho = a*cos(2*theta)
    float pho = length(float2(x, y));
    float theta = atan2(y, x);      //                           theta->[-Pi,+Pi] 
    if (theta < 0) theta += XM_2PI; // [-Pi,0)+2Pi = [Pi,2Pi) => theta->[0,2Pi)

    float pho_p = g_a * cos(2*theta);
    float dis = abs(pho_p - pho);
    float normDis = smoothstep(0.f, gWidth / 2.f, dis);
    float3 color = lerp(gLineColor, gBackdropColor, normDis);
    return float4(color, 1.0f);
}