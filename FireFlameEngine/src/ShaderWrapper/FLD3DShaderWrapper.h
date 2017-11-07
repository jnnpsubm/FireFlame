#pragma once
#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include "..\Matrix\FLMatrix4X4.h"
#include "..\GPUMemory\D3DUploadBuffer.h"

namespace FireFlame {
struct stShaderDescription;
class D3DShaderWrapper {
public:
    typedef Microsoft::WRL::ComPtr<ID3D12PipelineState> PSO_ComPtr;

    // todo:if it has to grow bigger again,use inherit to use base class's < operator
    struct stPSODesc {
        UINT MSAAMode;
        D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveType;
        bool operator <(const stPSODesc& rhs) const {
            if (MSAAMode == rhs.MSAAMode) return primitiveType < rhs.primitiveType;
            return MSAAMode < rhs.MSAAMode;
        }
    };

public:
    D3DShaderWrapper() = default;

    template <typename T>
    void UpdateShaderCBData(unsigned int index, const T& data) {
        mShaderCB->CopyData(index, data);
    }

    void BuildRootSignature(ID3D12Device* device);
    void BuildPSO(ID3D12Device*, DXGI_FORMAT, DXGI_FORMAT, CRef_MSAADesc_Vec);
    void BuildConstantBuffers(ID3D12Device* device, UINT CBSize);
    void BuildCBVDescriptorHeaps(ID3D12Device* device, UINT numDescriptors);
    void BuildShadersAndInputLayout(const stShaderDescription& shaderDesc);

    // Get Methods
    // todo : variant heaps with variant shaders
    ID3D12DescriptorHeap* GetCBVHeap()          const { return mCbvHeap.Get(); }
    ID3D12RootSignature*  GetRootSignature()    const { return mRootSignature.Get(); }
    ID3D12PipelineState*  GetPSO(UINT MSAAMode, D3D12_PRIMITIVE_TOPOLOGY_TYPE ptype) const 
    { 
        auto it = mPSO.find({ MSAAMode, ptype });
        if (it != mPSO.end()) return it->second.Get();
        return nullptr;
    }

private:
    std::map<stPSODesc,PSO_ComPtr>               mPSO;
    std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

    Microsoft::WRL::ComPtr<ID3DBlob> mVSByteCode = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> mPSByteCode = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> mTSByteCode = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> mGSByteCode = nullptr;

    std::unique_ptr<UploadBuffer>                  mShaderCB      = nullptr;
    Microsoft::WRL::ComPtr<ID3D12RootSignature>    mRootSignature = nullptr;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>   mCbvHeap       = nullptr;

    void BuildInputLayout(const stShaderDescription& shaderDesc);
};

} // end namespace