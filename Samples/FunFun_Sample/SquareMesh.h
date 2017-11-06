#pragma once
#include <string>
#include <array>
#include "FireFlameHeader.h"

class SquareMesh {
public:
	struct Vertex {
		float x, y, z;
        float u, v;
		Vertex(float _x, float _y, float _z, float _u, float _v)
			:x(_x), y(_y), z(_z), u(_u), v(_v)
		{}
	};

public:
    SquareMesh() = default;
    void GetMeshDesc(FireFlame::stRawMesh& meshDesc);
    void GetSubMeshDesc(FireFlame::stRawMesh::stSubMesh& subMesh);
	FireFlame::Matrix4X4 matrixLocal2World;
	std::array<Vertex, 4> vertices = {
        // front face
        Vertex(-1.0f, -1.0f,  0.0f, -1.000000000f, -1.000000000f),
        Vertex(-1.0f, +1.0f,  0.0f, -1.000000000f, +1.000000000f),
        Vertex(+1.0f, +1.0f,  0.0f, +1.000000000f, +1.000000000f),
        Vertex(+1.0f, -1.0f,  0.0f, +1.000000000f, -1.000000000f)
	};
	std::array<std::uint16_t, 6> indices = {
		// front face
		0, 1, 2,
		0, 2, 3
	};
};