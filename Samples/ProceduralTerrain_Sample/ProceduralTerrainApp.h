#pragma once
#include "..\Common\FLEngineApp.h"

class ProceduralTerrainApp : public FLEngineApp
{
public:
    ProceduralTerrainApp(FireFlame::Engine& e) :FLEngineApp(e) {}

    void Initialize() override;

private:

};
