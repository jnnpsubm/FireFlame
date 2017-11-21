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
    void BuildGeometry();
    void LoadSkull(const std::string& filePath);
    void AddMaterials();
    void BuildRenderItems();

    std::unordered_map<std::string, NamedMaterialConstants> mMaterials;
};
