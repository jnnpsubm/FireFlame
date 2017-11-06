#include "FFCommon.hlsli"

float4 main(VertexOut pin) : SV_TARGET
{
    double y = pin.TexC.y;
    double x = pin.TexC.x;

    double x2 = x*x;
    double y2 = y*y;
    double dis = x2 + y2 + g_a*x - g_a*sqrt(x2 + y2);
    double step = 0;
    if (dis > 0) {
        step = smoothstep(0., gWidth / 2., (float)dis);
    }
    else {
        step = 1. - smoothstep(-gWidth / 2., 0., (float)dis);
    }
    float3 color = lerp(gLineColor, gBackdropColor, (float)step);
    return float4(color, 1.0f);
}