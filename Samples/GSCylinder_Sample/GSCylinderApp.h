#pragma once
#include "..\Common\FLEngineApp3.h"

class GSCylinderApp : public FLEngineApp3 {
public:
    GSCylinderApp(FireFlame::Engine& e) : FLEngineApp3(e)
    { 
        /*mTheta = 1.5f*FireFlame::MathHelper::FL_PI;
        mPhi = FireFlame::MathHelper::FL_PIDIV2 - 0.1f;
        mRadius = 50.f;*/
    }
    void PreInitialize() override;
    void Initialize() override;

    void Update(float time_elapsed) override;
    void UpdateMainPassCB(float time_elapsed) override;

private:
    void AddShaders();
    void AddShaderCircle();
    void AddShaderCylinder();

    void AddPSOs();

    void AddMaterials();

    void AddMeshs();
    void AddMeshCircle();

    void AddRenderItems();
    void AddRenderItemsCircle();
    void AddRenderItemsCylinder();
};
