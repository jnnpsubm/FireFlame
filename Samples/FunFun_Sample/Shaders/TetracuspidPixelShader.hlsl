#include "FFCommon.hlsli"

float4 main(VertexOut pin) : SV_TARGET
{
    double y = pin.TexC.y;
    double x = pin.TexC.x;

    float k = 2.0 / 3.0;
    double dis = pow((float)x, k) + pow((float)y, k) - pow((float)g_a, k);
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