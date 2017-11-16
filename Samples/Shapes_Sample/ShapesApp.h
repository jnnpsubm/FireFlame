#pragma once
#include "..\Common\FLEngineApp.h"

class ShapesApp : public FLEngineApp {
public:
    ShapesApp(FireFlame::Engine& e) : FLEngineApp(e) {}
    void Initialize() override;

private:
    void BuildShaders();
    void BuildMesh();
    void BuildRenderItems();
};
