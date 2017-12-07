#pragma once
#include "..\Common\FLEngineApp3.h"
#include "..\Common\Waves.h"

struct DepthComplexityObjConsts
{
    FireFlame::Vector3f color;
    float reserve;
};

struct MaterialConstants2 : MaterialConstants
{
    int UseTexture = 0;
};

class TreeBillboardsApp : public FLEngineApp3 {
public:
    TreeBillboardsApp(FireFlame::Engine& e) : FLEngineApp3(e)
    { 
        mTheta = 1.5f*FireFlame::MathHelper::FL_PI;
        mPhi = FireFlame::MathHelper::FL_PIDIV2 - 0.1f;
        mRadius = 50.f;
    }
    void PreInitialize() override;
    void Initialize() override;

    void Update(float time_elapsed) override;
    void UpdateMainPassCB(float time_elapsed) override;
    void OnKeyUp(WPARAM wParam, LPARAM lParam) override;

private:
    void AddShaders();
    void AddShaderMain();
    void AddShaderDepthComplexity();

    void AddPSOs();

    void AddTextures();
    void AddMaterials();

    void AddMeshs();
    void AddWavesMesh();
    void AddBoxMesh();
    void AddLandMesh();
    void AddFullScreenRectMesh();

    void AddRenderItems();
    void AddRenderItemsNormal();
    void AddRenderItemsDepthComplexity();

    void UpdateWaves();

    float GetHillsHeight(float x, float z) const;
    FireFlame::Vector3f GetHillsNormal(float x, float z) const;

    bool                           mShowDepthComplexity = false;

    bool                                                    mWaveStart = true;
    std::unique_ptr<Waves>                                  mWaves;
};
