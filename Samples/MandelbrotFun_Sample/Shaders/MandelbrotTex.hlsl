
cbuffer cbPerObject : register(b0)
{
	float4x4 gWorldViewProj; 
    int gMaxIters = 10;
    double uTS = 1.;       // texture coordinate scaling
    float uCS = 10.;       // color scaling
    double uS0 = 0.3;      // starting texture value in S
    double uT0 = 0.5;      // starting texture value in T
    double uLimit = 10000000.0; // how large before stop iterations
    float3 convergeColor = float3(1., 0., 0.);
    float3 divergeColor1 = float3(0., 1., 0.);
    float3 divergeColor2 = float3(0., 0., 1.);
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
    double2 vST = pin.TexC;
    double real = vST.x * uTS + uS0;
    double imag = vST.y * uTS + uT0;
    double real0 = real;
    double imag0 = imag;
    double newr;
    int numIters;
    float4 color = float4(0., 0., 0., 1.);
    for (numIters = 0; numIters < gMaxIters; ++numIters)
    {
        double newreal = real0 + real*real - imag*imag;
        double newimag = imag0 + 2.*real*imag;
        newr = newreal*newreal + newimag*newimag;
        if (newr >= uLimit) break;
        real = newreal;
        imag = newimag;
    }

    // choose the color
    if (newr < uLimit)
    {
        color = float4(convergeColor, 1.);
        //color = float4(1., 0., 0., 1.);
    }
    else
    {
        color = float4(lerp(divergeColor1, divergeColor2, (float)frac(numIters / uCS)), 1.);
        //color = float4(1., 1., 0., 1.);
    }
    
    return color;
}


