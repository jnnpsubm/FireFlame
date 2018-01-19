#pragma once
#include "..\Common\FLEngineApp4.h"

class CubeMapDynamicApp : public FLEngineApp4 {
public:
    CubeMapDynamicApp(FireFlame::Engine& e) : FLEngineApp4(e) {}
    void PreInitialize() override;
    void Initialize() override;

    void Update(float time_elapsed) override;
    void UpdateMainPassCB(float time_elapsed) override;
    void OnKeyUp(WPARAM wParam, LPARAM lParam) override;

private:
    void AddShaders();
    void AddShaderMain();
    void AddShaderSky();

    void AddPSOs();

    void AddTextures();
    void AddMaterials();

    void AddMeshs();
    void AddMeshShapes();
    void AddMeshSkull();

    void AddRenderItems();
};
