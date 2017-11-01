#include <Windows.h>
#include <crtdbg.h>
#include <iostream>
#include <fstream>
#include <array>
#include "FireFlameHeader.h"
#include "Mesh.h"

struct ObjectConstants
{
    DirectX::XMFLOAT4X4 WorldViewProj = FireFlame::Matrix4X4();
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd) {
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	FireFlame::Engine engine(hInstance);
	try {
		using namespace FireFlame;
		engine.InitMainWindow(150, 80, 1280, 600);
		engine.InitRenderer(FireFlame::API_Feature::API_DX11On12);
		
		Mesh boxMesh;
		stRawMesh meshDesc;

		meshDesc.name = boxMesh.name;
		meshDesc.indexCount = (unsigned int)boxMesh.indices.size();
		meshDesc.indexFormat = FireFlame::Index_Format::UINT16;
		meshDesc.indices = boxMesh.indices.data();
		
		meshDesc.vertexCount = (unsigned int)boxMesh.vertices.size();
		meshDesc.vertexSize = sizeof(Mesh::Vertex);
		meshDesc.vertexFormat = VERTEX_FORMAT_POS_FLOAT_3 | VERTEX_FORMAT_COLOR_FLOAT_4;
		meshDesc.vertices = boxMesh.vertices.data();
		meshDesc.LocalToWorld = boxMesh.matrixLocal2World;

        stShaderDescription shader("color", L"Shaders\\color.hlsl", 1,
                                   { "VS","PS" }, { "vs_5_0","ps_5_0" }, 
                                   { "POSITION","COLOR" }, 
                                   stShaderDescription::Shader_VS | stShaderDescription::Shader_PS,
                                   {sizeof(ObjectConstants)});
		engine.GetScene()->AddPrimitive(meshDesc, shader);

		stRawMesh::stSubMesh subMesh;
		subMesh.name = "Box";
		subMesh.indexCount = meshDesc.indexCount;
		subMesh.startIndexLocation = 0;
		subMesh.baseVertexLocation = 0;
		engine.GetScene()->PrimitiveAddSubMesh(boxMesh.name, subMesh);
		
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