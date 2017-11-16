#pragma once
#include <unordered_map>
#include <functional>
#include "Primitive\FLD3DPrimitive.h"
#include "Vertex\FLVertex.h"
#include "..\FLTypeDefs.h"
#include "..\ShaderWrapper\FLD3DShaderWrapper.h"
#include "RenderItem\FLD3DRenderItem.h"

namespace FireFlame {
class D3DRenderer;
class StopWatch;
struct Pass;
class Scene {
public:
    typedef std::vector<D3DRenderItem*>                        VecRItem;
    typedef std::unordered_map<bool, VecRItem>                 PSOMappedVecRItem;
    typedef std::unordered_map<std::string, PSOMappedVecRItem> ShaderMappedVecRItem;
    typedef std::unordered_map<UINT, ShaderMappedVecRItem>     TopologyMappedVecRItem;

	Scene(std::shared_ptr<D3DRenderer>& renderer);

    // some scene management
    int  GetReady();

	void Update(const StopWatch& gt);
	void Render(const StopWatch& gt);

    void UpdateObjectCBs(const StopWatch& gt);

    void AddShader(const stShaderDescription& shaderDesc);
    void AddPrimitive(const stRawMesh& mesh);
	void AddPrimitive(const stRawMesh& mesh, const std::string& shaderName);
	void PrimitiveAddSubMesh(const std::string& name, const stRawMesh::stSubMesh& subMesh);

    void AddRenderItem
    (
        const std::string&      primitiveName,
        const std::string&      shaderName,
        const stRenderItemDesc& desc
    );
    void AddPass(const std::string& shaderName, const std::string& passName);

    void PrimitiveUseShader(const std::string& primitive, const std::string& shader);
    void RenderItemChangeShader(const std::string& renderItem, const std::string& shader);

    /*template <typename T>
    void UpdateShaderCBData(const std::string& shaderName, unsigned int index, const T& data) {
        auto it = mShaders.find(shaderName);
        if (it == mShaders.end()) return;
        auto& shader = it->second;
        shader->UpdateShaderCBData(index, data);
    }
    void UpdateShaderCBData(const std::string& shaderName, unsigned int index, size_t size, const void* data) {
        auto it = mShaders.find(shaderName);
        if (it == mShaders.end()) return;
        auto& shader = it->second;
        shader->UpdateShaderCBData(index, size, data);
    }*/
    void UpdateRenderItemCBData(const std::string& name, size_t size, const void* data);
    void UpdatePassCBData(const std::string& name, size_t size, const void* data);
    void UpdateMeshCurrVBFrameRes(const std::string& name, int index, size_t size, const void* data);

    // register callbacks
    void RegisterUpdateFunc(std::function<void(float)> func) { mUpdateFunc = func; }

private:
    std::shared_ptr<D3DRenderer> mRenderer;

    void PreRender();
    void Draw(ID3D12GraphicsCommandList* cmdList);
    void DrawPass(ID3D12GraphicsCommandList* cmdList, Pass* pass);

    // callbacks
    std::function<void(float)> mUpdateFunc = [](float) {};

	// todo : scene manage
    std::unordered_map<std::string, std::shared_ptr<D3DRenderItem>>    mRenderItems;
    // true for opaque
    TopologyMappedVecRItem                                             mMappedRItems;

    // passes
    std::unordered_map<std::string, std::shared_ptr<Pass>>             mPasses;

	std::unordered_map<std::string, std::unique_ptr<D3DPrimitive>>     mPrimitives; 
    std::unordered_map<std::string, std::shared_ptr<D3DShaderWrapper>> mShaders;
};
}