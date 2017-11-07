#pragma once
#include <string>
#include <array>
#include "FireFlameHeader.h"
#include <DirectXPackedVector.h>

class Mesh {
public:
	struct Vertex {
		float x, y, z;
        float tx, ty, tz;
        float nx, ny, nz;
        float Tex1x, Tex1y;
        float Tex2x, Tex2y;
        UINT32 color;
        DirectX::PackedVector::XMCOLOR clr;
		Vertex(float _x, float _y, float _z,
               float _tx, float _ty, float _tz, 
               float _nx, float _ny, float _nz,
               float _Tex1x, float _Tex1y,
               float _Tex2x, float _Tex2y,
               UINT32 _color)
			: x(_x), y(_y), z(_z), 
              tx(_tx), ty(_ty), tz(_tz), 
              nx(_nx), ny(_ny), nz(_nz),
              Tex1x(_Tex1x), Tex1y(_Tex1y),
              Tex2x(_Tex2x), Tex2y(_Tex2y),
              color(_color)
		{}
	};

public:
	Mesh() = default;
    void GetMeshDesc(FireFlame::stRawMesh& meshDesc);
    void GetSubMeshDesc(FireFlame::stRawMesh::stSubMesh& subMesh);
	FireFlame::Matrix4X4 matrixLocal2World;
    std::array<Vertex, 8> vertices = {
        Vertex
        (
            -1.0f, -1.0f, -1.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f,
            0.0f, 0.0f,
            0xff000000
        ),
		Vertex
        (
            -1.0f, +1.0f, -1.0f, 
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f,
            0.0f, 0.0f,
            0xff000000
        ),
		Vertex
        (
            +1.0f, +1.0f, -1.0f, 
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f,
            0.0f, 0.0f,
            0xff000000
        ),
		Vertex
        (
            +1.0f, -1.0f, -1.0f, 
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f,
            0.0f, 0.0f,
            0xff000000
        ),
		Vertex
        (
            -1.0f, -1.0f, +1.0f, 
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f,
            0.0f, 0.0f,
            0xff000000
        ),
		Vertex
        (
            -1.0f, +1.0f, +1.0f, 
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f,
            0.0f, 0.0f,
            0xff000000
        ),
		Vertex
        (
            +1.0f, +1.0f, +1.0f, 
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f,
            0.0f, 0.0f,
            0xff000000
        ),
		Vertex
        (
            +1.0f, -1.0f, +1.0f, 
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f,
            0.0f, 0.0f,
            0xff000000
        )
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