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

private:
    void BuildShaders();
    void AddMaterials();
    void BuildMesh();
    void BuildRenderItems();

    void LoadSkull(const std::string& filePath);

    std::vector<FireFlame::Vector3f> mSpherePos;

    std::unordered_map<std::string, NamedMaterialConstants> mMaterials;
};
