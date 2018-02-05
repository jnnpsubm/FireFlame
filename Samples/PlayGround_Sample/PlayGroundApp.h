#pragma once
#include "..\Common\FLEngineApp4.h"

class PlayGroundApp : public FLEngineApp4 {
public:
    PlayGroundApp(FireFlame::Engine& e) : FLEngineApp4(e) {}
    void PreInitialize() override;
    void Initialize() override;

    void Update(float time_elapsed) override;
    void UpdateMainPassCB(float time_elapsed) override;
    void OnKeyUp(WPARAM wParam, LPARAM lParam) override;

private:
    void AddShaders();
    void AddShaderSky();
    void AddShaderTerrain();

    void AddPSOs();

    void AddTextures();
    void AddMaterials();

    void AddMeshs();
    void AddMeshTerrain();
    void AddMeshShapes();
    void AddMeshSkull();

    void AddRenderItems();
    void AddRenderItemSky();
    void AddRenderItemTerrain();
};
