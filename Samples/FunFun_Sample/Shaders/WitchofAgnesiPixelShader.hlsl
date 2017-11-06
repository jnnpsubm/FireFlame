#include "FFCommon.hlsli"

float4 main(VertexOut pin) : SV_TARGET
{
    float y = pin.TexC.y;
    float x = pin.TexC.x;

    float a2 = g_a*g_a;
    float dis = y - (8.f*a2*g_a) / (x*x + 4 * a2);
    float step = 0;
    if (dis > 0) {
        step = smoothstep(0.f, gWidth / 2, dis);
    }
    else {
        step = 1.f - smoothstep(-gWidth / 2, 0.f, dis);
    }
    float3 color = lerp(gLineColor, gBackdropColor, step);
    return float4(color, 1.0f);
}