#include "PNDataGrad2D.hlsli"


float Grad(int x, int y, float dx, float dy) {
	int h = NoisePerm[NoisePerm[x] + y];
	float2 grad = RandomGrad2D[h];
	return dot(grad, float2(dx, dy));
}

float Noise(float x, float y) {
	// Compute noise cell coordinates and offsets
	int ix = floor(x), iy = floor(y);
	float dx = x - ix, dy = y - iy;

	// Compute gradient weights
	ix &= NoisePermSize - 1;
	iy &= NoisePermSize - 1;
	float w00 = Grad(ix,     iy,     dx,     dy);
	float w10 = Grad(ix + 1, iy,     dx - 1, dy);
	float w01 = Grad(ix,     iy + 1, dx,     dy - 1);
	float w11 = Grad(ix + 1, iy + 1, dx - 1, dy - 1);

	// Compute trilinear interpolation of weights
	float wx = NoiseWeight(dx), wy = NoiseWeight(dy);
	float x00 = lerp(w00, w10, wx);
	float x10 = lerp(w01, w11, wx);
	return lerp(x00, x10, wy);
}
float Noise(float2 p) { return Noise(p.x, p.y); }

float FBm(float2 p, float2 dpdx, float2 dpdy,
	float omega, int maxOctaves) {
	// Compute number of octaves for antialiased FBm
	float len = max(length(dpdx), length(dpdy));
	float len2 = len*len;
	float n = clamp(-1 - .5f * log2(len2), 0, maxOctaves);
	int nInt = floor(n);

	// Compute sum of octaves of noise for FBm
	float sum = 0, lambda = 1, o = 1;
	for (int i = 0; i < nInt; ++i) {
		sum += o * Noise(lambda * p);
		lambda *= 1.99f;
		o *= omega;
	}
	float nPartial = n - nInt;
	sum += o * smoothstep(.3f, .7f, nPartial) * Noise(lambda * p);
	return sum;
}

float Turbulence(float2 p, float2 dpdx, float2 dpdy,
	float omega, int maxOctaves) {
	// Compute number of octaves for antialiased FBm
	float len = max(length(dpdx), length(dpdy));
	float len2 = len*len;
	float n = clamp(-1 - .5f * log2(len2), 0, maxOctaves);
	int nInt = floor(n);

	// Compute sum of octaves of noise for turbulence
	float sum = 0, lambda = 1, o = 1;
	for (int i = 0; i < nInt; ++i) {
		sum += o * abs(Noise(lambda * p));
		lambda *= 1.99f;
		o *= omega;
	}

	// Account for contributions of clamped octaves in turbulence
	float nPartial = n - nInt;
	sum += o * lerp(0.2, abs(Noise(lambda * p)), smoothstep(.3f, .7f, nPartial));
	for (int i2 = nInt; i2 < maxOctaves; ++i2) {
		sum += o * 0.2f;
		o *= omega;
	}
	return sum;
}

float FBm(float2 p, float omega, int maxOctaves) {
	// Compute number of octaves for antialiased FBm
	float n = maxOctaves;
	int nInt = floor(n);

	// Compute sum of octaves of noise for FBm
	float sum = 0, lambda = 1, o = 1;
	for (int i = 0; i < nInt; ++i) {
		sum += o * Noise(lambda * p);
		lambda *= 1.99f;
		o *= omega;
	}
	float nPartial = n - nInt;
	sum += o * smoothstep(.3f, .7f, nPartial) * Noise(lambda * p);
	return sum;
}

float Turbulence(float2 p, float omega, int maxOctaves) {
	// Compute number of octaves for antialiased FBm
	float n = maxOctaves;
	int nInt = floor(n);

	// Compute sum of octaves of noise for turbulence
	float sum = 0, lambda = 1, o = 1;
	for (int i = 0; i < nInt; ++i) {
		sum += o * abs(Noise(lambda * p));
		lambda *= 1.99f;
		o *= omega;
	}

	// Account for contributions of clamped octaves in turbulence
	float nPartial = n - nInt;
	sum += o * lerp(0.2, abs(Noise(lambda * p)), smoothstep(.3f, .7f, nPartial));
	for (int i2 = nInt; i2 < maxOctaves; ++i2) {
		sum += o * 0.2f;
		o *= omega;
	}
	return sum;
}

float GetNoise2D(float2 p, float2 ddxT, float2 ddyT) {
	float val;
	if (!gTurbulence) {
		if (gManualOctave) {
			val = FBm(p, 0.5, gOctave);
		}
		else {
			val = FBm(p, ddxT, ddyT, 0.5, 128);
		}
		val = (val + 1.) / 2.;
	}
	else {
		if (gManualOctave) {
			val = Turbulence(p, 0.5, gOctave);
		}
		else {
			val = Turbulence(p, ddxT, ddyT, 0.5, 128);
		}
	}
	return val;
}