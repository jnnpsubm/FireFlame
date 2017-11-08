#include <Windows.h>
#include "FireFlameHeader.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd) {
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    FireFlame::Engine engine(hInstance);
    //PointListDemo         demo(engine);
    //LineStripDemo         demo(engine);
    //LineListDemo          demo(engine);
    //TriangleStripDemo     demo(engine);
    //TriangleListDemo     demo(engine);
    try {
        using namespace FireFlame;
        using namespace std::placeholders;

        // application handles
        /*engine.RegisterUpdateFunc(std::bind(&Demo::Update, &demo, std::placeholders::_1));
        engine.RegisterWindowResizedHandler(std::bind(&Demo::OnGameWindowResized, &demo, _1, _2));
        engine.GetWindow()->RegisterMouseHandlers
        (
            std::bind(&Demo::OnMouseDown, &demo, _1, _2, _3),
            std::bind(&Demo::OnMouseUp,   &demo, _1, _2, _3),
            std::bind(&Demo::OnMouseMove, &demo, _1, _2, _3)
        );*/

        // engine initialization
        engine.InitMainWindow(150, 80, 1280, 600);
        engine.InitRenderer(FireFlame::API_Feature::API_DX12_1);

        //engine.GetScene()->AddShader(demo.GetShaderDesc());
        //engine.GetScene()->AddPrimitive(demo.GetMeshDesc());
        //engine.GetScene()->PrimitiveUseShader(demo.GetMeshDesc().name, demo.GetShaderDesc().name);

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