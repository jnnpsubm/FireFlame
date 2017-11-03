
cbuffer cbPerObject : register(b0)
{
	float4x4 gWorldViewProj; 
    int bAntialiasing;
};

struct VertexIn
{
	float3 PosL : POSITION;
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
	
	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
	
	// Just pass vertex color into the pixel shader.
    vout.TexC = vin.TexC;
    
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    float3 theColor;
    float3 colorBrick = float3(0.8, 0.3, 0.0);
    float3 colorMortar = float3(0.9, 0.6, 0.4);
    float2 stBound = float2(0.94,0.88);
    float2 stEdge  = float2(0.05, 0.05);
    float2 st = pin.TexC;
    st = frac(st*float2(10.,10.));
    if (st.x<stBound.x && st.y<stBound.y)
    {
        if (bAntialiasing)
        {
            float2 dis2Bound = stBound - st; //((0.0, 0.0) , (stBound.x, stBound.y)]
            float dis = min(min(dis2Bound.x, dis2Bound.y), min(st.x, st.y));
            dis = clamp(dis, 0., stEdge.x);
            float step = smoothstep(0., stEdge.x, dis);
            theColor = step*colorBrick + (1. - step)*colorMortar;
        }
        else
        {
            theColor = colorBrick;
        }
        
    }
    else
    {
        theColor = colorMortar;
    }
    
    return float4(theColor,1.0f);
}


