#pragma once
#include "..\Common\Vertex.h"
#include <vector>

class PointList {
public:
    void Generate(size_t num);
    void* GetVertexData() { return mPoints.data(); }
    void* GetIndexData()  { return mIndices.data(); }

private:
    std::vector<VertexColored> mPoints;
    std::vector<std::int16_t>  mIndices;
};