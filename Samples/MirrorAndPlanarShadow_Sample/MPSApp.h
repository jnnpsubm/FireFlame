#pragma once
#include "..\Common\FLEngineApp2.h"

class MPSApp : public FLEngineApp2
{
public:
    MPSApp(FireFlame::Engine& e);
    void Initialize() override;

private:
    void AddShaders();
    void AddPSOs();
    void AddMeshs();
    void AddTextures();
    void AddMaterials();
    void AddRenderItems();
    void AddPasses();

    void AddRoomMesh();
};
