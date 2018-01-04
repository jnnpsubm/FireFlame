#pragma once
#include "..\Common\FLEngineApp3.h"

class TSIcosahedronApp : public FLEngineApp3 {
public:
    TSIcosahedronApp(FireFlame::Engine& e) : FLEngineApp3(e) {
        mTheta = 1.24f*DirectX::XM_PI;
        mPhi = 0.42f*DirectX::XM_PI;
        /*mTheta = 0;
        mPhi = 0;*/
        mRadius = 12.0f;
    }
    void PreInitialize() override;
    void Initialize() override;

    void Update(float time_elapsed) override;
    void UpdateMainPassCB(float time_elapsed) override;
    void OnKeyUp(WPARAM wParam, LPARAM lParam) override;

private:
    void AddShaders();
    void AddPSOs();
    void AddMaterials();
    void AddMeshs();
    void AddRenderItems();

    float mTessLod = 1.0f;
};
