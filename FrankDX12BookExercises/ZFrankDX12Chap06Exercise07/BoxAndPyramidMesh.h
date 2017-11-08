#pragma once
#include <array>
#include "..\Common\Vertex.h"

class BoxAndPyramidMesh {
public:
    std::array<VertexColored, 13> vertices = {
        // box
        VertexColored(-1.0f, -1.0f, -1.0f, 1.000000000f, 1.000000000f, 1.000000000f, 1.000000000f),
        VertexColored(-1.0f, +1.0f, -1.0f, 0.000000000f, 0.000000000f, 0.000000000f, 1.000000000f),
        VertexColored(+1.0f, +1.0f, -1.0f, 1.000000000f, 0.000000000f, 0.000000000f, 1.000000000f),
        VertexColored(+1.0f, -1.0f, -1.0f, 0.000000000f, 0.501960814f, 0.000000000f, 1.000000000f),
        VertexColored(-1.0f, -1.0f, +1.0f, 0.000000000f, 0.000000000f, 1.000000000f, 1.000000000f),
        VertexColored(-1.0f, +1.0f, +1.0f, 1.000000000f, 1.000000000f, 0.000000000f, 1.000000000f),
        VertexColored(+1.0f, +1.0f, +1.0f, 0.000000000f, 1.000000000f, 1.000000000f, 1.000000000f),
        VertexColored(+1.0f, -1.0f, +1.0f, 1.000000000f, 0.000000000f, 1.000000000f, 1.000000000f),

        // pyramid
        VertexColored(-1.f, -1.f, -1.f, 0.f, 1.f, 0.f, 1.f),
        VertexColored(-1.f, -1.f, +1.f, 0.f, 1.f, 0.f, 1.f),
        VertexColored(+1.f, -1.f, +1.f, 0.f, 1.f, 0.f, 1.f),
        VertexColored(+1.f, -1.f, -1.f, 0.f, 1.f, 0.f, 1.f),
        VertexColored(0.f,  +1.f,  0.f, 1.f, 0.f, 0.f, 1.f)
    };
    std::array<VertexPackedColored, 13> verticesPacked = {
        // box
        VertexPackedColored(-1.0f, -1.0f, -1.0f, 0x00ff00ff),
        VertexPackedColored(-1.0f, +1.0f, -1.0f, 0x00ffffff),
        VertexPackedColored(+1.0f, +1.0f, -1.0f, 0xffff00ff),
        VertexPackedColored(+1.0f, -1.0f, -1.0f, 0x22ff12ff),
        VertexPackedColored(-1.0f, -1.0f, +1.0f, 0x00ff00ff),
        VertexPackedColored(-1.0f, +1.0f, +1.0f, 0x00ff11ff),
        VertexPackedColored(+1.0f, +1.0f, +1.0f, 0x00ff33ff),
        VertexPackedColored(+1.0f, -1.0f, +1.0f, 0x55ff00ff),

        // pyramid
        VertexPackedColored(-1.f, -1.f, -1.f, 0x00ff00ff),
        VertexPackedColored(-1.f, -1.f, +1.f, 0x00ff00ff),
        VertexPackedColored(+1.f, -1.f, +1.f, 0x00ff00ff),
        VertexPackedColored(+1.f, -1.f, -1.f, 0x00ff00ff),
        VertexPackedColored(0.f,  +1.f,  0.f, 0xffff0000)
    };
    std::array<std::uint16_t, 54> indices = {
        // box
        // front face
        0, 1, 2,
        0, 2, 3,

        // back face
        4, 6, 5,
        4, 7, 6,

        // left face
        4, 5, 1,
        4, 1, 0,

        // right face
        3, 2, 6,
        3, 6, 7,

        // top face
        1, 5, 6,
        1, 6, 2,

        // bottom face
        4, 0, 3,
        4, 3, 7,

        // pyramid
        0,3,1,
        1,3,2,
        0,1,4,
        0,4,3,
        2,3,4,
        1,2,4
    };
};