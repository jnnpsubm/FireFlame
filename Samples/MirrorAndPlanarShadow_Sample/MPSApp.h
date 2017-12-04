#pragma once
#include "..\Common\FLEngineApp2.h"

class MPSApp : public FLEngineApp2
{
public:
    MPSApp(FireFlame::Engine& e);

    void Initialize()                        override;
    void OnKeyboardInput(float time_elapsed) override;

private:
    void AddShaders();
    void AddPSOs();

    void AddMeshs();
    void AddRoomMesh();
    void AddSkullMesh();

    void AddTextures();
    void AddMaterials();

    void AddRenderItems();
    void AddRenderItemFloor();
    void AddRenderItemWall();
    void AddRenderItemSkull();
    void AddRenderItemMirror();

    void AddPasses();

    FireFlame::Vector3f mSkullTranslation = { 0.0f, 1.0f, -5.0f };
};
