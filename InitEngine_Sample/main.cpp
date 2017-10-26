#include <Windows.h>
#include <crtdbg.h>
#include "FLEngine.h"
#include "EngineUser.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	try{
		FireFlame::Engine engine(hInstance);
		EngineUser someGame;

		auto funcDraw = std::bind(&EngineUser::Draw, someGame, std::placeholders::_1);
		auto funcUpdate = std::bind(&Update_OutClassScope, &someGame, std::placeholders::_1);

		engine.InitMainWindow(100, 10, 1024, 768);
		engine.RegisterRendererDrawFunc(funcDraw);
		engine.RegisterRendererUpdateFunc(funcUpdate);
		engine.RegisterRendererUpdateFunc(stEngineUser());
		return engine.Run();
	}
	catch (FireFlame::Exception& e){
		MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
		return 0;
	}
}