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
    void Update(float time_elapsed) override;
    void UpdateMainPassCB(float time_elapsed) override;

    void SetTerrainFile(const char* terrainFile) { mTerrainFile = terrainFile; }

private:
    std::string mTerrainFile;

    float  mModelScale = 1.0f;
    float  mModelTransY = 0.0f;
    float  mMinHeight = 0.f;
    float  mMaxHeight = 0.f;

    void BuildShaders();
    void BuildGeometry();
    void AddTextures();
    void AddMaterials();
    void BuildRenderItems();

    std::pair<std::uint8_t*,size_t> GetResource(int resID, const wchar_t* resType);

    std::unordered_map<std::string, NamedMaterialConstants> mMaterials;
};
