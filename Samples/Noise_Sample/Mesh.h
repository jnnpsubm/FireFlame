#pragma once
#include <string>
#include <array>
#include "FireFlameHeader.h"

class Mesh {
public:
	struct Vertex {
		float x, y, z;
        float u, v;
		Vertex(float _x, float _y, float _z, float _u, float _v)
			:x(_x), y(_y), z(_z), u(_u), v(_v)
		{}
	};

public:
	Mesh() = default;
    void GetMeshDesc(FireFlame::stRawMesh& meshDesc);
    void GetSubMeshDesc(FireFlame::stRawMesh::stSubMesh& subMesh);
	FireFlame::Matrix4X4 matrixLocal2World;
	std::array<Vertex, 24> vertices = {
        // front face
        Vertex(-1.0f, -1.0f, -1.0f, 0.000000000f, 0.000000000f),
        Vertex(-1.0f, +1.0f, -1.0f, 0.000000000f, 1.000000000f),
        Vertex(+1.0f, +1.0f, -1.0f, 1.000000000f, 1.000000000f),
        Vertex(+1.0f, -1.0f, -1.0f, 1.000000000f, 0.000000000f),

        // back face
        Vertex(-1.0f, -1.0f, +1.0f, 0.000000000f, 0.000000000f),
        Vertex(-1.0f, +1.0f, +1.0f, 0.000000000f, 1.000000000f),
        Vertex(+1.0f, +1.0f, +1.0f, 1.000000000f, 1.000000000f),
        Vertex(+1.0f, -1.0f, +1.0f, 1.000000000f, 0.000000000f),

        // left face
        Vertex(-1.0f, -1.0f, +1.0f, 0.000000000f, 0.000000000f),
        Vertex(-1.0f, +1.0f, +1.0f, 0.000000000f, 1.000000000f),
        Vertex(-1.0f, +1.0f, -1.0f, 1.000000000f, 1.000000000f),
        Vertex(-1.0f, -1.0f, -1.0f, 1.000000000f, 0.000000000f),

        // right face
        Vertex(+1.0f, -1.0f, -1.0f, 0.000000000f, 0.000000000f),
        Vertex(+1.0f, +1.0f, -1.0f, 0.000000000f, 1.000000000f),
        Vertex(+1.0f, +1.0f, +1.0f, 1.000000000f, 1.000000000f),
        Vertex(+1.0f, -1.0f, +1.0f, 1.000000000f, 0.000000000f),

        // top face
        Vertex(-1.0f, +1.0f, -1.0f, 0.000000000f, 0.000000000f),
        Vertex(-1.0f, +1.0f, +1.0f, 0.000000000f, 1.000000000f),
        Vertex(+1.0f, +1.0f, +1.0f, 1.000000000f, 1.000000000f),
        Vertex(+1.0f, +1.0f, -1.0f, 1.000000000f, 0.000000000f),

        // bottom face
        Vertex(-1.0f, -1.0f, +1.0f, 0.000000000f, 0.000000000f),
        Vertex(-1.0f, -1.0f, -1.0f, 0.000000000f, 1.000000000f),
        Vertex(+1.0f, -1.0f, -1.0f, 1.000000000f, 1.000000000f),
        Vertex(+1.0f, -1.0f, +1.0f, 1.000000000f, 0.000000000f),
	};
	std::array<std::uint16_t, 36> indices = {
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		8, 9, 10,
        8, 10, 11,

		// right face
		12, 13, 14,
        12, 14, 15,

		// top face
		16, 17, 18,
        16, 18, 19,

		// bottom face
		20, 21, 22,
        20, 22, 23
	};
};