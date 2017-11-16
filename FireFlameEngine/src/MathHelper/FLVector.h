#pragma once
#include <math.h>

namespace FireFlame {
template<typename T>
class Vector2 {
public:
    union {
        struct {
            T x;
            T y;
        };
        T m[2];
    };
    Vector2(){}
    Vector2(T x, T y) :x(x), y(y) {}
};

template<typename T>
class Vector3 {
public:
    union {
        struct {
            T x;
            T y;
            T z;
        };
        T m[3];
    };
    Vector3() {}
    Vector3(T x, T y, T z) :x(x), y(y), z(z) {}

    Vector3<T> Normalize() {
        T len_inverse = (T)1.0 / std::sqrt(x*x + y*y + z*z);
        x *= len_inverse;
        y *= len_inverse;
        z *= len_inverse;
        return *this;
    }
};

template<typename T>
class Vector4 {
public:
    union {
        struct {
            T x;
            T y;
            T z;
            T w;
        };
        struct {
            T r;
            T g;
            T b;
            T a;
        };
        T m[4];
    };
    Vector4() {}
    Vector4(T x, T y, T z, T w) :x(x), y(y), z(z), w(w) {}

    Vector4<T> Normalize() {
        T len_inverse = (T)1.0 / std::sqrt(x*x + y*y + z*z + w*w);
        x *= len_inverse;
        y *= len_inverse;
        z *= len_inverse;
        w *= len_inverse;
        return *this;
    }
};

template<typename T>
inline Vector3<T> operator+(const Vector3<T>& lhs, const Vector3<T>& rhs) {
    return Vector3<T>(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}
template<typename T>
inline Vector2<T> operator+(const Vector2<T>& lhs, const Vector2<T>& rhs) {
    return Vector2<T>(lhs.x + rhs.x, lhs.y + rhs.y);
}
template<typename T>
inline Vector3<T> operator*(T val, const Vector3<T>& rhs) {
    return Vector3<T>(val*rhs.x, val*rhs.y, val*rhs.z);
}
template<typename T>
inline Vector2<T> operator*(T val, const Vector2<T>& rhs) {
    return Vector2<T>(val*rhs.x, val*rhs.y);
}

// cross product
template<typename T>
inline Vector3<T> Vector3Cross(const Vector3<T>& u, const Vector3<T>& v) {
    return Vector3<T>(u.y*v.z - u.z*v.y, u.z*v.x - u.x*v.z, u.x*v.y - u.y*v.x);
}

typedef Vector2<float> Vector2f;
typedef Vector3<float> Vector3f;
typedef Vector4<float> Vector4f;
}
