#pragma once
#include "..\Common\FLEngineApp2.h"

class MPSApp : public FLEngineApp2
{
public:
    MPSApp(FireFlame::Engine& e);

    void Initialize()                        override;

    void Update(float time_elapsed) override;

    void OnKeyUp(WPARAM wParam, LPARAM lParam) override;
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
    void AddUndeadLegionMesh();
    void AddUndeadLegionPart(size_t part, bool reverseNormal);

    void AddTextures();
    void AddFireKeeperTextures();
    void AddUndeadLegionTextures();

    void AddMaterials();
    void AddFireKeeperMaterials();
    void AddUndeadLegionMaterials();

    void AddRenderItems();
    void AddRenderItemFloor();
    void AddRenderItemWall();
    void AddRenderItemSkull();
    void AddRenderItemFireKeeper();
    void AddRenderItemUndeadLegion();
    void AddRenderItemUndeadLegionInst(int no, float inst_x, float inst_z);
    void AddRenderItemMirror();

    void AddPasses();

    void MoveSkull();
    void MoveFireKeeper();

    FireFlame::FLVERLoader mFireKeeperLoader;
    std::map<size_t, std::string> mTexMapFK;

    FireFlame::FLVERLoader mUndeadLegionLoader;
    std::map<size_t, std::string> mTexMapUL;

    unsigned long mNoiseTexWidth = 2048;
    std::unique_ptr<float[]> mNoiseData;

    FireFlame::Vector3f mSkullTranslation = { 12.0f, 1.0f, -5.0f };
    FireFlame::Vector3f mFKTranslation = { 0.0f, 0.0f, -4.0f };
    FireFlame::Vector3f* mTranslation = &mSkullTranslation;
};
