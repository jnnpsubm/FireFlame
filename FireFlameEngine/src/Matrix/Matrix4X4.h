#pragma once
#include <iostream>
#include <DirectXMath.h>

namespace FireFlame {
class Matrix4X4 {
#define VecGetX DirectX::XMVectorGetX
#define VecGetY DirectX::XMVectorGetY
#define VecGetZ DirectX::XMVectorGetZ
#define VecGetW DirectX::XMVectorGetW // todo : not elegant
public:
	union {
		struct {
			float m00, m01, m02, m03;
			float m10, m11, m12, m13;
			float m20, m21, m22, m23;
			float m30, m31, m32, m33;
		};
		float m[4][4];
	};

	Matrix4X4() {}
	explicit Matrix4X4(DirectX::FXMVECTOR u, DirectX::FXMVECTOR v, DirectX::FXMVECTOR w, DirectX::FXMVECTOR q)
		: m00(VecGetX(u)), m01(VecGetY(u)), m02(VecGetZ(u)), m03(VecGetW(u)),
		  m10(VecGetX(v)), m11(VecGetY(v)), m12(VecGetZ(v)), m13(VecGetW(v)),
		  m20(VecGetX(w)), m21(VecGetY(w)), m22(VecGetZ(w)), m23(VecGetW(w)),
		  m30(VecGetX(q)), m31(VecGetY(q)), m32(VecGetZ(q)), m33(VecGetW(q)) {}
	explicit Matrix4X4(DirectX::FXMMATRIX m)
		: m00(VecGetX(m.r[0])), m01(VecGetY(m.r[0])), m02(VecGetZ(m.r[0])), m03(VecGetW(m.r[0])),
		  m10(VecGetX(m.r[1])), m11(VecGetY(m.r[1])), m12(VecGetZ(m.r[1])), m13(VecGetW(m.r[1])),
		  m20(VecGetX(m.r[2])), m21(VecGetY(m.r[2])), m22(VecGetZ(m.r[2])), m23(VecGetW(m.r[2])),
		  m30(VecGetX(m.r[3])), m31(VecGetY(m.r[3])), m32(VecGetZ(m.r[3])), m33(VecGetW(m.r[3])) {}
	Matrix4X4(float _m00, float _m01, float _m02, float _m03,
		      float _m10, float _m11, float _m12, float _m13,
		      float _m20, float _m21, float _m22, float _m23,
		      float _m30, float _m31, float _m32, float _m33)
		: m00(_m00), m01(_m01), m02(_m02), m03(_m03),
		  m10(_m10), m11(_m11), m12(_m12), m13(_m13),
		  m20(_m20), m21(_m21), m22(_m22), m23(_m23),
		  m30(_m30), m31(_m31), m32(_m32), m33(_m33) {}

	float(&operator[](size_t i))[4]{
		return m[i];
	}
	const float(&operator[](size_t i) const)[4]{
		return m[i];
	}

	static Matrix4X4 LookAtLH(DirectX::FXMVECTOR EyePos, 
		                      DirectX::FXMVECTOR FocusPos, 
		                      DirectX::FXMVECTOR upDir){
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
	/*
	    | 1/[r*tan(a/2)]	0	          0	          0 |
		| 0	                1/tan(a/2)	  0	          0 |
        | 0                 0             f/(f-n)     1 |
	    | 0                 0             -n*f/(f-n)  0 |
	*/
	// fovY radians
	static Matrix4X4 PerspectiveFovLH(float fovY, float r, float n, float f) {
		float one_div_tanFovYDiv2 = 1.0f / std::tanf(fovY / 2.0f);
		return Matrix4X4(one_div_tanFovYDiv2*1.0f/r, 0,                   0,            0,
			             0,                          one_div_tanFovYDiv2, 0,            0,
			             0,                          0,                   f/(f - n),    1.0f,
			             0,                          0,                   -n*f/(f - n), 0);
	}
	/*
	f = B/(1-A);
	n = [(A-1)/A]*f
	todo : not considering real numbers stability...
	*/
	void ReversePerspectiveFovLH(float& fovY, float& r, float& n, float& f) const {
		float alphaDiv2 = std::atanf(1.0f/m[1][1]);
		if (alphaDiv2 < 0) alphaDiv2 += DirectX::XMVectorGetX(DirectX::g_XMTwoPi);
		fovY = 2.0f * alphaDiv2;

		double one_div_m00 = 1.0f / m[0][0];
		double one_div_m11 = 1.0f / m[1][1];
		r = (float)(one_div_m00 / one_div_m11);

		f = m[3][2] / (1.0f - m[2][2]);
		n = -m[3][2] / m[2][2];
	}
	Matrix4X4 Transpose() const {
		Matrix4X4 mT;
		for (size_t i = 0; i < 4; i++) {
			for (size_t j = 0; j < 4; j++) {
				mT[i][j] = (*this)[j][i];
			}
		}
		return mT;
	}

	Matrix4X4 operator*(const Matrix4X4& rhs) {
#define dot(r,c) m##r##0*rhs.m0##c+m##r##1*rhs.m1##c+m##r##2*rhs.m2##c+m##r##3*rhs.m3##c
		Matrix4X4 ret(dot(0, 0), dot(0, 1), dot(0, 2), dot(0, 3),
			          dot(1, 0), dot(1, 1), dot(1, 2), dot(1, 3),
			          dot(2, 0), dot(2, 1), dot(2, 2), dot(2, 3),
			          dot(3, 0), dot(3, 1), dot(3, 2), dot(3, 3));
		return ret;
#undef dot
	}

	operator DirectX::XMMATRIX() const {
		return DirectX::XMMatrixSet(m00, m01, m02, m03,
			m10, m11, m12, m13,
			m20, m21, m22, m23,
			m30, m31, m32, m33);
	}
	bool operator==(DirectX::FXMMATRIX rhs) const {
#define row_equal(row) \
m##row##0==DirectX::XMVectorGetX(rhs.r[##row##])&&m##row##1==DirectX::XMVectorGetY(rhs.r[##row##])&&\
m##row##2==DirectX::XMVectorGetZ(rhs.r[##row##])&&m##row##3==DirectX::XMVectorGetW(rhs.r[##row##])
		return row_equal(0) && row_equal(1) && row_equal(2) && row_equal(3);
#undef row_equal
	}
	bool nearlyEqual(DirectX::FXMMATRIX rhs) const {
		using std::fabs;
		static const float episilon = 0.00001f;
#define row_nearly_equal(row) \
fabs(m##row##0 - DirectX::XMVectorGetX(rhs.r[##row##]))<episilon && \
fabs(m##row##1 - DirectX::XMVectorGetY(rhs.r[##row##]))<episilon && \
fabs(m##row##2 - DirectX::XMVectorGetZ(rhs.r[##row##]))<episilon && \
fabs(m##row##3 - DirectX::XMVectorGetW(rhs.r[##row##]))<episilon
		return row_nearly_equal(0) && row_nearly_equal(1) &&
			row_nearly_equal(2) && row_nearly_equal(3);
#undef row_nearly_equal
	}

#undef VecGetX
#undef VecGetY
#undef VecGetZ
#undef VecGetW // todo : not elegant
};

inline
std::ostream& operator<<(std::ostream& os, DirectX::FXMMATRIX m) {
	for (int i = 0; i < 4; ++i) {
		os << DirectX::XMVectorGetX(m.r[i]) << '\t';
		os << DirectX::XMVectorGetY(m.r[i]) << '\t';
		os << DirectX::XMVectorGetZ(m.r[i]) << '\t';
		os << DirectX::XMVectorGetW(m.r[i]) << std::endl;
	}
	return os;
}

inline
std::ostream& operator<<(std::ostream& os, const Matrix4X4& m) {
	for (int i = 0; i < 4; ++i) {
		os << m[i][0] << '\t';
		os << m[i][1] << '\t';
		os << m[i][2] << '\t';
		os << m[i][3] << std::endl;
	}
	return os;
}
} // end namespace