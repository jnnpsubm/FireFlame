#pragma once
#include "..\Common\FLEngineApp3.h"

class BasicTessellationApp : public FLEngineApp3 {
public:
    BasicTessellationApp(FireFlame::Engine& e) : FLEngineApp3(e) {
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
    void AddMeshs();
    void AddRenderItems();
};
