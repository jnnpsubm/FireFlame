#pragma once
#include "..\Common\FLEngineApp4.h"

class PickingApp : public FLEngineApp4 {
public:
    PickingApp(FireFlame::Engine& e) : FLEngineApp4(e) {}
    void PreInitialize() override;
    void Initialize() override;

    void Update(float time_elapsed) override;
    void UpdateMainPassCB(float time_elapsed) override;
    void OnKeyUp(WPARAM wParam, LPARAM lParam) override;

    void OnMouseDown(WPARAM btnState, int x, int y) override;

private:
    FireFlame::stRenderItemDesc mPickedRenderItem;
    void Pick(int sx, int sy);

    void AddShaders();

    void AddPSOs();

    void AddTextures();
    void AddMaterials();

    void AddMeshs();
    void AddMeshSkull();
    void AddMeshCar();

    void AddRenderItems();
};
