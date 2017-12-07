#pragma once
#include <map>
#include <unordered_map>
#include <functional>
#include <queue>
#include "Primitive\FLD3DPrimitive.h"
#include "Vertex\FLVertex.h"
#include "..\FLTypeDefs.h"
#include "RenderItem\FLD3DRenderItem.h"

namespace FireFlame {
class D3DRenderer;
class D3DShaderWrapper;
class StopWatch;
struct Pass;
struct Material;
struct Texture;
struct MultiObjectConsts;
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

    // some scene management
    int  GetReady();

	void Update(const StopWatch& gt);
	void Render(const StopWatch& gt);

    void UpdateObjectCBs(const StopWatch& gt);
    void UpdateMaterialCBs(const StopWatch& gt);

    void PrimitiveVisible(const std::string& name, bool visible);
    void PrintScene() { mPrintScene = true; }

    void AddShader(const stShaderDescription& shaderDesc);
    void AddPSO(const std::string& name, const PSODesc& desc);
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
    void AddTextureGroup(const std::string& name, std::vector<std::wstring> filenames);
    void AddTexture(const std::string& name, std::uint8_t* data, size_t len);
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
        if(!texName.empty()) desc.texNames.push_back(texName);
        return AddMaterial(desc);
    }
    void AddMaterial(const stMaterialDesc& matDesc);
    void AddMultiObjCB(const std::string& shaderName, const std::string& name);

    void AddPass(const std::string& shaderName, const std::string& passName);

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
    void UpdatePassCBData(const std::string& name, size_t size, const void* data);
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
    void PrintAllRenderItems();
    void PrintAllPSOs();
    void PrintAllPasses();
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

    std::unordered_map<std::string, std::shared_ptr<MultiObjectConsts>> mMultiObjCBs;

	std::unordered_map<std::string, std::unique_ptr<D3DPrimitive>>     mPrimitives; 
    std::unordered_map<std::string, std::shared_ptr<D3DShaderWrapper>> mShaders;

    // todo : move to primitive
    std::unordered_map<std::string, std::shared_ptr<Material>>         mMaterials;
    std::unordered_map<std::string, std::shared_ptr<Texture>>          mTextures;
};
}