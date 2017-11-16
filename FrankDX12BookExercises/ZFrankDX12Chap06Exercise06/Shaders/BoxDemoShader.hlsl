


cbuffer cbPerObject : register(b0){
    float4x4 gWorldViewProj;
    float4   gPulseColor; // Chap06Exercise16
    float    gTime;
};

struct VertexIn{
    float3 PosL  : POSITION;
    float4 Color : COLOR;
};

struct VertexOut{
    float4 PosH  : SV_POSITION;
    float4 Color : COLOR;
};

VertexOut VS(VertexIn vin){
    VertexOut vout;
    vin.PosL.xy += 0.5f*sin(vin.PosL.x)*sin(3.0f*gTime);
    vin.PosL.z *= 0.6f + 0.4f*sin(2.0f*gTime);
    vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
    // Chap06Exercise14
    //float4 colorDest = float4(0.823529482f, 0.411764741f, 0.117647067f, 1.000000000f);
    //float s = (sin(gTime/2.f)+1.f)/2.f;
    //vin.Color = lerp(vin.Color, colorDest, s);
    // end
    vout.Color = vin.Color;
    return vout;
}

float4 PS(VertexOut pin) : SV_Target{
    // Chap06Exercise14
    //float4 colorDest = float4(0.823529482f, 0.411764741f, 0.117647067f, 1.000000000f);
    //float s = (sin(gTime / 2.f) + 1.f) / 2.f;
    //pin.Color = lerp(pin.Color, colorDest, s);
    // end
    
    // Chap06Exercise15
    //clip(pin.Color.r - 0.5f);
    // end

    // Chap06Exercise16
    //const float pi = 3.14159;
    //float s = 0.5f*sin(2 * gTime - 0.25f*pi) + 0.5f;
    //pin.Color = lerp(pin.Color, gPulseColor, s);
    // end
    return pin.Color;
}