#include <Windows.h>
#include <crtdbg.h>
#include <iostream>
#include <fstream>
#include <array>
#include "FireFlameHeader.h"
#include "Mesh.h"
#include "Game.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd) {
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	FireFlame::Engine engine(hInstance);
    Game              someGame(engine);
	try {
		using namespace FireFlame;
        // engine initialization
		engine.InitMainWindow(150, 80, 1280, 600);
		engine.InitRenderer(FireFlame::API_Feature::API_DX11On12);
        engine.RegisterUpdateFunc(std::bind(&Game::Update, &someGame, std::placeholders::_1));
		
        // add some geometry to render
		Mesh                 boxMesh;
		stRawMesh            meshDesc("BoxMesh");
        stRawMesh::stSubMesh subMesh("Box");
        boxMesh.GetMeshDesc(meshDesc);
        boxMesh.GetSubMeshDesc(subMesh);
        engine.GetScene()->AddPrimitive(meshDesc);
        engine.GetScene()->PrimitiveAddSubMesh(meshDesc.name, subMesh);

        // use what shader to render the geometry
        stShaderDescription shader
        (
            "colorShader", 
            { VERTEX_FORMAT_POS_FLOAT_3 , VERTEX_FORMAT_COLOR_FLOAT_4 },
            { "POSITION","COLOR" }, 
            { sizeof(ObjectConstants) }
        );
        shader.AddShaderStage(L"Shaders\\color.hlsl", Shader_Type::VS, "VS", "vs_5_0");
        shader.AddShaderStage(L"Shaders\\color.hlsl", Shader_Type::PS, "PS", "ps_5_0");
        engine.GetScene()->AddShader(shader);

        // some initial work like scene management and 
        // make resource resident to GPU memory
        engine.GetScene()->GetReady(); 
		
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