#pragma once
#include <string>
#include <array>
#include "FireFlameHeader.h"

class Mesh {
public:
	struct VPosData {
		float x, y, z;
        VPosData(float _x, float _y, float _z) : x(_x), y(_y), z(_z){}
	};
    struct VColorData {
        float r, g, b, a;
        VColorData(float _r, float _g, float _b, float _a) : r(_r), g(_g), b(_b), a(_a){}
    };

public:
	Mesh() = default;
    void GetMeshDesc(FireFlame::stRawMesh& meshDesc);
    void GetSubMeshDesc(FireFlame::stRawMesh::stSubMesh& subMesh);
	FireFlame::Matrix4X4 matrixLocal2World;
	std::array<VPosData, 8> vertexPos = {
        VPosData(-1.0f, -1.0f, -1.0f),
        VPosData(-1.0f, +1.0f, -1.0f),
        VPosData(+1.0f, +1.0f, -1.0f),
        VPosData(+1.0f, -1.0f, -1.0f),
        VPosData(-1.0f, -1.0f, +1.0f),
        VPosData(-1.0f, +1.0f, +1.0f),
        VPosData(+1.0f, +1.0f, +1.0f),
        VPosData(+1.0f, -1.0f, +1.0f)
	};
    std::array<VColorData, 8> vertexColor = {
        VColorData(1.000000000f, 1.000000000f, 1.000000000f, 1.000000000f),
        VColorData(0.000000000f, 0.000000000f, 0.000000000f, 1.000000000f),
        VColorData(1.000000000f, 0.000000000f, 0.000000000f, 1.000000000f),
        VColorData(0.000000000f, 0.501960814f, 0.000000000f, 1.000000000f),
        VColorData(0.000000000f, 0.000000000f, 1.000000000f, 1.000000000f),
        VColorData(1.000000000f, 1.000000000f, 0.000000000f, 1.000000000f),
        VColorData(0.000000000f, 1.000000000f, 1.000000000f, 1.000000000f),
        VColorData(1.000000000f, 0.000000000f, 1.000000000f, 1.000000000f)
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