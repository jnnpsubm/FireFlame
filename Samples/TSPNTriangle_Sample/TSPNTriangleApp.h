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
    void LoadFBXModel(const std::string& fileName);
    void PrintFBXModelInfo(std::ofstream& out, const std::string& prefix, const fbx::FBXNode& node);

    void AddShaders();
    void AddShaderOctahedron();
    void AddShaderModel();
    void AddShaderModelTess();
    void AddShaderDSModel();

    void AddPSOs();

    void AddTextures();
    void AddMaterials();

    void AddMeshs();
    void AddMeshOctahedron();
    void AddMeshObjFromFile(const std::string& fileName);
    void AddMeshDSModel();
    void AddMeshDSModelPart(size_t part, bool reverseNormal);

    void AddRenderItems();
    void AddRenderItemOctahedron();
    void AddRenderItemModel();
    void AddRenderItemModelTess();
    void AddRenderItemDSModel();

    float mTessLod = 1.0f;

    fbx::FBXDocument mFBXModel;

    FireFlame::FLVERLoader mSilverKnightLoader;

    std::vector<std::string>  mSKTextures;
    std::vector<std::wstring> mSKTexFiles;

    std::string mCurrDSModelPart;
    int         mCurrDSMaterial = 0;
    std::map<std::string, int> mMapPartMat;
};
