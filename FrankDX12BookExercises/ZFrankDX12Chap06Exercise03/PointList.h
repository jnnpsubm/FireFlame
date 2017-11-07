#pragma once
#include "Common.h"
#include <vector>

class PointList {
public:
    void Generate(size_t num);
    void* GetData() { return mPoints.data(); }

private:
    std::vector<VertexColored> mPoints;
};