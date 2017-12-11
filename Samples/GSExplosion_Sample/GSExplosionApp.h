#pragma once
#include "..\Common\FLEngineApp3.h"

struct ObjectConsts2 : ObjectConsts
{
    int  StartExplosion     = 0;
    float StartExplosionTime = 0.0f;
};

struct MaterialConstants2 : MaterialConstants
{
    int      UseTexture = 1;
    int      UseSpecularMap = 0;
};

struct NamedMaterialConstants2 : MaterialConstants2
{
    std::string Name;
};

class GSExplosionApp : public FLEngineApp3 {
public:
    GSExplosionApp(FireFlame::Engine& e) : FLEngineApp3(e)
    { 
        /*mTheta = 1.5f*FireFlame::MathHelper::FL_PI;
        mPhi = FireFlame::MathHelper::FL_PIDIV2 - 0.1f;
        mRadius = 50.f;*/
    }
    void PreInitialize() override;
    void Initialize() override;

    void OnKeyUp(WPARAM wParam, LPARAM lParam) override final;

    void Update(float time_elapsed) override;
    void UpdateMainPassCB(float time_elapsed) override;

private:
    void AddShaders();
    void AddShaderExplosion();
    void AddShaderExplosion2();

    void AddPSOs();

    void AddTextures();
    void AddTexturesUndeadLegion();

    void AddMaterials();
    void AddMaterialDragon();
    void AddMaterialUndeadLegion();

    void AddMeshs();
    void AddMeshDragon();
    void AddMeshUndeadLegion();
    void AddMeshULPart(size_t part, bool reverseNormal);

    void AddRenderItems();
    void AddRenderItemsDragon();
    void AddRenderItemsUL();

    float mModelScale = 1.f;
    float mModelTransY = 0.f;

    ObjectConsts2 mObjectConsts;

    FireFlame::FLVERLoader mUndeadLegionLoader;
    std::map<size_t, std::string> mTexMapUL;
    std::unordered_map<std::string, NamedMaterialConstants2>         mMaterials2;
    std::vector<std::string> mTexUL;
    std::vector<std::string> mTestMatUL;
    std::map<std::string, int> mPartMatMap;
    std::unordered_map<std::string, ObjectConsts2> mULPartsConsts;
};
