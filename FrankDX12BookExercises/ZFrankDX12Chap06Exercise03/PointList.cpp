#include "PointList.h"
#include <random>

void PointList::Generate(size_t num) {
    static std::default_random_engine rng;
    static std::uniform_real_distribution<float> distPos(-1.f, 1.f);
    static std::uniform_real_distribution<float> distColor(0.f, 1.f);

    mPoints.resize(num);
    for (auto& vertex : mPoints) {
        vertex.x = distPos(rng);
        vertex.y = distPos(rng);
        vertex.z = distPos(rng);
        vertex.r = distColor(rng);
        vertex.g = distColor(rng);
        vertex.b = distColor(rng);
        vertex.a = 1.0f;
    }
}