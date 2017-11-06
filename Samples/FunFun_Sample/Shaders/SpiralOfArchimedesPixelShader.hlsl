#include "FFCommon.hlsli"

static const float XM_2PI = 6.283185307f;

float4 main(VertexOut pin) : SV_TARGET
{
    float y = pin.TexC.y;
    float x = pin.TexC.x;

    float pho = length(float2(x, y));
    float theta = atan2(y, x);      //                           theta->[-Pi,+Pi] 
    if (theta < 0) theta += XM_2PI; // [-Pi,0)+2Pi = [Pi,2Pi) => theta->[0,2Pi)

    float theta_p     = (pho) / g_a;
    float theta_p_min = (pho - gWidth / 2.f) / g_a;
    float theta_p_max = (pho + gWidth / 2.f) / g_a;
    float bound = gWidth / 2.0 / g_a; // edge bound

    int   circle = theta_p / XM_2PI;
    theta += circle * XM_2PI;

    /*if (theta > XM_2PI) {
        float dis = theta % XM_2PI;
        if (dis < bound) {
            theta -= XM_2PI;
        }
    }*/
    if (theta_p >= XM_2PI && theta_p <= XM_2PI + bound) {
        //theta -= XM_2PI;
    }
    else if (theta_p < XM_2PI && theta_p > XM_2PI - bound) {
        //theta += XM_2PI;
    }

    float dis = pho - g_a*theta;
    float step = smoothstep(0, gWidth / 2.f, abs(dis));
    
    //float pho_p = g_a*theta;
    //float step = smoothstep(0, gWidth / 2.f, abs(pho - pho_p));

    float3 color = lerp(gLineColor, gBackdropColor, step);
    return float4(color, 1.0f);
}