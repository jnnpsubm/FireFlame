#pragma once
#include "..\Common\FLEngineApp.h"
#include "..\Common\Waves.h"

struct MaterialConstants2 : MaterialConstants
{
    int UseTexture = 0;
};

struct NamedMaterialConstants : MaterialConstants2
{
    std::string name;
};

class BlendApp : public FLEngineApp {
public:
    BlendApp(FireFlame::Engine& e) : FLEngineApp(e)
    { 
        mTheta = 1.5f*FireFlame::MathHelper::FL_PI;
        mPhi = FireFlame::MathHelper::FL_PIDIV2 - 0.1f;
        mRadius = 50.f;
    }
    void PreInitialize() override;
    void Initialize() override;

    void UpdateMainPassCB(float time_elapsed) override;
    void OnKeyUp(WPARAM wParam, LPARAM lParam) override;

private:
    void AddShaders();
    void AddTextures();
    void AddMaterials();
    void AddGeoMeshs();
    void AddWavesMesh();
    void AddBoxMesh();
    void AddLandMesh();
    void AddRenderItems();

    void UpdateWaves();

    float GetHillsHeight(float x, float z) const;
    FireFlame::Vector3f GetHillsNormal(float x, float z) const;

    bool                                                    mWaveStart = true;
    std::unique_ptr<Waves>                                  mWaves;
    std::unordered_map<std::string, NamedMaterialConstants> mMaterials;
    std::unordered_map<std::string, std::string>            mShaderMacrosVS;
    std::unordered_map<std::string, std::string>            mShaderMacrosPS;
};
