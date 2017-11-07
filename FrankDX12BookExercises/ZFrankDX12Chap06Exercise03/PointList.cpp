#include "PointList.h"
#include <random>

void PointList::Generate(size_t num) {
    static std::default_random_engine rng;
    static std::uniform_real_distribution<float> distPos(-1.f, 1.f);
    static std::uniform_real_distribution<float> distColor(0.f, 1.f);

    mPoints.resize(num);
    std::uint16_t index = 0;
    for (auto& vertex : mPoints) {
        vertex.x = distPos(rng);
        vertex.y = distPos(rng);
        vertex.z = distPos(rng);
        vertex.r = distColor(rng);
        vertex.g = distColor(rng);
        vertex.b = distColor(rng);
        vertex.a = 1.0f;
        mIndices.push_back(index++);
    }
}

void PointList::Generate(const std::vector<Vertex>& vPos) {
    static std::default_random_engine rng;
    static std::uniform_real_distribution<float> distColor(0.f, 1.f);

    mPoints.resize(vPos.size());
    mIndices.resize(vPos.size());
    std::uint16_t index = 0;
    for (size_t i = 0; i < mPoints.size(); ++i) {
        mPoints[i].x = vPos[i].x;
        mPoints[i].y = vPos[i].y;
        mPoints[i].z = vPos[i].z;
        mPoints[i].r = distColor(rng);
        mPoints[i].g = distColor(rng);
        mPoints[i].b = distColor(rng);
        mPoints[i].a = 1.0f;
        mIndices[i] = index++;
    }
}