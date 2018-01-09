#pragma once
#include <map>
#include <unordered_map>
#include <functional>
#include <queue>
#include <future>
#include <thread>
#include <mutex>
#include <chrono>
#include "Primitive\FLD3DPrimitive.h"
#include "CSTask\FLCSTask.h"
#include "Vertex\FLVertex.h"
#include "..\FLTypeDefs.h"

namespace FireFlame {
class D3DRenderer;
class D3DShaderWrapper;
class D3DComputeShaderWrapper;
class D3DPrimitive;
class StopWatch;
struct D3DRenderItem;
struct Pass;
struct Material;
struct D3DTextureBase;
struct PassConstBuffer;
struct MultiObjectConstBuffer;
class Scene {
public:
    typedef std::vector<D3DRenderItem*>                               VecRItem;
    typedef std::unordered_map<std::string, VecRItem>                 PSOMappedRItem;
    typedef std::unordered_map<std::string, PSOMappedRItem>           ShaderMappedRItem;
    typedef std::map<bool, ShaderMappedRItem>                         OpacityMappedRItem;
    typedef std::map<int, OpacityMappedRItem>                         PriorityMappedRItem;

    typedef std::pair<int, OpacityMappedRItem*>                       RItemsWithPriority;

	Scene(std::shared_ptr<D3DRenderer>& renderer);

    D3DShaderWrapper* GetShader(const std::string& name) const 
    {
        auto it = mShaders.find(name);
        if (it != mShaders.end()) return it->second.get();
        return nullptr;
    }

    D3DComputeShaderWrapper* GetComputeShader(const std::string& name) const
    {
        auto it = mComputeShaders.find(name);
        if (it != mComputeShaders.end()) return it->second.get();
        return nullptr;
    }

    D3DTextureBase* GetTexture(const std::string& name) const
    {
        auto it = mTextures.find(name);
        if (it != mTextures.end()) return it->second.get();
        return nullptr;
    }

    // some scene management
    int  GetReady();

	void Update(const StopWatch& gt);
	void Render(const StopWatch& gt);

    void Quit();

    void UpdateObjectCBs(const StopWatch& gt);
    void UpdateMaterialCBs(const StopWatch& gt);

    void PrimitiveVisible(const std::string& name, bool visible);
    void RenderItemVisible(const std::string& name, bool visible);
    void PrintScene() { mPrintScene = true; }

    void AddShader(const ShaderDescription& shaderDesc);
    void AddComputeShader(const ComputeShaderDescription& desc);

    void AddPSO(const std::string& name, const PSODesc& desc);
    void AddComputePSO(const std::string& name, const ComputePSODesc& desc);

    void SetCSRootParamData
    (
        const std::string& taskName, const std::string& shaderName, const std::string& paramName,
        const ResourceDesc& resDesc, size_t dataLen, std::uint8_t* data
    );

    void AddCSTask(const CSTaskDesc& desc);

    void AddPrimitive(const stRawMesh& mesh);
	void AddPrimitive(const stRawMesh& mesh, const std::string& shaderName);
	void PrimitiveAddSubMesh(const std::string& name, const stRawMesh::stSubMesh& subMesh);

    void AddRenderItem
    (
        const std::string&      primitiveName,
        const std::string&      shaderName,
        const stRenderItemDesc& desc
    )
    {
        return AddRenderItem(primitiveName, shaderName, "", "", desc);
    }
    void AddRenderItem
    (
        const std::string&      primitiveName,
        const std::string&      shaderName,
        const std::string&      shaderMacroVS,
        const std::string&      shaderMacroPS,
        const stRenderItemDesc& desc
    );
    void AddRenderItem
    (
        const std::string&      primitiveName,
        const std::string&      shaderName,
        const std::string&      PSOName,
        const stRenderItemDesc& desc
    ) 
    {
        return AddRenderItem(primitiveName, shaderName, PSOName, 0, desc);
    }
    // priority:0 > 1 > 2......
    void AddRenderItem
    (
        const std::string&      primitiveName,
        const std::string&      shaderName,
        const std::string&      PSOName,
        int                     priority,
        const stRenderItemDesc& desc
    )
    {
        return AddRenderItem(primitiveName, shaderName, PSOName, "", priority, desc);
    }
    void AddRenderItem
    (
        const std::string&      primitiveName,
        const std::string&      shaderName,
        const std::string&      PSOName,
        const std::string&      MultiObjCBName,
        int                     priority,
        const stRenderItemDesc& desc
    );
    void AddTexture(const std::string& name, const std::wstring& filename);
    void AddTextureGroup(const std::string& shaderName, const std::vector<TEX>& textures);
    void AddTexture(const std::string& name, std::uint8_t* data, size_t len);
    void AddTexture2D
    (
        const std::string& name, 
        const std::uint8_t* data,
        unsigned long format, 
        unsigned long width, 
        unsigned long height
    );
    // width height must be divideble by 16
    void AddTextureWaves
    (
        const std::string& name,
        unsigned width,
        unsigned height,
        unsigned disturbCount,
        float dx,
        float dt,
        float speed,
        float damping
    );
    void AnimateTexture(const std::string& name);
    void AddMaterial
    (
        const std::string& name,
        const std::string& shaderName,
        size_t dataLen, const void* data
    )
    {
        return AddMaterial(name, shaderName, "", dataLen, data);
    }
    void AddMaterial
    (
        const std::string& name,
        const std::string& shaderName,
        const std::string& texName,
        size_t dataLen, const void* data
    )
    {
        stMaterialDesc desc;
        desc.data = data;
        desc.dataLen = dataLen;
        desc.name = name;
        desc.shaderName = shaderName;
        if(!texName.empty()) desc.textures.emplace_back(texName);
        return AddMaterial(desc);
    }
    void AddMaterial(const stMaterialDesc& matDesc);
    void AddMultiObjCB(const std::string& shaderName, const std::string& name);

    void AddPassCB(const std::string& shaderName, const std::string& passName);
    void SetShaderPassCB(const std::string& shaderName, const std::string& passName);

    void AddPass(const std::string& name);

    void PrimitiveUseShader(const std::string& primitive, const std::string& shader);
    void RenderItemChangeShader
    (
        const std::string& renderItem, 
        int                priority,
        const stRenderItemDesc& desc,
        const std::string&      shader,
        const std::string&      shaderMacroVS = "",
        const std::string&      shaderMacroPS = ""
    );
    void RenderItemChangeMaterial
    (
        const std::string& renderItem,
        const std::string& matname
    );

    void UpdateRenderItemCBData(const std::string& name, size_t size, const void* data);
    
    void UpdateMaterialCBData(const std::string& name, size_t size, const void* data);
    void UpdateMultiObjCBData(const std::string& name, size_t size, const void* data);

    void UpdateShaderPassCBData(const std::string& shaderName, size_t size, const void* data);
    void UpdateShaderPassCBData(const std::string& shaderName, const std::string& passName, size_t size, const void* data);
    
    void UpdateMeshCurrVBFrameRes(const std::string& name, int index, size_t size, const void* data);

    // register callbacks
    void RegisterUpdateFunc(std::function<void(float)> func) { mUpdateFunc = func; }

private:
    std::shared_ptr<D3DRenderer> mRenderer;

    void PreRender();
    void Draw(ID3D12GraphicsCommandList* cmdList);
    void DrawPass(ID3D12GraphicsCommandList* cmdList, const Pass* pass);
    void DrawRenderItems
    (
        ID3D12GraphicsCommandList* cmdList,
        const Pass* pass,
        int priority,
        bool opaque
    );

    void UpdateShaderPassCBData(D3DShaderWrapper* shader, UINT CBIndex, size_t size, const void* data);

    void PrintAllRenderItems();
    void PrintAllShaders();
    void PrintAllPSOs();
    void PrintAllPasses();
    void PrintAllPassCBs();
    void PrintAllMultiObjCBs();
    void PrintAllPrimitives();
    void PrintAllMaterials();
    void PrintAllTextures();

    bool mPrintScene = false;

    // callbacks
    std::function<void(float)> mUpdateFunc = [](float) {};

	// todo : scene manage
    std::unordered_map<std::string, std::shared_ptr<D3DRenderItem>>    mRenderItems;
    // true for opaque
    ShaderMappedRItem& GetShaderMappedRItem(int priority, bool opaque) {
        auto& opacityMapped = mPriorityMappedRItems[priority];
        return opacityMapped[opaque];
    }
    PriorityMappedRItem                                                mPriorityMappedRItems;

    // passes
    std::unordered_map<std::string, std::shared_ptr<Pass>>             mPasses;

    std::unordered_map<std::string, std::shared_ptr<PassConstBuffer>>        mPassCBs;
    std::unordered_map<std::string, std::shared_ptr<MultiObjectConstBuffer>> mMultiObjCBs;

	std::unordered_map<std::string, std::shared_ptr<D3DPrimitive>>     mPrimitives;

    std::unordered_map<std::string, std::shared_ptr<D3DShaderWrapper>>        mShaders;
    std::unordered_map<std::string, std::shared_ptr<D3DComputeShaderWrapper>> mComputeShaders;

    // todo : move to primitive
    std::unordered_map<std::string, std::shared_ptr<Material>>         mMaterials;
    std::unordered_map<std::string, std::shared_ptr<D3DTextureBase>>      mTextures;

    // todo : get out of the scene
    bool                                                     mQuit = false;
    std::mutex                                               mComputeMutex;
    int Compute(std::shared_ptr<D3DRenderer> renderer);
    std::unique_ptr<std::future<int>>                        mCompute = nullptr;
    std::unordered_map<std::string, std::unique_ptr<CSTask>> mCSTasks;
    std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12CommandAllocator>> mCSTaskCmdAllocs;
};
}