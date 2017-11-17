#pragma once
#include "..\Common\FLEngineApp.h"
#include "..\Common\Waves.h"

struct NamedMaterialConstants : MaterialConstants
{
    std::string name;
};

class LitWavesApp : public FLEngineApp
{
public:
    LitWavesApp(FireFlame::Engine& e);
    LitWavesApp(const LitWavesApp&) = delete;
    LitWavesApp& operator=(const LitWavesApp&) = delete;
    ~LitWavesApp() = default;

    void Initialize() override;
    void Update(float time_elapsed) override;
    void UpdateMainPassCB(float time_elapsed) override;

    void OnKeyboardInput(float time_elapsed) override;

private:
    std::unique_ptr<Waves> mWaves;

    bool                   mWaterMatDirty = false;
    bool                   mGrassMatDirty = false;
    NamedMaterialConstants mWaterMat;
    NamedMaterialConstants mGrassMat;

    void BuildShaders();
    void AddMaterials();
    void BuildLandGeometry();
    void BuildWavesGeometry();
    void BuildRenderItems();

    float GetHillsHeight(float x, float z) const;
    FireFlame::Vector3f GetHillsNormal(float x, float z) const;
};