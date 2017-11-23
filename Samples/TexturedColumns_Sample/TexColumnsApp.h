#pragma once
#include "..\Common\FLEngineApp.h"

struct NamedMaterialConstants : MaterialConstants
{
    std::string name;
};

class TexColumnsApp : public FLEngineApp {
public:
    TexColumnsApp(FireFlame::Engine& e) : FLEngineApp(e) 
    { 
        mTheta = 1.5f*FireFlame::MathHelper::FL_PI;
        mPhi = 0.2f*FireFlame::MathHelper::FL_PI;
        mRadius = 15.0f;
    }
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
