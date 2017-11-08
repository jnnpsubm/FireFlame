#pragma once
#include <array>
#include "..\Common\Vertex.h"

class PyramidMesh {
public:
    std::array<VertexColored, 5> vertices = {
        VertexColored(-1.f, -1.f, -1.f, 0.f, 1.f, 0.f, 1.f),
        VertexColored(-1.f, -1.f, +1.f, 0.f, 1.f, 0.f, 1.f),
        VertexColored(+1.f, -1.f, +1.f, 0.f, 1.f, 0.f, 1.f),
        VertexColored(+1.f, -1.f, -1.f, 0.f, 1.f, 0.f, 1.f),
        VertexColored( 0.f, +1.f,  0.f, 1.f, 0.f, 0.f, 1.f)
    };
    std::array<std::int16_t, 18> indices = {
        0,3,1,
        1,3,2,
        0,1,4,
        0,4,3,
        2,3,4,
        1,2,4
    };
};