#pragma once
#include <array>
#include "..\Common\Vertex.h"

class BoxMesh {
public:
    std::array<VertexColored, 8> vertices = {
        VertexColored(-1.0f, -1.0f, -1.0f, 1.000000000f, 1.000000000f, 1.000000000f, 1.000000000f),
        VertexColored(-1.0f, +1.0f, -1.0f, 0.000000000f, 0.000000000f, 0.000000000f, 1.000000000f),
        VertexColored(+1.0f, +1.0f, -1.0f, 1.000000000f, 0.000000000f, 0.000000000f, 1.000000000f),
        VertexColored(+1.0f, -1.0f, -1.0f, 0.000000000f, 0.501960814f, 0.000000000f, 1.000000000f),
        VertexColored(-1.0f, -1.0f, +1.0f, 0.000000000f, 0.000000000f, 1.000000000f, 1.000000000f),
        VertexColored(-1.0f, +1.0f, +1.0f, 1.000000000f, 1.000000000f, 0.000000000f, 1.000000000f),
        VertexColored(+1.0f, +1.0f, +1.0f, 0.000000000f, 1.000000000f, 1.000000000f, 1.000000000f),
        VertexColored(+1.0f, -1.0f, +1.0f, 1.000000000f, 0.000000000f, 1.000000000f, 1.000000000f)
    };
    std::array<std::uint16_t, 36> indices = {
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
        4, 3, 7
    };
};