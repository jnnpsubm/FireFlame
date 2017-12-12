#pragma once
#include "..\Common\FLEngineApp3.h"

struct ObjConst2 : ObjectConsts
{
    int SubdivideLevel = 0;
};

class GSIcosahedronApp : public FLEngineApp3 {
public:
    GSIcosahedronApp(FireFlame::Engine& e) : FLEngineApp3(e)
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
    void AddShaderIcosahedron();

    void AddPSOs();

    void AddMaterials();

    void AddMeshs();
    void AddMeshIcosahedron();

    void AddRenderItems();
    void AddRenderItemsIcosahedron();
};
