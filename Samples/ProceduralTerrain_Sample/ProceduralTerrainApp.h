#pragma once
#include "..\Common\FLEngineApp.h"

class ProceduralTerrainApp : public FLEngineApp
{
public:
    ProceduralTerrainApp(FireFlame::Engine& e);

    void Initialize() override;

private:
    void BuildShaders();
    void BuildGeometry();
    void BuildRenderItems();
};
