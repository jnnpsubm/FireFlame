#include <Windows.h>
#include <crtdbg.h>
#include <iostream>
#include <fstream>
#include <array>
#include "FireFlameHeader.h"
#include "ShapesApp.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd) {
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    FireFlame::Engine engine(hInstance);
    ShapesApp         app(engine);
    try {
        using namespace FireFlame;
        using namespace std::placeholders;

        // application handles
        engine.RegisterUpdateFunc(std::bind(&FLEngineApp::Update, &app, std::placeholders::_1));
        engine.RegisterWindowResizedHandler(std::bind(&FLEngineApp::OnGameWindowResized, &app, _1, _2));
        engine.GetWindow()->RegisterMouseHandlers
        (
            std::bind(&FLEngineApp::OnMouseDown, &app, _1, _2, _3),
            std::bind(&FLEngineApp::OnMouseUp,   &app, _1, _2, _3),
            std::bind(&FLEngineApp::OnMouseMove, &app, _1, _2, _3)
        );
        engine.GetWindow()->RegisterKeyUpHandler(std::bind(&FLEngineApp::OnKeyUp, &app, _1, _2));

        // engine initialization
        engine.InitMainWindow(150, 80, 1280, 600);
        engine.InitRenderer(FireFlame::API_Feature::API_DX12_1);

        // add shader, mesh, render item to scene
        app.Initialize();

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