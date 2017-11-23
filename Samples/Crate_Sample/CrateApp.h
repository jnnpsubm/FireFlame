#pragma once
#include "..\Common\FLEngineApp.h"

struct NamedMaterialConstants : MaterialConstants
{
    std::string name;
};

class CrateApp : public FLEngineApp {
public:
    CrateApp(FireFlame::Engine& e) : FLEngineApp(e) { mRadius = 4.f; }
    void Initialize() override;

    void UpdateMainPassCB(float time_elapsed) override;
    void OnKeyUp(WPARAM wParam, LPARAM lParam) override;

private:
    void AddShaders();
    void AddTextures();
    void AddMaterials();
    void AddGeoMeshs();
    void AddRenderItems();

    std::unordered_map<std::string, NamedMaterialConstants> mMaterials;
};
