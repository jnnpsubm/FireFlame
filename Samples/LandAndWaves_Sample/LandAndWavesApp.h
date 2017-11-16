#pragma once
#include <memory>
#include "..\Common\FLEngineApp.h"
#include "Waves.h"

class LandAndWavesApp : public FLEngineApp
{
public:
    LandAndWavesApp(FireFlame::Engine& e);

    void Initialize() override;
    void Update(float time_elapsed) override;

private:
    std::unique_ptr<Waves> mWaves;

    void BuildShaders();
    void BuildLandGeometry();
    void BuildWavesGeometry();
    void BuildRenderItems();

    float GetHillsHeight(float x, float z) const;
};
