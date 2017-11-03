#include <Windows.h>
#include <crtdbg.h>
#include <iostream>
#include <fstream>
#include "FireFlameHeader.h"
#include "EngineUser.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd){
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	FireFlame::Engine engine(hInstance);
	EngineUser someGame;
	try{
		auto funcDraw = std::bind(&EngineUser::Draw, someGame, std::placeholders::_1);
		auto funcUpdate = std::bind(&Update_OutClassScope, &someGame, std::placeholders::_1);

		engine.InitMainWindow(150, 80, 1280, 600);
		engine.InitRenderer(FireFlame::API_Feature::API_DX11_0_ON12);

		std::wofstream logFile(L"VideoAdapters.txt");
		engine.LogVideoAdapters(logFile);
		logFile.close();

		engine.RegisterRendererDrawFunc(funcDraw);
		engine.RegisterUpdateFunc(funcUpdate);
		engine.RegisterUpdateFunc(stEngineUser());
		return engine.Run();
	}catch (FireFlame::Exception& e){
		MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
		return 0;
	}
	catch (std::exception& e) {
		MessageBoxA(nullptr, e.what(), "Exception Raised!", MB_OK);
		return 0;
	}
}