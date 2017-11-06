#include <Windows.h>
#include <crtdbg.h>
#include <iostream>
#include <fstream>
#include <array>
#include "FireFlameHeader.h"
#include "SquareMesh.h"
#include "Demo.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd) {
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	FireFlame::Engine engine(hInstance);
    Demo              someDemo(engine);
	try {
		using namespace FireFlame;
        using namespace std::placeholders;

        // application handles
        engine.RegisterUpdateFunc(std::bind(&Demo::Update, &someDemo, std::placeholders::_1));
        engine.RegisterWindowResizedHandler
        (
            std::bind(&Demo::OnGameWindowResized,&someDemo,_1,_2)
        );
        engine.GetWindow()->RegisterMouseHandlers
        (
            std::bind(&Demo::OnMouseDown, &someDemo, _1, _2, _3),
            std::bind(&Demo::OnMouseUp,   &someDemo, _1, _2, _3),
            std::bind(&Demo::OnMouseMove, &someDemo, _1, _2, _3)
        );
        engine.GetWindow()->RegisterKeyUpHandler(std::bind(&Demo::OnKeyUp,     &someDemo, _1, _2));
        engine.GetWindow()->RegisterKeyDownHandler(std::bind(&Demo::OnKeyDown, &someDemo, _1, _2));

        // engine initialization
		engine.InitMainWindow(150, 80, 600, 600);
		engine.InitRenderer(FireFlame::API_Feature::API_DX12_1);
        someDemo.SetMainWnd(engine.GetWindow()->MainWnd());
		
        // use what shader to render the geometry
        someDemo.AddShaders();

        // add some geometry to render
        someDemo.AddGeometry();

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