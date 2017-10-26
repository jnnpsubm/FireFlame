#pragma once
#include <iostream>
#include <math.h>
#include "FLMatrix4X4.h"

namespace FireFlame {
struct stFLViewFrustum {
	stFLViewFrustum() = default;
	float l = 0.f, r = 0.f, t = 0.f, b = 0.f, n = 0.f, f = 0.f;
};

stFLViewFrustum GetViewFrustum(float fovY, float aspectRatio, float n, float f) {
	stFLViewFrustum frustum;
	float hdiv2 = n * std::tanf(fovY / 2.f);
	float wdiv2 = aspectRatio * hdiv2;
	frustum.l = -wdiv2; frustum.r = wdiv2;
	frustum.t = hdiv2; frustum.b = -hdiv2;
	frustum.n = n; frustum.f = f;
	return frustum;
}

std::ostream& operator<<(std::ostream& os, const stFLViewFrustum& frustum) {
	os << "left:\t" << frustum.l << "\tright:\t" << frustum.r << '\n'
	   << "top:\t" << frustum.t << "\tbottom:\t" << frustum.b << '\n'
	   << "near plane:\t" << frustum.n << "\tfar plane:\t" << frustum.f;
	return os;
}

/*============================World to Camera Matrix======================================*/
inline
Matrix4X4 MatrixLookAtLH(DirectX::FXMVECTOR EyePos,
	                     DirectX::FXMVECTOR FocusPos,
	                     DirectX::FXMVECTOR upDir) {
	using namespace DirectX;
	XMVECTOR w = XMVector3Normalize(FocusPos - EyePos);
	XMVECTOR u = XMVector3Normalize(XMVector3Cross(upDir, w));
	XMVECTOR v = XMVector3Cross(w, u);

	const XMVECTORU32 CXMSelect1110 = { { { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0U } } };
	XMVECTOR negtiveQ = -EyePos;
	XMVECTOR uQDoted = XMVectorSelect(XMVector3Dot(negtiveQ, u), u, CXMSelect1110);
	XMVECTOR vQDoted = XMVectorSelect(XMVector3Dot(negtiveQ, v), v, CXMSelect1110);
	XMVECTOR wQDoted = XMVectorSelect(XMVector3Dot(negtiveQ, w), w, CXMSelect1110);
	XMVECTOR q = XMVectorSet(0, 0, 0, 1.f);
	return Matrix4X4(uQDoted, vQDoted, wQDoted, q).Transpose();
}

/*=======================================Matrix Perspective===================================*/
/*  from rtr3 Page96

	| 2n/(r-l) 0        -(r+l)/(r-l)    0         |
	| 0        2n/(t-b) -(t+b)/(t-b)    0         |
	| 0        0        f/(f-n)         -fn/(f-n) |
	| 0        0        1               0         |
*/
inline
Matrix4X4 PerspectiveNearFarPlaneDX(float l, float r, float t, float b, float n, float f) {
	double one_div_r_l = 1.0 / (r - l);
	double one_div_t_b = 1.0 / (t - b);
	double one_div_f_n = 1.0 / (f - n);

	Matrix4X4 M;
	M[0][0] = (float)(2.0*n*one_div_r_l);
	M[0][2] = (float)(-(r + l)*one_div_r_l);
	M[1][1] = (float)(2.0*one_div_t_b);
	M[1][2] = (float)(-(t + b)*one_div_t_b);
	M[2][2] = (float)(f*one_div_f_n);
	M[2][3] = (float)(-f*n*one_div_f_n);
	M[3][2] = 1.f;
	return M.Transpose();
}
/*
	| 1/[r*tan(a/2)]	0	          0	          0 |
	| 0	                1/tan(a/2)	  0	          0 |
	| 0                 0             f/(f-n)     1 |
	| 0                 0             -n*f/(f-n)  0 |
*/
// fovY radians
inline
Matrix4X4 PerspectiveFovLH(float fovY, float r, float n, float f) {
	float one_div_tanFovYDiv2 = 1.0f / std::tanf(fovY / 2.0f);
	return Matrix4X4(one_div_tanFovYDiv2*1.0f/r, 0,                   0,            0,
		             0,                          one_div_tanFovYDiv2, 0,            0,
		             0,                          0,                   f/(f - n),    1.0f,
		             0,                          0,                   -n*f/(f - n), 0);
}
/*===================================End Matrix Perspective===================================*/
}