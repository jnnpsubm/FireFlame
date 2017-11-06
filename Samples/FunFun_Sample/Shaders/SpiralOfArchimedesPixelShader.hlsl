#include "FFCommon.hlsli"

static const float XM_2PI = 6.283185307f;

float3 Archimides(float x, float y)
{
    float pho = length(float2(x, y));
    float theta = atan2(y, x);      //                           theta->[-Pi,+Pi] 
    if (theta < 0) theta += XM_2PI; // [-Pi,0)+2Pi = [Pi,2Pi) => theta->[0,2Pi)

    float theta_p = pho/g_a;
    int   circle  = theta_p/XM_2PI;
    float circles = circle*XM_2PI;
    theta += circles;
    theta_p -= circles;
    
    float pho_p = g_a * theta;
    float dis = abs(pho_p - pho);
    float normDis = smoothstep(0.f, gWidth / 2.f, dis);
    return lerp(gLineColor, gBackdropColor, normDis);
}

float3 Archimides2(float x, float y) 
{
    float pho = length(float2(x, y));
    float theta = atan2(y, x);      //                           theta->[-Pi,+Pi] 
    if (theta < 0) theta += XM_2PI; // [-Pi,0)+2Pi = [Pi,2Pi) => theta->[0,2Pi)

    float theta_p = pho/g_a;
    float theta_s = theta;
    int   circle  = theta_p/XM_2PI;
    float circles = circle*XM_2PI;
    theta += circles;
    theta_p -= circles;
    // bug fix
    float theta_edge = gWidth / 2.0 / g_a;
    if (theta_s<=theta_edge&&theta_p<=XM_2PI&&theta_p>=(XM_2PI-theta_edge))
    {
        theta += XM_2PI;
    }
    if (theta_s<=XM_2PI&&theta_s>=(XM_2PI-theta_edge)&&theta_p<=theta_edge)
    {
        theta -= XM_2PI;
    }
    // end
    float pho_p = g_a * theta;

    float dis = abs(pho_p - pho);
    float normDis = smoothstep(0.f, gWidth / 2.f, dis);
    return lerp(gLineColor, gBackdropColor, normDis);
}
float4 main(VertexOut pin) : SV_TARGET
{
    return float4(Archimides2(pin.TexC.x, pin.TexC.y), 1.0f);
}