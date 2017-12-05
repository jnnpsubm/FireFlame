#pragma once
#include "..\Common\FLEngineApp2.h"

class MPSApp : public FLEngineApp2
{
public:
    MPSApp(FireFlame::Engine& e);

    void Initialize()                        override;

    void Update(float time_elapsed) override;

    void OnKeyboardInput(float time_elapsed) override;

private:
    void BuildNoiseData();

    void AddShaders();
    void AddPSOs();

    void AddMeshs();
    void AddRoomMesh();
    void AddSkullMesh();
    void AddFireKeeperMesh();
    void AddFireKeeperPart(size_t part, bool reverseNormal);

    void AddTextures();
    void AddFireKeeperTextures();

    void AddMaterials();
    void AddFireKeeperMaterials();

    void AddRenderItems();
    void AddRenderItemFloor();
    void AddRenderItemWall();
    void AddRenderItemSkull();
    void AddRenderItemFireKeeper();
    void AddRenderItemMirror();

    void AddPasses();

    void MoveSkull();

    FireFlame::FLVERLoader mFireKeeperLoader;
    std::map<size_t, std::string> mTexMapFK;

    unsigned long mNoiseTexWidth = 2048;
    std::unique_ptr<float[]> mNoiseData;

    FireFlame::Vector3f mSkullTranslation = { 12.0f, 1.0f, -5.0f };
};
