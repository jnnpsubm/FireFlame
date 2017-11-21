#pragma once
#include <random>
#include "FLVector.h"

namespace FireFlame {
//namespace MathHelper {} // end namespace MathHelper
class MathHelper
{
public:
    // Returns random float in [0, 1).
    static float RandF()
    {
        // uncomment to use a non-deterministic seed  
        //std::random_device rd;  
        static std::default_random_engine e/*(rd())*/;
        static std::uniform_real_distribution<float> dist;
        return dist(e);
    }

    // Returns random float in [a, b).
    static float RandF(float a, float b)
    {
        static std::default_random_engine e;
        static std::uniform_real_distribution<float> dist(a, b);
        return dist(e);
    }

    static int Rand(int a, int b)
    {
        static std::default_random_engine e;
        static std::uniform_int_distribution<int> dist(a, b);
        return dist(e);
    }

    template<typename T, typename U, typename V>
    static T Clamp(T x, U low, V high)
    {
        return x < low ? low : (x > high ? high : x);
    }

    static double Lerp(double t, double v1, double v2) { return (1 - t) * v1 + t * v2; }
    static float Lerp(float t, float v1, float v2) { return (1 - t) * v1 + t * v2; }

    static float SmoothStep(float min, float max, float value) {
        float v = Clamp((value - min) / (max - min), 0, 1);
        return v * v * (-2 * v + 3);
    }
    static double SmoothStep(double min, double max, double value) {
        double v = Clamp((value - min) / (max - min), 0, 1);
        return v * v * (-2 * v + 3);
    }

    static float Log2(float x) {
        const float invLog2 = 1.442695040888963387004650940071f;
        return std::log(x) * invLog2;
    }
    static double Log2(double x) {
        const double invLog2 = 1.442695040888963387004650940071;
        return std::log(x) * invLog2;
    }

    static Vector3f SphericalToCartesian(float radius, float theta, float phi)
    {
        return Vector3f
        (
            radius*sinf(phi)*cosf(theta),
            radius*cosf(phi),
            radius*sinf(phi)*sinf(theta)
        );
    }

    static const float FL_PI;
    static const float FL_2PI;
    static const float FL_1DIVPI;
    static const float FL_1DIV2PI;
    static const float FL_PIDIV2;
    static const float FL_PIDIV4;
};

} // end namespace FireFlame

