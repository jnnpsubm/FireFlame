#include "PNDataGrad3D.hlsli"


float Grad(int x, int y, int z, float dx, float dy, float dz) {
	int h = NoisePerm[NoisePerm[NoisePerm[x] + y] + z];
	if (gUseRandomGrad) {
		float3 grad = RandomGrad[h];
		return dot(grad, float3(dx, dy, dz));
	}
	else {
		h &= 15;
		float u = h < 8 || h == 12 || h == 13 ? dx : dy;
		float v = h < 4 || h == 12 || h == 13 ? dy : dz;
		return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
	}
}

float Noise(float x, float y, float z) {
	// Compute noise cell coordinates and offsets
	int ix = floor(x), iy = floor(y), iz = floor(z);
	float dx = x - ix, dy = y - iy, dz = z - iz;

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
	float y0 = lerp(x00, x10, wy);
	float y1 = lerp(x01, x11, wy);
	return lerp(y0, y1, wz);
}
float Noise(float3 p) { return Noise(p.x, p.y, p.z); }

float FBm(float3 p, float3 dpdx, float3 dpdy,
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

float Turbulence(float3 p, float3 dpdx, float3 dpdy,
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
	sum += o * lerp(0.2,abs(Noise(lambda * p)),smoothstep(.3f, .7f, nPartial));
	for (int i2 = nInt; i2 < maxOctaves; ++i2) {
		sum += o * 0.2f;
		o *= omega;
	}
	return sum;
}

float FBm(float3 p, float omega, int maxOctaves) {
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

float Turbulence(float3 p, float omega, int maxOctaves) {
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

float GetNoise3D(float3 p, float3 ddxT, float3 ddyT) {
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