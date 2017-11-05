
cbuffer cbPerObject : register(b0)
{
	float4x4 gWorldViewProj; 
    float    gTexScale;
    int      gTurbulence;
    int      gTurbulenceModulate;
    int      gManualOctave;
    int      gOctave;

    int      gErosion;
    float    gErosionMin;
    float    gErosionMax;
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
    vout.TexC = vin.TexC*gTexScale;
    
    return vout;
}

// Perlin Noise Data
static const int NoisePermSize = 256;
static const int NoisePerm[2 * NoisePermSize] = {
    151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140,
    36, 103, 30, 69, 142,
    // Remainder of the noise permutation table
    8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62,
    94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33, 88, 237, 149, 56, 87, 174,
    20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166, 77,
    146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55,
    46, 245, 40, 244, 102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76,
    132, 187, 208, 89, 18, 169, 200, 196, 135, 130, 116, 188, 159, 86, 164, 100,
    109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123, 5, 202, 38, 147,
    118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28,
    42, 223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101,
    155, 167, 43, 172, 9, 129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232,
    178, 185, 112, 104, 218, 246, 97, 228, 251, 34, 242, 193, 238, 210, 144, 12,
    191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107, 49, 192, 214, 31,
    181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254,
    138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66,
    215, 61, 156, 180, 151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194,
    233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23, 190, 6,
    148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32,
    57, 177, 33, 88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74,
    165, 71, 134, 139, 48, 27, 166, 77, 146, 158, 231, 83, 111, 229, 122, 60,
    211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244, 102, 143, 54, 65, 25,
    63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196, 135,
    130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226,
    250, 124, 123, 5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59,
    227, 47, 16, 58, 17, 182, 189, 28, 42, 223, 183, 170, 213, 119, 248, 152, 2,
    44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9, 129, 22, 39, 253, 19,
    98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228, 251,
    34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249,
    14, 239, 107, 49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115,
    121, 50, 45, 127, 4, 150, 254, 138, 236, 205, 93, 222, 114, 67, 29, 24, 72,
    243, 141, 128, 195, 78, 66, 215, 61, 156, 180 
};

float Grad(int x, int y, int z, float dx, float dy, float dz) {
    int h = NoisePerm[NoisePerm[NoisePerm[x] + y] + z];
    h &= 15;
    float u = h < 8 || h == 12 || h == 13 ? dx : dy;
    float v = h < 4 || h == 12 || h == 13 ? dy : dz;
    return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}
float NoiseWeight(float t) {
    float t3 = t * t * t;
    float t4 = t3 * t;
    return 6 * t4 * t - 15 * t4 + 10 * t3;
}
float Noise(float x, float y, float z) {
    // Compute noise cell coordinates and offsets
    int ix = floor(x), iy = floor(y), iz = floor(z);
    Float dx = x - ix, dy = y - iy, dz = z - iz;

    // Compute gradient weights
    ix &= NoisePermSize - 1;
    iy &= NoisePermSize - 1;
    iz &= NoisePermSize - 1;
    float w000 = Grad(ix, iy, iz, dx, dy, dz);
    float w100 = Grad(ix + 1, iy, iz, dx - 1, dy, dz);
    float w010 = Grad(ix, iy + 1, iz, dx, dy - 1, dz);
    float w110 = Grad(ix + 1, iy + 1, iz, dx - 1, dy - 1, dz);
    float w001 = Grad(ix, iy, iz + 1, dx, dy, dz - 1);
    float w101 = Grad(ix + 1, iy, iz + 1, dx - 1, dy, dz - 1);
    float w011 = Grad(ix, iy + 1, iz + 1, dx, dy - 1, dz - 1);
    float w111 = Grad(ix + 1, iy + 1, iz + 1, dx - 1, dy - 1, dz - 1);

    // Compute trilinear interpolation of weights
    float wx = NoiseWeight(dx), wy = NoiseWeight(dy), wz = NoiseWeight(dz);
    float x00 = lerp(w000, w100, wx);
    float x10 = lerp(w010, w110, wx);
    float x01 = lerp(w001, w101, wx);
    float x11 = lerp(w011, w111, wx);
    float y0 =  lerp(x00, x10, wy);
    float y1 =  lerp(x01, x11, wy);
    return      lerp(y0, y1, wz);
}
float Noise(float3 p)
{
    return Noise(p.x, p.y, p.z);
}
float Noise(float2 p)
{
    return Noise(p.x, p.y, .5f);
}
float FBm(float3 p, float3 dpdx, float3 dpdy, float omega, int maxOctaves) {
    // Compute number of octaves for antialiased FBm
	float lenx = length(dpdx);
	float leny = length(dpdy);
	float len = max(lenx, leny);
	Float len2 = len*len;

    float n = clamp(-1 - .5f * log2(len2), 0, maxOctaves);
    int nInt = floor(n);

    // Compute sum of octaves of noise for FBm
    float sum = 0;
    float lambda = 1;
    float o = 1;
    for (int i = 0; i < nInt; ++i) {
        float3 pscale = lambda*p;
        sum += o * Noise(pscale.x, pscale.y, pscale.z);
        lambda *= 1.99f;
        o *= omega;
    }
    float nPartial = n - nInt;
    sum += o * smoothstep(.3f, .7f, nPartial) * Noise(lambda * p);
    return sum;
}
float FBm(float2 p, float2 dpdx, float2 dpdy, float omega, int maxOctaves) {
    // Compute number of octaves for antialiased FBm
	float lenx = length(dpdx);
	float leny = length(dpdy);
	float len = max(lenx, leny);
	Float len2 = len*len;

    float n = clamp(-1 - .5f * log2(len2), 0, maxOctaves);
    int nInt = floor(n);

    // Compute sum of octaves of noise for FBm
    float sum = 0;
    float lambda = 1;
    float o = 1;
    for (int i = 0; i < nInt; ++i) {
        float2 pscale = lambda*p;
        sum += o * Noise(pscale);
        lambda *= 1.99f;
        o *= omega;
    }
    float nPartial = n - nInt;
    sum += o * smoothstep(.3f, .7f, nPartial) * Noise(lambda * p);
    return sum;
}
float FBm(float2 p, float omega, int maxOctaves) {
    // Compute number of octaves for antialiased FBm
    int nInt = maxOctaves;

    // Compute sum of octaves of noise for FBm
    float sum = 0;
    float lambda = 1;
    float o = 1;
    for (int i = 0; i < nInt; ++i) {
        float2 pscale = lambda*p;
        sum += o * Noise(pscale);
        lambda *= 1.99f;
        o *= omega;
    }
    sum += o * .3f * Noise(lambda * p);
    return sum;
}
float Turbulence(float2 p, float2 dpdx, float2 dpdy, float omega, int maxOctaves) {
    // Compute number of octaves for antialiased FBm
	float lenx = length(dpdx);
	float leny = length(dpdy);
	float len = max(lenx,leny);
	Float len2 = len*len;
    Float n = clamp(-1 - .5f * log2(len2), 0, maxOctaves);
    int nInt = floor(n);

    // Compute sum of octaves of noise for turbulence
    Float sum = 0, lambda = 1, o = 1;
    for (int i = 0; i < nInt; ++i) {
        sum += o * abs(Noise(lambda * p));
        lambda *= 1.99f;
        o *= omega;
    }

    // Account for contributions of clamped octaves in turbulence
    Float nPartial = n - nInt;
    sum += o * lerp(0.2, abs(Noise(lambda * p)), smoothstep(.3f, .7f, nPartial));
    for (int i2 = nInt; i2 < maxOctaves; ++i2) {
        sum += o * 0.2f;
        o *= omega;
    }
    return sum;
}
float Turbulence(float2 p, float omega, int maxOctaves) {
    // Compute number of octaves for antialiased FBm
    int nInt = maxOctaves;

    // Compute sum of octaves of noise for turbulence
    Float sum = 0, lambda = 1, o = 1;
    for (int i = 0; i < nInt; ++i) {
        sum += o * abs(Noise(lambda * p));
        lambda *= 1.99f;
        o *= omega;
    }

    // Account for contributions of clamped octaves in turbulence
    sum += o * lerp(0.2, abs(Noise(lambda * p)), 0.3f);
    for (int i2 = nInt; i2 < maxOctaves; ++i2) {
        sum += o * 0.2f;
        o *= omega;
    }
    return sum;
}
float GetNoise(float2 p, float2 ddxT, float2 ddyT) {
    float val;
    if (!gTurbulence)
    {
        if (gManualOctave) {
            val = FBm(p, 0.5, gOctave);
        }
        else {
            val = FBm(p, ddxT, ddyT, 0.5, 128);
        }
        val = (val + 1.) / 2.;
    }
    else
    {
		if (gManualOctave) {
			val = Turbulence(p, 0.5, gOctave);
		}
		else
		{
			val = Turbulence(p, ddxT, ddyT, 0.5, 128);
		}
        if (gTurbulenceModulate)
        {
            val = (val + 1.) / 2.;
        }
    }
    return val;
}
float4 PS(VertexOut pin) : SV_Target
{
    //float3 color = float3(0.87f, 0.8078f, 0.745f);
    float3 color = float3(230.f/255.f, 192.f/255.f, 143.f/255.f);
    float2 p = float2(pin.TexC.x, pin.TexC.y);
    float val = 0.;
	float2 ddxTex = ddx_fine(pin.TexC);
	float2 ddyTex = ddy_fine(pin.TexC);
    val = GetNoise(p, ddxTex, ddyTex);
    if (gErosion)
    {
        clip(val - gErosionMin);
        clip(gErosionMax - val);
    }
    
    return float4(val*color,1.0);

	//float weight = max(max(ddxTex.x,ddxTex.y), max(ddyTex.x,ddyTex.y));
	//return float4(weight*color,1.f);
}


