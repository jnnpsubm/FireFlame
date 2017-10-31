#include <Windows.h>
#include <crtdbg.h>
#include <iostream>
#include <fstream>
#include <array>
#include "FLEngine.h"
#include "FLMatrix.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd) {
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	FireFlame::Engine engine(hInstance);
	try {
		engine.InitMainWindow(150, 80, 1280, 600);
		engine.InitRenderer(FireFlame::API_Feature::API_DX11On12);

		struct Vertex {
			float x, y, z;
			float r, g, b, a;
			Vertex(float _x, float _y, float _z, float _r, float _g, float _b, float _a)
				:x(_x), y(_y), z(_z), r(_r), g(_g), b(_b), a(_a) {}
		};
		std::array<Vertex, 8> vertices = {
			Vertex(-1.0f, -1.0f, -1.0f, 1.000000000f, 1.000000000f, 1.000000000f, 1.000000000f),
			Vertex(-1.0f, +1.0f, -1.0f, 0.000000000f, 0.000000000f, 0.000000000f, 1.000000000f),
			Vertex(+1.0f, +1.0f, -1.0f, 1.000000000f, 0.000000000f, 0.000000000f, 1.000000000f),
			Vertex(+1.0f, -1.0f, -1.0f, 0.000000000f, 0.501960814f, 0.000000000f, 1.000000000f),
			Vertex(-1.0f, -1.0f, +1.0f, 0.000000000f, 0.000000000f, 1.000000000f, 1.000000000f),
			Vertex(-1.0f, +1.0f, +1.0f, 1.000000000f, 1.000000000f, 0.000000000f, 1.000000000f),
			Vertex(+1.0f, +1.0f, +1.0f, 0.000000000f, 1.000000000f, 1.000000000f, 1.000000000f),
			Vertex(+1.0f, -1.0f, +1.0f, 1.000000000f, 0.000000000f, 1.000000000f, 1.000000000f)
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
		FireFlame::Matrix4X4 matL2W;
		//engine.GetScene()->AddPrimitive<Vertex, std::uint16_t, 8, 36>("BoxGeo", vertices, indices,
			//FireFlame::VERTEX_POS3 | FireFlame::VERTEX_COLOR, matL2W);
		
		return engine.Run();
	}
	catch (FireFlame::Exception& e) {
		MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
		return 0;
	}
	catch (std::exception& e) {
		MessageBoxA(nullptr, e.what(), "Exception Raised!", MB_OK);
		return 0;
	}
}