#pragma once

namespace FireFlame {
namespace MathHelper {
template<typename T>
static T Clamp(const T& x, const T& low, const T& high) {
    return x < low ? low : (x > high ? high : x);
}
} // end namespace MathHelper
} // end namespace FireFlame

