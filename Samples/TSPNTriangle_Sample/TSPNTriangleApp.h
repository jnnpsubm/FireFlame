#pragma once
#include "..\Common\FLEngineApp3.h"
#include "src\3rd_utils\fbx\fbxdocument.h"

class TSPNTriangleApp : public FLEngineApp3 {
public:
    TSPNTriangleApp(FireFlame::Engine& e) : FLEngineApp3(e) {
        mTheta = 1.24f*DirectX::XM_PI;
        mPhi = 0.42f*DirectX::XM_PI;
        /*mTheta = 0;
        mPhi = 0;*/
        mRadius = 12.0f;
    }
    void PreInitialize() override;
    void Initialize() override;

    void Update(float time_elapsed) override;
    void UpdateMainPassCB(float time_elapsed) override;
    void OnKeyUp(WPARAM wParam, LPARAM lParam) override;

private:
    void AddMeshObjFromFile(const std::string& fileName);

    void LoadFBXModel(const std::string& fileName);
    void PrintFBXModelInfo(std::ofstream& out, const std::string& prefix, const fbx::FBXNode& node);

    void AddShaders();
    void AddShaderOctahedron();
    void AddShaderModel();

    void AddPSOs();

    void AddTextures();
    void AddMaterials();

    void AddMeshs();
    void AddMeshOctahedron();

    void AddRenderItems();
    void AddRenderItemOctahedron();
    void AddRenderItemModel();

    float mTessLod = 1.0f;

    fbx::FBXDocument mFBXModel;
};
