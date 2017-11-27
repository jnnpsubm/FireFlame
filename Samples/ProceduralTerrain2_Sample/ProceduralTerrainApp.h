#pragma once
#include "..\Common\FLEngineApp.h"

struct NamedMaterialConstants : MaterialConstants
{
    std::string name;
};

class ProceduralTerrainApp : public FLEngineApp
{
public:
    ProceduralTerrainApp(FireFlame::Engine& e);

    void Initialize() override;
    void UpdateMainPassCB(float time_elapsed) override;

private:
    void BuildShaders();
    void BuildNoiseData();
    void BuildGeometry();
    void AddTextures();
    void AddMaterials();
    void BuildRenderItems();

    static const size_t pixel_width = 4096;
    static const size_t length = 32;
    std::unique_ptr<float[]> mNoiseData;

    std::unordered_map<std::string, NamedMaterialConstants> mMaterials;
};
