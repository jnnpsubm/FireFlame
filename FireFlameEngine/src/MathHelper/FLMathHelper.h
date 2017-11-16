#pragma once
#include <random>

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

    template<typename T>
    static T Clamp(const T& x, const T& low, const T& high)
    {
        return x < low ? low : (x > high ? high : x);
    }

    static const float FL_PI;
    static const float FL_2PI;
    static const float FL_1DIVPI;
    static const float FL_1DIV2PI;
    static const float FL_PIDIV2;
    static const float FL_PIDIV4;
};

} // end namespace FireFlame

