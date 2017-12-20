#pragma once
#include "..\Common\FLEngineApp3.h"
#include "..\Common\Waves.h"
#include <stack>

struct DepthComplexityObjConsts
{
    FireFlame::Vector3f color;
    float reserve;
};

struct ObjectConsts2 : ObjectConsts
{
    float texSize[2];
    float dx;
    float pad1;
};

struct MaterialConstants2 : MaterialConstants
{
    MaterialConstants2(const MaterialConstants& m) :MaterialConstants(m) {}
    int UseTexture = 0;
};

class CSBlurApp : public FLEngineApp3 {
public:
    CSBlurApp(FireFlame::Engine& e) : FLEngineApp3(e)
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
    void AddShaderTree();
    void AddShaderImage();
    void AddShaderDepthComplexity();

    void AddPSOs();

    void AddTextures();
    void AddMaterials();

    void AddMeshs();
    void AddMeshWaves();
    void AddMeshGrid();
    void AddMeshBox();
    void AddMeshLand();
    void AddMeshTrees();
    void AddMeshImage();
    void AddMeshFullScreenRect();

    void AddRenderItems();
    void AddRenderItemsLand();
    void AddRenderItemsBox();
    void AddRenderItemsGrid();
    void AddRenderItemsWaves();
    void AddRenderItemsTree();
    void AddRenderItemsImage();
    void AddRenderItemsDepthComplexity();

    void UpdateWaves();

    float GetHillsHeight(float x, float z) const;
    FireFlame::Vector3f GetHillsNormal(float x, float z) const;

    std::unordered_map<std::string, std::string> mTreeShaderMacrosPS;

    bool                           mWaterOnly = false;
    bool                           mGpuWaves = true;
    bool                           mGpuWavesUseTex = true;

    bool                           mShowImage = false;
    bool                           mShowDepthComplexity = false;

    bool                                                    mWaveStart = true;
    std::unique_ptr<Waves>                                  mWaves;

    std::stack<std::string>                                mFilters;
};
