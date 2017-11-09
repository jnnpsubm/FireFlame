#include <Windows.h>
#include <crtdbg.h>
#include <iostream>
#include <fstream>
#include <array>
#include "FireFlameHeader.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd) {
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    FireFlame::Engine engine(hInstance);
    try {
        using namespace FireFlame;
        using namespace std::placeholders;

        // engine initialization
        engine.InitMainWindow(150, 80, 1280, 600);
        engine.InitRenderer(FireFlame::API_Feature::API_DX12_1);

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