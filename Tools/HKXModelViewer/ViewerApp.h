#pragma once
#include "ViewerAppBase.h"

class ViewerApp : public ViewerAppBase
{
public:
    ViewerApp(FireFlame::Engine& e);

    void Initialize() override;

    void OnKeyUp(WPARAM wParam, LPARAM lParam) override;

private:
    //FireFlame::HKXLoader hkxloader;
    FireFlame::FLVERLoader flverloader;

    std::map<size_t, std::string> mTexMap;

    float mModelScale = 1.f;
    float mModelTransY = 0.f;

    void AddShaders();
    void AddPSOs();
    void AddTextures();
    void AddMaterials();
    void AddPasses();

    void AddFloor();
    void AddHKXModel(size_t part, bool reverseNormal);

    void DecideScaleAndTrans();
    void save_whole(FireFlame::FLVERLoader& loader);
    void save_parts(FireFlame::FLVERLoader& loader);
};
