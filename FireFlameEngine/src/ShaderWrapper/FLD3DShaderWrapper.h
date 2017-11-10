#pragma once
#include <vector>
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
    D3DShaderWrapper() = default;

    template <typename T>
    void UpdateShaderCBData(unsigned int index, const T& data) {
        //mShaderCB->CopyData(index, data);
    }
    void UpdateShaderCBData(unsigned int index, size_t size, const void* data);

    void BuildRootSignature(ID3D12Device* device);
    void BuildPSO(ID3D12Device*, DXGI_FORMAT, DXGI_FORMAT);
    //void BuildConstantBuffers(ID3D12Device* device, UINT CBSize);
    //void BuildCBVDescriptorHeaps(ID3D12Device* device, UINT numDescriptors);
    void BuildShadersAndInputLayout(const stShaderDescription& shaderDesc);
    void BuildFrameCBResources
    (
        UINT objConstSize, UINT passConstSize, 
        UINT maxObjConstCount, UINT maxPassConstCount
    );

    // Get Methods
    // todo : variant heaps with variant shaders
    ID3D12DescriptorHeap* GetCBVHeap()          const { return mCbvHeap.Get();       }
    ID3D12RootSignature*  GetRootSignature()    const { return mRootSignature.Get(); }
    UINT GetPassCBVMaxCount()                   const { return mPassCbvMaxCount;     }
    UINT GetObjCBVMaxCount()                    const { return mObjCbvMaxCount;      }
    UINT GetFreeObjCBV() { 
        if (mObjCbvHeapFreeList.empty())
            throw std::exception("todo : dynamiclly grow size of shader object const buff");
        UINT ret = mObjCbvHeapFreeList.front();
        mObjCbvHeapFreeList.pop_front();
        return ret; 
    }
    UINT GetFreePassCBV(int currFrame) {
        if (mPassCbvHeapFreeList.empty())
            throw std::exception("todo : dynamiclly grow size of shader pass const buff");
        UINT index = mPassCbvHeapFreeList.front();
        index += currFrame * mPassCbvMaxCount;
        index += mPassCbvOffset;
        // todo : pass cbv management
        //mPassCbvHeapFreeList.pop_front();
        return index;
    }

private:
    std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

    Microsoft::WRL::ComPtr<ID3DBlob> mVSByteCode = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> mPSByteCode = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> mTSByteCode = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> mGSByteCode = nullptr;

    std::unique_ptr<UploadBuffer>                  mShaderCB      = nullptr;
    Microsoft::WRL::ComPtr<ID3D12RootSignature>    mRootSignature = nullptr;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>   mCbvHeap       = nullptr;

    UINT                                           mPassCbvOffset = 0;
    UINT                                           mPassCbvMaxCount = 0;
    UINT                                           mObjCbvMaxCount = 0;
    std::forward_list<UINT>                        mObjCbvHeapFreeList;
    std::forward_list<UINT>                        mPassCbvHeapFreeList;

    void BuildInputLayout(const stShaderDescription& shaderDesc);
};

} // end namespace