#pragma once
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
struct stShaderDescription;
class D3DShaderWrapper {
public:
    D3DShaderWrapper(const std::string& name) : mName(name) {}

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

    void BuildRootSignature(ID3D12Device* device);
    void BuildPSO(ID3D12Device*, DXGI_FORMAT, DXGI_FORMAT);
    //void BuildConstantBuffers(ID3D12Device* device, UINT CBSize);
    //void BuildCBVDescriptorHeaps(ID3D12Device* device, UINT numDescriptors);
    void BuildShadersAndInputLayout(const stShaderDescription& shaderDesc);
    void BuildFrameCBResources
    (
        UINT objConstSize, UINT maxObjConstCount,
        UINT passConstSize, UINT maxPassConstCount,
        UINT matConstSize, UINT maxMatConstCount,
        UINT texSRVTableSize, UINT texSRVCount,
        UINT multiObjConstSize, UINT maxMultiObjConstCount
    );
    UINT CreateTexSRV(ID3D12Resource* res);
    UINT CreateTexSRV(const std::vector<ID3D12Resource*>& vecRes);

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
#ifndef TEX_SRV_USE_CB_HEAP
    ID3D12DescriptorHeap* GetTexSRVHeap()       const { return mTexSrvDescriptorHeap.Get(); }
#endif
    ID3D12RootSignature*  GetRootSignature()    const { return mRootSignature.Get();        }
    UINT GetTexSRVDescriptorTableSize()         const { return mTexSrvDescriptorTableSize;  }
#ifdef TEX_SRV_USE_CB_HEAP
    UINT GetTexSrvOffset()                      const { return mTexSrvOffset;               }
#endif
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
    std::string                           mName;
    std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;
    std::vector<std::string>              mSemanticNames;

    std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>> mVSByteCodes;
    std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>> mPSByteCodes;
    std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>> mTSByteCodes;
    std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>> mGSByteCodes;

    std::unique_ptr<UploadBuffer>                  mShaderCB             = nullptr;
    Microsoft::WRL::ComPtr<ID3D12RootSignature>    mRootSignature        = nullptr;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>   mCbvHeap              = nullptr;
#ifndef TEX_SRV_USE_CB_HEAP
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>   mTexSrvDescriptorHeap = nullptr;
#endif

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

    void BuildInputLayout(const stShaderDescription& shaderDesc);
    std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();
};

} // end namespace