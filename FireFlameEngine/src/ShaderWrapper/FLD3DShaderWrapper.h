#pragma once
#include "FLD3DShaderWrapperBase.h"
#include <vector>
#include <array>
#include <forward_list>
#include <map>
#include <unordered_map>
#include <memory>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include "..\Matrix\FLMatrix4X4.h"
#include "..\GPUMemory\D3DUploadBuffer.h"
#include "..\FrameResource\FLD3DFrameResource.h"

namespace FireFlame {
struct ShaderDescription;
class D3DShaderWrapper : public D3DShaderWrapperBase {
private:
    struct RootParamData
    {
        UINT descriptorOffset;
        UINT maxDescriptorSize;
        std::forward_list<UINT> freeList;
        bool inFrameResource;
    };

public:
    D3DShaderWrapper(const std::string& name) : D3DShaderWrapperBase(name) {}

    void SetTextureGroupSrvIndex(UINT index) { mTextureGroupSrvIndex = index; }
    UINT GetTextureGroupSrvIndex() const { return mTextureGroupSrvIndex; }
    bool UseDynamicMaterial() const { return mDynamicMaterials; }
    bool UseInstancing()      const { return mInstancing;       }
    ID3D12Resource* GetCurrentDynamicMatBuffer();
    ID3D12Resource* GetCurrentInstanceBuffer();

    void SetParamIndex(UINT texParamIndex, UINT objParamIndex, UINT multiObjParamIndex, UINT matParamIndex, UINT passParamIndex)
    {
        mTexParamIndex = texParamIndex;
        mObjParamIndex = objParamIndex;
        mMatParamIndex = matParamIndex;
        mPassParamIndex = passParamIndex;
    }
    void SetPassCbvIndex(UINT index) { mPassCbvIndex = index; }
    void UpdateObjCBData(unsigned int index, size_t size, const void* data);
    void UpdateMultiObjCBData(unsigned int index, size_t size, const void* data);
    void UpdatePassCBData(unsigned int index, size_t size, const void* data);

    void BuildRootSignature(ID3D12Device* device, const ShaderDescription& shaderDesc);
    void BuildRootSignatureNormal(ID3D12Device* device);
    void BuildRootSignatureDynamicMat(ID3D12Device* device, const ShaderDescription& shaderDesc);
    void BuildRootSignatureDynamicInstancing(ID3D12Device* device);
    void BuildRootSignatureUserDef(ID3D12Device* device, const ShaderDescription& shaderDesc);

    void BuildShadersAndInputLayout(const ShaderDescription& shaderDesc);
    void BuildRootInputResources
    (
        UINT objConstSize, UINT maxObjConstCount, bool Instancing,
        UINT passConstSize, UINT maxPassConstCount,
        UINT matConstSize, UINT maxMatConstCount, bool DynamicMat,
        UINT texSRVTableSize, UINT texSRVCount,
        UINT multiObjConstSize, UINT maxMultiObjConstCount
    );
    void BuildRootInputResources(const ShaderDescription& shaderDesc);

    UINT CreateTexSRV(ID3D12Resource* res);
    UINT CreateTexSRV(const std::vector<ID3D12Resource*>& vecRes);
    UINT CreateTexSRV(const std::vector<TEX>& vecTex);

    // Get Methods
    std::string           GetDefaultPassCBName() const { return mName + "_pass0"; }
    UINT                  GetPassCBVIndex()      const { return mPassCbvIndex;   }
    const std::string&    GetName()              const { return mName;           }

    std::pair<size_t, void*> GetInputLayout()   const 
    {
        return std::make_pair(mInputLayout.size(), (void*)mInputLayout.data()); 
    }
    std::pair<void*, size_t> GetVS(const std::string& macro) const
    {
        auto it = mVSByteCodes.find(macro);
        if (it == mVSByteCodes.end()) return std::make_pair(nullptr, 0);
        return std::make_pair
        (
            reinterpret_cast<void*>(it->second->GetBufferPointer()), 
            it->second->GetBufferSize()
        );
    }
    std::pair<void*, size_t> GetHS(const std::string& macro) const
    {
        auto it = mHSByteCodes.find(macro);
        if (it == mHSByteCodes.end()) return std::make_pair(nullptr, 0);
        return std::make_pair
        (
            reinterpret_cast<void*>(it->second->GetBufferPointer()),
            it->second->GetBufferSize()
        );
    }
    std::pair<void*, size_t> GetDS(const std::string& macro) const
    {
        auto it = mDSByteCodes.find(macro);
        if (it == mDSByteCodes.end()) return std::make_pair(nullptr, 0);
        return std::make_pair
        (
            reinterpret_cast<void*>(it->second->GetBufferPointer()),
            it->second->GetBufferSize()
        );
    }
    std::pair<void*, size_t> GetGS(const std::string& macro) const
    {
        auto it = mGSByteCodes.find(macro);
        if (it == mGSByteCodes.end()) return std::make_pair(nullptr, 0);
        return std::make_pair
        (
            reinterpret_cast<void*>(it->second->GetBufferPointer()),
            it->second->GetBufferSize()
        );
    }
    std::pair<void*, size_t> GetPS(const std::string& macro) const
    {
        auto it = mPSByteCodes.find(macro);
        if (it == mPSByteCodes.end()) return std::make_pair(nullptr, 0);
        return std::make_pair
        (
            reinterpret_cast<void*>(it->second->GetBufferPointer()),
            it->second->GetBufferSize()
        );
    }
    
    // todo : variant heaps with variant shaders
    ID3D12DescriptorHeap* GetCBVHeap()          const { return mCbvHeap.Get();              }
    UINT GetTexSRVDescriptorTableSize()         const { return mTexSrvDescriptorTableSize;  }
    UINT GetTexSrvOffset()                      const { return mTexSrvOffset;               }
    UINT GetMatCBVOffset()                      const { return mMaterialCbvOffset;          }
    UINT GetMaterialCBVMaxCount()               const { return mMatCbvMaxCount;             }
    UINT GetPassCBVMaxCount()                   const { return mPassCbvMaxCount;            }
    UINT GetMultiObjCBVMaxCount()               const { return mMultiObjCbvMaxCount; }
    UINT GetObjCBVMaxCount()                    const { return mObjCbvMaxCount;             }
    UINT GetTexParamIndex()                     const { return mTexParamIndex;              }
    UINT GetObjParamIndex()                     const { return mObjParamIndex;              }
    UINT GetMatParamIndex()                     const { return mMatParamIndex;              }
    UINT GetPassParamIndex()                    const { return mPassParamIndex;             }
    UINT GetMultiObjParamIndex()                const { return mMultiParamIndex; }
    UINT GetFreeObjCBV() { 
        if (mObjCbvHeapFreeList.empty())
            throw std::exception("todo : dynamically grow size of shader object const buff");
        UINT ret = mObjCbvHeapFreeList.front();
        mObjCbvHeapFreeList.pop_front();
        return ret; 
    }
    UINT GetFreeMultiObjCBV() {
        if (mMultiObjCbvHeapFreeList.empty())
            throw std::exception("todo : dynamically grow size of shader multiobject const buff");
        UINT index = mMultiObjCbvHeapFreeList.front();
        index += mMultiObjCbvOffset;
        mMultiObjCbvHeapFreeList.pop_front();
        return index;
    }
    UINT GetFreePassCBV() {
        if (mPassCbvHeapFreeList.empty())
            throw std::exception("todo : dynamically grow size of shader pass const buff");
        UINT index = mPassCbvHeapFreeList.front();
        index += mPassCbvOffset;
        // todo : pass cbv management
        mPassCbvHeapFreeList.pop_front();
        return index;
    }
    UINT GetFreeMatCBV()
    {
        if (mMatCbvHeapFreeList.empty())
            throw std::exception("todo : dynamically grow size of shader material const buff");
        UINT index = mMatCbvHeapFreeList.front();
        //index += mMaterialCbvOffset;
        // todo : material cbv management
        mMatCbvHeapFreeList.pop_front();
        return index;
    }

private:
    std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;
    std::vector<std::string>              mSemanticNames;

    std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>> mVSByteCodes;
    std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>> mHSByteCodes;
    std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>> mDSByteCodes;
    std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>> mGSByteCodes;
    std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>> mPSByteCodes;

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>   mCbvHeap              = nullptr;

    std::unordered_map<std::string, RootParamData> mRootParamData;

    bool                                           mDynamicMaterials = false;
    bool                                           mInstancing = false;
    UINT                                           mTextureGroupSrvIndex = -1;

    UINT                                           mTexSrvDescriptorTableSize = 4;

    UINT                                           mTexParamIndex = 0;
    UINT                                           mObjParamIndex = 1;
    UINT                                           mMultiParamIndex = 4;
    UINT                                           mMatParamIndex = 2;
    UINT                                           mPassParamIndex = 3;

    UINT                                           mPassCbvIndex = -1;

    UINT                                           mMaterialCbvOffset = 0;
    UINT                                           mPassCbvOffset = 0;
    UINT                                           mTexSrvOffset = 0;
    UINT                                           mMultiObjCbvOffset = 0;

    UINT                                           mPassCbvMaxCount = 0;
    UINT                                           mObjCbvMaxCount = 0;
    UINT                                           mMultiObjCbvMaxCount = 0;
    UINT                                           mMatCbvMaxCount = 0;

    std::forward_list<UINT>                        mObjCbvHeapFreeList;
    std::forward_list<UINT>                        mMultiObjCbvHeapFreeList;
    std::forward_list<UINT>                        mPassCbvHeapFreeList;
    std::forward_list<UINT>                        mMatCbvHeapFreeList;
    std::forward_list<UINT>                        mTexSrvHeapFreeList;

    void BuildInputLayout(const ShaderDescription& shaderDesc);
};

} // end namespace