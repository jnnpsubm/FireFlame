#pragma once
#include <iostream>
#include <DirectXMath.h>

namespace FireFlame {
template <size_t N>
class MatrixNXN {
private:
	template <size_t D>
	struct stMember : public stMember<D + 1> {
		stMember() :m(0.0f) {}
		template <typename... Args>
		stMember(float val, const Args&... rest) : m(val), stMember<D + 1>(rest...) {}
		float m;
	};
	template <>
	struct stMember<N*N> {
		stMember() :m(0.0f) {}
		stMember(float val) : m(val) {}
		float m;
	};
public:
	union {
		stMember<1> members; // todo:order reversed with m[N][N]...
		struct { float m[N*N]; } s; // stolen code
		float m[N][N];
	};
	template <typename... Args>
	MatrixNXN(Args... args) :s{ args... } {}
	//template <typename... Args>
	//MatrixNXN(Args&&... args) :members(std::forward<Args>(args)...) { }
	MatrixNXN() { }
	MatrixNXN(const float(&m_in)[N][N]) {
		for (auto i = 0; i < N; ++i) {
			for (auto j = 0; j < N; ++j) {
				m[i][j] = m_in[i][j];
			}
		}
	}
	MatrixNXN(std::initializer_list<float> li) { // todo bug
		float* entry = &m[0][0];
		memset(entry, 0, sizeof(m));
		for (auto val : li) {
			*entry++ = val;
		}
	}
	float(&operator[](size_t i))[N] {
		return m[i];
	}
	const float(&operator[](size_t i) const)[N] {
		return m[i];
	}
	float Determinant() const {
		float det = 0.0f;
		for (auto i = 0; i < N; ++i) {
			float detMinor = MinorMatrix(0, i).Determinant();
			det += m[0][i] * ((i & 1) ? -detMinor : detMinor);
		}
		return det;
	}
	MatrixNXN<N - 1> MinorMatrix(size_t r, size_t c) const {
		MatrixNXN<N - 1> minorM{};
		float* dest = &minorM[0][0];
		for (auto i = 0; i < N; ++i) {
			if (i == r) continue;
			for (auto j = 0; j < N; ++j) {
				if (j == c) continue;
				*dest++ = m[i][j];
			}
		}
		return minorM;
	}
	MatrixNXN<N> Inverse(float& det) const {
		det = Determinant();
		return AdjointMatrix() / det;// possibly inf or nan
	}
	MatrixNXN<N> AdjointMatrix() const {
		return CofactorMatrix().Transpose();
	}
	MatrixNXN<N> CofactorMatrix() const {
		MatrixNXN<N> CoM;
		for (size_t i = 0; i < N; i++) {
			for (size_t j = 0; j < N; j++) {
				float minorDet = MinorMatrix(i, j).Determinant();
				CoM[i][j] = (i + j) & 1 ? -minorDet : minorDet;
			}
		}
		return CoM;
	}
	MatrixNXN<N> Transpose() const {
		MatrixNXN mT;
		for (size_t i = 0; i < N; i++) {
			for (size_t j = 0; j < N; j++) {
				mT[i][j] = (*this)[j][i];
			}
		}
		return mT;
	}
	MatrixNXN<N> operator/(float rhs) const {
		MatrixNXN ret;
		for (size_t i = 0; i < N; i++) {
			for (size_t j = 0; j < N; j++) {
				ret[i][j] = m[i][j] / rhs;
			}
		}
		return ret;
	}
	operator DirectX::XMMATRIX() const;
};

template <>
class MatrixNXN<1> {
public:
	float m[1][1];
	float Determinant() const {
		return m[0][0];
	}
	float(&operator[](size_t i))[1]{
		return m[i];
	}
		const float(&operator[](size_t i) const)[1]{
		return m[i];
	}
};

template <>
inline MatrixNXN<4>::operator DirectX::XMMATRIX() const {
	return DirectX::XMMatrixSet(m[0][0], m[0][1], m[0][2], m[0][3],
		m[1][0], m[1][1], m[1][2], m[1][3],
		m[2][0], m[2][1], m[2][2], m[2][3],
		m[3][0], m[3][1], m[3][2], m[3][3]);
}

template <size_t N>
inline std::ostream& operator<<(std::ostream& os, const MatrixNXN<N>& m) {
	for (auto i = 0; i < N; ++i) {
		for (auto j = 0; j < N; ++j) {
			os << m[i][j] << '\t';
		}
		os << std::endl;
	}
	return os;
}
} // end namespace