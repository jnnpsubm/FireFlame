#include "PlyModelViewer.h"
#include <iostream>

int main(int argc, char *argv[])
{
    using namespace FireFlame;
    using namespace std::placeholders;

    if (argc < 2)
    {
        std::cerr << "input a ply file name to view......\n";
        system("pause");
        return 0;
    }

    HINSTANCE hInstance = GetModuleHandle(NULL);
    Engine            engine(hInstance);
    PlyModelViewer    viewer(engine);
    try {
        // application handles
        engine.RegisterUpdateFunc(std::bind(&PlyModelViewer::Update, &viewer, std::placeholders::_1));
        engine.RegisterWindowResizedHandler(std::bind(&PlyModelViewer::OnGameWindowResized, &viewer, _1, _2));
        engine.GetWindow()->RegisterMouseHandlers
        (
            std::bind(&PlyModelViewer::OnMouseDown, &viewer, _1, _2, _3),
            std::bind(&PlyModelViewer::OnMouseUp,   &viewer, _1, _2, _3),
            std::bind(&PlyModelViewer::OnMouseMove, &viewer, _1, _2, _3)
        );

        // engine initialization
        engine.InitMainWindow(150, 80, 1280, 600);
        engine.InitRenderer(API_Feature::API_DX12_1);

        // add shader, mesh, render item to scene
        viewer.Initialize(argv[1]);

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