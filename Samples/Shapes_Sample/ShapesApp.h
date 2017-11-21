#pragma once
#include "..\Common\FLEngineApp.h"

struct NamedMaterialConstants : MaterialConstants
{
    std::string name;
};

class ShapesApp : public FLEngineApp {
public:
    ShapesApp(FireFlame::Engine& e) : FLEngineApp(e) {}
    void Initialize() override;

    void UpdateMainPassCB(float time_elapsed) override;
    void OnKeyUp(WPARAM wParam, LPARAM lParam) override;

private:
    void BuildShaders();
    void AddMaterials();
    void BuildMesh();
    void AddRenderItems();
    void AddDragonRItem();

    void LoadSkull(const std::string& filePath);
    void LoadDragon(const std::string& filePath);
    void LoadOtherPlyModels();

    std::vector<FireFlame::Vector3f> mSpherePos;

    std::unordered_map<std::string, NamedMaterialConstants> mMaterials;
};
