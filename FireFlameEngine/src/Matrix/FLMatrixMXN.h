#pragma once
#include <iostream>
namespace FireFlame {
template<size_t N>
class VectorN{
public:
    float m[N];

    float operator*(const VectorN<N>& rhs) {
        float val = 0.f;
        for (size_t i = 0; i < N; i++)
        {
            val += m[i] * rhs.m[i];
        }
        return val;
    }
};

template <size_t M, size_t N>
class MatrixMXN {
public:
	float m[M][N];

	MatrixMXN() {}
	MatrixMXN(const float(&m_in)[M][N]) {
		for (auto i = 0; i < M; ++i) {
			for (auto j = 0; j < N; ++j) {
				m[j][i] = m_in[i][j];
			}
		}
	}
	MatrixMXN(std::initializer_list<float> li) {
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
	MatrixMXN<N, M> Transpose() const {
		MatrixMXN<N, M> m;
		for (auto i = 0; i < M; ++i) {
			for (auto j = 0; j < N; ++j) {
				m[j][i] = (*this)[i][j];
			}
		}
		return m;
	}

    /*template <size_t P>
    MatrixMXN<P, N> operator*(const MatrixMXN<P, M>& rhs) {
        MatrixMXN<P, N> M;
        memset(&M, 0, sizeof(M));
        for (size_t i = 0; i < P; i++)
        {
            for (size_t j = 0; j < M; j++)
            {
                M[i][j]
            }
        }
    }*/
};

template <size_t M, size_t N>
inline std::ostream& operator <<(std::ostream& os, const MatrixMXN<M, N>& m) {
	for (auto i = 0; i < M; ++i) {
		for (auto j = 0; j < N; ++j) {
			os << m[i][j] << '\t';
		}
		os << std::endl;
	}
	return os;
}
} // end namespace