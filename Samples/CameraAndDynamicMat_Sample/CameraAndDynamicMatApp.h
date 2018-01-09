#pragma once
#include "..\Common\FLEngineApp4.h"

class CameraAndDynamicMatApp : public FLEngineApp4 {
public:
    CameraAndDynamicMatApp(FireFlame::Engine& e) : FLEngineApp4(e) {
        mTheta = 1.24f*DirectX::XM_PI;
        mPhi = 0.42f*DirectX::XM_PI;
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

    void AddTextures();
    void AddMaterials();

    void AddMeshs();

    void AddRenderItems();
};
