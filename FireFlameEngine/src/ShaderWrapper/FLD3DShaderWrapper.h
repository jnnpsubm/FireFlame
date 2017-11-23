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

    void SetParamIndex(UINT texParamIndex, UINT objParamIndex, UINT matParamIndex, UINT passParamIndex)
    {
        mTexParamIndex = texParamIndex;
        mObjParamIndex = objParamIndex;
        mMatParamIndex = matParamIndex;
        mPassParamIndex = passParamIndex;
    }
    void UpdateObjCBData(unsigned int index, size_t size, const void* data);
    void UpdatePassCBData(unsigned int index, size_t size, const void* data);

    void BuildRootSignature(ID3D12Device* device);
    void BuildPSO(ID3D12Device*, DXGI_FORMAT, DXGI_FORMAT);
    //void BuildConstantBuffers(ID3D12Device* device, UINT CBSize);
    //void BuildCBVDescriptorHeaps(ID3D12Device* device, UINT numDescriptors);
    void BuildShadersAndInputLayout(const stShaderDescription& shaderDesc);
    void BuildTexSRVHeap(UINT maxDescriptor);
#ifdef TEX_SRV_USE_CB_HEAP
    void BuildFrameCBResources
    (
        UINT objConstSize, UINT maxObjConstCount,
        UINT passConstSize, UINT maxPassConstCount,
        UINT matConstSize, UINT maxMatConstCount,
        UINT texSRVCount
    );
#else
    void BuildFrameCBResources
    (
        UINT objConstSize, UINT maxObjConstCount,
        UINT passConstSize, UINT maxPassConstCount,
        UINT matConstSize, UINT maxMatConstCount,
        UINT texSRVCount
    );
#endif
    UINT CreateTexSRV(ID3D12Resource* res);

    // Get Methods
    // todo : variant heaps with variant shaders
    ID3D12DescriptorHeap* GetCBVHeap()          const { return mCbvHeap.Get();              }
    ID3D12DescriptorHeap* GetTexSRVHeap()       const { return mTexSrvDescriptorHeap.Get(); }
    ID3D12RootSignature*  GetRootSignature()    const { return mRootSignature.Get();        }
    UINT GetMatCBVOffset()                      const { return mMaterialCbvOffset;          }
    UINT GetMaterialCBVMaxCount()               const { return mMatCbvMaxCount;             }
    UINT GetPassCBVMaxCount()                   const { return mPassCbvMaxCount;            }
    UINT GetObjCBVMaxCount()                    const { return mObjCbvMaxCount;             }
    UINT GetTexParamIndex()                     const { return mTexParamIndex;              }
    UINT GetObjParamIndex()                     const { return mObjParamIndex;              }
    UINT GetMatParamIndex()                     const { return mMatParamIndex;              }
    UINT GetPassParamIndex()                    const { return mPassParamIndex;             }
    UINT GetFreeObjCBV() { 
        if (mObjCbvHeapFreeList.empty())
            throw std::exception("todo : dynamically grow size of shader object const buff");
        UINT ret = mObjCbvHeapFreeList.front();
        mObjCbvHeapFreeList.pop_front();
        return ret; 
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

    Microsoft::WRL::ComPtr<ID3DBlob> mVSByteCode = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> mPSByteCode = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> mTSByteCode = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> mGSByteCode = nullptr;

    std::unique_ptr<UploadBuffer>                  mShaderCB             = nullptr;
    Microsoft::WRL::ComPtr<ID3D12RootSignature>    mRootSignature        = nullptr;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>   mCbvHeap              = nullptr;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>   mTexSrvDescriptorHeap = nullptr;

    UINT                                           mTexParamIndex = 0;
    UINT                                           mObjParamIndex = 1;
    UINT                                           mMatParamIndex = 2;
    UINT                                           mPassParamIndex = 3;

    UINT                                           mMaterialCbvOffset = 0;
    UINT                                           mPassCbvOffset = 0;
#ifdef TEX_SRV_USE_CB_HEAP
    UINT                                           mTexSrvOffset = 0;
#endif

    UINT                                           mPassCbvMaxCount = 0;
    UINT                                           mObjCbvMaxCount = 0;
    UINT                                           mMatCbvMaxCount = 0;

    std::forward_list<UINT>                        mObjCbvHeapFreeList;
    std::forward_list<UINT>                        mPassCbvHeapFreeList;
    std::forward_list<UINT>                        mMatCbvHeapFreeList;
    std::forward_list<UINT>                        mTexSrvHeapFreeList;

    void BuildInputLayout(const stShaderDescription& shaderDesc);
    std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();
};

} // end namespace