#include "ViewerApp.h"
#include <fstream>

int main()
{
    using namespace FireFlame;
    using namespace std::placeholders;

    Engine    engine(GetModuleHandle(NULL));
    ViewerApp viewer(engine);

    try {
        FireFlame::OpenConsole();

        // application handles
        engine.RegisterUpdateFunc(std::bind(&ViewerApp::Update, &viewer, std::placeholders::_1));
        engine.RegisterWindowResizedHandler(std::bind(&ViewerApp::OnGameWindowResized, &viewer, _1, _2));
        engine.GetWindow()->RegisterMouseHandlers
        (
            std::bind(&ViewerApp::OnMouseDown, &viewer, _1, _2, _3),
            std::bind(&ViewerApp::OnMouseUp, &viewer, _1, _2, _3),
            std::bind(&ViewerApp::OnMouseMove, &viewer, _1, _2, _3)
        );
        engine.GetWindow()->RegisterKeyUpHandler(std::bind(&ViewerApp::OnKeyUp, &viewer, _1, _2));

        // engine initialization
        engine.InitMainWindow(150, 80, 1280, 600);
        engine.InitRenderer(API_Feature::API_DX12_1);

        // add shader, mesh, render item to scene
        viewer.Initialize();

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
    
    return 0;
}