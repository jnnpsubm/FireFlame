#pragma once

namespace FireFlame {
namespace MathHelper {
const float FL_PI = 3.141592654f;
const float FL_2PI = 6.283185307f;
const float FL_1DIVPI = 0.318309886f;
const float FL_1DIV2PI = 0.159154943f;
const float FL_PIDIV2 = 1.570796327f;
const float FL_PIDIV4 = 0.785398163f;

template<typename T>
static T Clamp(const T& x, const T& low, const T& high) {
    return x < low ? low : (x > high ? high : x);
}
} // end namespace MathHelper
} // end namespace FireFlame

