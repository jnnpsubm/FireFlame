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
struct Material;
struct Texture;
class Scene {
public:
    typedef std::vector<D3DRenderItem*>                               VecRItem;
    typedef std::unordered_map<bool, VecRItem>                        OpacityMappedVecRItem;
    typedef std::unordered_map<std::string, OpacityMappedVecRItem>    MacroMappedVecRItem;
    typedef std::unordered_map<std::string, MacroMappedVecRItem>      ShaderMappedVecRItem;
    typedef std::unordered_map<D3D12_CULL_MODE, ShaderMappedVecRItem> CullModeMappedVecRItem;
    typedef std::unordered_map<UINT, CullModeMappedVecRItem>          TopologyMappedVecRItem;

	Scene(std::shared_ptr<D3DRenderer>& renderer);

    // some scene management
    int  GetReady();

	void Update(const StopWatch& gt);
	void Render(const StopWatch& gt);

    void UpdateObjectCBs(const StopWatch& gt);
    void UpdateMaterialCBs(const StopWatch& gt);

    void PrintScene() { mPrintScene = true; }

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
    void AddRenderItem
    (
        const std::string&      primitiveName,
        const std::string&      shaderName,
        const std::string&      shaderMacros,
        const stRenderItemDesc& desc
    );
    void AddTexture(const std::string& name, const std::wstring& filename);
    void AddTexture2D
    (
        const std::string& name, 
        const std::uint8_t* data,
        unsigned long format, 
        unsigned long width, 
        unsigned long height
    );
    void AddMaterial
    (
        const std::string& name, 
        const std::string& shaderName, 
        size_t dataLen, const void* data
    );
    void AddMaterial
    (
        const std::string& name,
        const std::string& shaderName,
        const std::string& texName,
        size_t dataLen, const void* data
    );
    void AddMaterial(const stMaterialDesc& matDesc);
    void AddPass(const std::string& shaderName, const std::string& passName);

    void PrimitiveUseShader(const std::string& primitive, const std::string& shader);
    void RenderItemChangeShader
    (
        const std::string& renderItem, 
        const std::string& shader, 
        const std::string& shaderMacros = ""
    );

    void UpdateRenderItemCBData(const std::string& name, size_t size, const void* data);
    void UpdateMaterialCBData(const std::string& name, size_t size, const void* data);
    void UpdatePassCBData(const std::string& name, size_t size, const void* data);
    void UpdateMeshCurrVBFrameRes(const std::string& name, int index, size_t size, const void* data);

    // register callbacks
    void RegisterUpdateFunc(std::function<void(float)> func) { mUpdateFunc = func; }

private:
    std::shared_ptr<D3DRenderer> mRenderer;

    void PreRender();
    void Draw(ID3D12GraphicsCommandList* cmdList);
    void DrawPass(ID3D12GraphicsCommandList* cmdList, Pass* pass);
    void DrawRenderItems
    (
        ID3D12GraphicsCommandList* cmdList,
        OpacityMappedVecRItem& mappedRItems,
        D3DShaderWrapper* Shader,
        const std::string& shaderMacros,
        D3D12_PRIMITIVE_TOPOLOGY_TYPE topType,
        D3D12_CULL_MODE cullMode,
        bool opaque
    );

    bool mPrintScene = false;

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

    // todo : move to primitive
    std::unordered_map<std::string, std::shared_ptr<Material>>         mMaterials;
    std::unordered_map<std::string, std::shared_ptr<Texture>>          mTextures;
};
}