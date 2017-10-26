#include <Windows.h>
#include <random>
#include <iostream>
#include <mmsystem.h>
#include "testMatrix.h"
#include "../FireFlameEngine/header/FLMatrix.h"

using namespace FireFlame;

int testMatrixPerspective() {
	SetCoutFormat(8);

	float fovY = DirectX::XMConvertToRadians(45);
	float aspectRatio = 4.0f / 3.0f;
	float n = 1.0f;
	float f = 100.0f;
	//DirectX::XMMATRIX XMM = DirectX::XMMatrixPerspectiveFovLH(fovY, aspectRatio, n, f);
	Matrix4X4 MPerFov = FireFlame::PerspectiveFovLH(fovY, aspectRatio, n, f);
	std::cout << "MPerFov = \n" << MPerFov << std::endl;
	//std::cout << "MPerFov equals XMM is " << (MPerFov == XMM) << std::endl;
	//std::cout << "MPerFov nearly equals XMM is " << MPerFov.nearlyEqual(XMM) << std::endl;

	SplitOutput();
	stFLViewFrustum frustum = GetViewFrustum(fovY, aspectRatio, n, f);
	std::cout << "Frustum from MPerFov:\n" << frustum << std::endl;

	SplitOutput();
	Matrix4X4 MPerNF = FireFlame::PerspectiveNearFarPlaneDX(
		frustum.l, frustum.r, frustum.t, frustum.b, frustum.n, frustum.f);
	std::cout << "MPerNF = \n" << MPerNF << std::endl;
	std::cout << "MPerFov equals MPerNF is " << (MPerFov == MPerNF) << std::endl;
	std::cout << "MPerFov nearly equals MPerNF is " << MPerFov.nearlyEqual(MPerNF) << std::endl;
	return 0;
}

int testReversePerspectiveFovLH() {
	std::cout.precision(8);
	std::cout.setf(std::ios_base::fixed);
	std::cout.setf(std::ios_base::boolalpha);

	Matrix4X4 M(1.86603f, 0,          0,        0,
		        0,        3.73205f,   0,        0,
		        0,        0,          1.02564f, 1.0f,
		        0,        0,          5.12821f, 0);
	float fovY, aspect, n, f;
	M.ReversePerspectiveFovLH(fovY, aspect, n, f);
	std::cout << "fovY:" << fovY << " aspectRation:" << aspect
		      << " near plane:" << n << " far plane:" << f << std::endl;

	Matrix4X4 M2 = FireFlame::PerspectiveFovLH(fovY, aspect, n, f);
	std::cout << "M = \n" << M << std::endl;
	std::cout << "M2 = \n" << M2 << std::endl;
	std::cout << "M equals M2 is " << (M == M2) << std::endl;
	std::cout << "M nearly equals M2 is " << M.nearlyEqual(M2) << std::endl;
	return 0;
}
int testPerspectiveFovLH() {
	std::cout.precision(8);
	std::cout.setf(std::ios_base::fixed);
	std::cout.setf(std::ios_base::boolalpha);

	float fovY = DirectX::XMConvertToRadians(45);
	float aspectRatio = 4.0f / 3.0f;
	float n = 1.0f;
	float f = 100.0f;
	DirectX::XMMATRIX XMM = DirectX::XMMatrixPerspectiveFovLH(fovY, aspectRatio, n, f);
	Matrix4X4 M = FireFlame::PerspectiveFovLH(fovY, aspectRatio, n, f);
	std::cout << "XMM = \n" << XMM << std::endl;
	std::cout << "M = \n" << M << std::endl;
	std::cout << "M equals XMM is " << (M == XMM) << std::endl;
	std::cout << "M nearly equals XMM is " << M.nearlyEqual(XMM) << std::endl;
	return 0;
}
int testWorld2CameraMatrix() {
	std::cout.precision(8);
	std::cout.setf(std::ios_base::boolalpha);
	DirectX::XMVECTOR eyePos = DirectX::XMVectorSet(-20.f, 35.f, -50.f, 1.f);
	DirectX::XMVECTOR focPos = DirectX::XMVectorSet(10.f, 0.f, 30.f, 1.f);
	DirectX::XMVECTOR upDir  = DirectX::XMVectorSet(0.f, 1.f, 0.f, 0.f);

	Matrix4X4 M = FireFlame::MatrixLookAtLH(eyePos, focPos, upDir);
	DirectX::XMMATRIX XMM = DirectX::XMMatrixLookAtLH(eyePos, focPos, upDir);
	std::cout << "M = \n" << M << std::endl;
	std::cout << "XMM = \n" << XMM << std::endl;
	std::cout << "M equals XMM is " << (M == XMM) << std::endl;
	std::cout << "M nearly equals XMM is " << M.nearlyEqual(XMM) << std::endl;
	return 0;
}

int testMatrixNXNInverse() {
	std::cout << "\n=====Testing MatrixNXN Inverse=====" << std::endl;

	std::cout.precision(8);
	std::cout.setf(std::ios_base::boolalpha);
	std::default_random_engine RNG(100);
	std::uniform_real_distribution<float> Dist(0.f, 1.f);
	MatrixNXN<4> M4X4(Dist(RNG), Dist(RNG), Dist(RNG), Dist(RNG),
		Dist(RNG), Dist(RNG), Dist(RNG), Dist(RNG),
		Dist(RNG), Dist(RNG), Dist(RNG), Dist(RNG),
		Dist(RNG), Dist(RNG), Dist(RNG), Dist(RNG));
	DirectX::XMMATRIX XMM4X4 = M4X4;
	std::cout << "M4X4 = \n" << M4X4 << std::endl;
	std::cout << "Determinant of M4X4 = " << M4X4.Determinant() << std::endl;
	std::cout << "Determinant of XMM4X4 = "
		<< DirectX::XMVectorGetX(DirectX::XMMatrixDeterminant(XMM4X4)) << std::endl;

	const size_t dim = 10;
	MatrixNXN<dim> MatrixBig;
	for (size_t i = 0; i < dim; i++) {
		for (size_t j = 0; j < dim; j++) {
			MatrixBig[i][j] = Dist(RNG);
		}
	}
	//std::cout << "Determinant of " << dim << " X " << dim 
	//<< " Matrix = " << MatrixBig.Determinant() << std::endl;

	std::cout << "\ntesting inverse..." << std::endl;
	float det;
	MatrixNXN<4> MInverse = M4X4.Inverse(det);
	DirectX::XMVECTOR detVector;
	DirectX::XMMATRIX XMMInverse = DirectX::XMMatrixInverse(&detVector, XMM4X4);
	std::cout << "MInverse = \n" << MInverse << std::endl;
	std::cout << "XMMInverse = \n" << XMMInverse << std::endl;
	return 0;
}

int testMatrixMXNTranspose() {
	std::cout << "\n=====Testing MatrixMXN Transpose=====" << std::endl;

	std::cout.precision(8);
	std::default_random_engine RNG(1000);
	std::uniform_real_distribution<float> Dist(0.f, 1.f);

	const size_t row = 5, col = 4;
	float m_arr[row][col];
	MatrixMXN<row, col> m(m_arr);
	for (auto i = 0; i < row; ++i) {
		for (auto j = 0; j < col; ++j) {
			m[i][j] = Dist(RNG);
		}
	}
	MatrixMXN<col, row> mTranspose = m.Transpose();
	std::cout << "m = \n" << m << std::endl;
	std::cout << "Transpose of m = \n" << m.Transpose() << std::endl;
	return 0;
}

int testDirectXMathSIMD() {
	std::cout << "\n=====Testing Matrix4X4 Product(SIMD)=====" << std::endl;

	std::cout.precision(16);
	std::cout.setf(std::ios_base::boolalpha);
	std::default_random_engine RND(10000/*timeGetTime()*/);
	std::uniform_real_distribution<float> DIST(0.f, .655f);

	Matrix4X4 M1(DIST(RND), DIST(RND), DIST(RND), DIST(RND),
		DIST(RND), DIST(RND), DIST(RND), DIST(RND),
		DIST(RND), DIST(RND), DIST(RND), DIST(RND),
		DIST(RND), DIST(RND), DIST(RND), DIST(RND));
	Matrix4X4 M2(DIST(RND), DIST(RND), DIST(RND), DIST(RND),
		DIST(RND), DIST(RND), DIST(RND), DIST(RND),
		DIST(RND), DIST(RND), DIST(RND), DIST(RND),
		DIST(RND), DIST(RND), DIST(RND), DIST(RND));
	DirectX::XMMATRIX XMM1 = M1;
	DirectX::XMMATRIX XMM2 = M2;

	Matrix4X4 M1XM2 = M1 * M2;
	DirectX::XMMATRIX XMM1XXMM2 = XMM1 * XMM2;
	std::cout << "M1 * M2 = " << std::endl << M1XM2 << std::endl;
	std::cout << "XMM1 * XMM2 = " << std::endl << XMM1XXMM2 << std::endl;
	std::cout << "M1 * M2 equal XMM1 * XMM2 is " << (M1XM2 == XMM1XXMM2) << std::endl;
	std::cout << "M1 * M2 nearly equal XMM1 * XMM2 is " << M1XM2.nearlyEqual(XMM1XXMM2) << std::endl;

	// SSE2 profile
	std::cout << "testing SIMD......" << std::endl;
	const size_t multiply_times = 10000 * 100;
	DirectX::XMMATRIX XMRet = XMM2;
	size_t loops = multiply_times;
	DWORD tmStart = timeGetTime();
	while (loops--) {
		XMRet = DirectX::XMMatrixMultiply(XMM1, XMRet);
	}
	DWORD tmEnd = timeGetTime();
	std::cout << multiply_times << " XMMatrix4X4 mutiplies, time consuming = " << tmEnd - tmStart << std::endl;

	// not using SSE2
	std::cout << "testing no SIMD......" << std::endl;
	Matrix4X4 Ret = M2;
	loops = multiply_times;
	tmStart = timeGetTime();
	while (loops--) {
		Ret = M1 * Ret;
	}
	tmEnd = timeGetTime();
	std::cout << multiply_times << " Matrix4X4 mutiplies, time consuming = " << tmEnd - tmStart << std::endl;

	std::cout << "XMRet = " << std::endl << XMRet << std::endl;
	std::cout << "Ret = " << std::endl << Ret << std::endl;
	std::cout << "Results nearly equal is " << Ret.nearlyEqual(XMRet) << std::endl;
	return 0;
}

void SetCoutFormat(std::streamsize precision) {
	std::cout.precision(precision);
	std::cout.setf(std::ios_base::fixed);
	std::cout.setf(std::ios_base::boolalpha);
}
void SplitOutput() {
	std::cout << "=============================================================" << std::endl;
}