#include "FLD3DShaderWrapper.h"
#include "..\FLD3DUtils.h"

namespace FireFlame {
//void D3DShaderWrapper::BuildConstantBuffers(ID3D12Device* device){
//    mShaderCB = std::make_unique<UploadBuffer<ShaderConstants>>(device, 1, true);
//
//    UINT objCBByteSize = D3DUtils::CalcConstantBufferByteSize(sizeof(ShaderConstants));
//
//    D3D12_GPU_VIRTUAL_ADDRESS cbAddress = mShaderCB->Resource()->GetGPUVirtualAddress();
//    // Offset to the ith object constant buffer in the buffer.
//    int boxCBufIndex = 0;
//    cbAddress += boxCBufIndex*objCBByteSize;
//
//    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
//    cbvDesc.BufferLocation = cbAddress;
//    cbvDesc.SizeInBytes = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
//
//    md3dDevice->CreateConstantBufferView(
//        &cbvDesc,
//        mCbvHeap->GetCPUDescriptorHandleForHeapStart());
//}
void D3DShaderWrapper::BuildCBVDescriptorHeaps(ID3D12Device* device, UINT numDescriptors){
    D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
    cbvHeapDesc.NumDescriptors = numDescriptors;
    cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    cbvHeapDesc.NodeMask = 0;
    ThrowIfFailed(device->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(mCbvHeap.ReleaseAndGetAddressOf())));
}

void D3DShaderWrapper::BuildShadersAndInputLayout(const stShaderDescription& shaderDesc, UINT vertexFormat) {
    int index = 0;
    if (shaderDesc.HaveVS()) {
        mVSByteCode = D3DUtils::CompileShader(shaderDesc.shaderFile, nullptr, 
                                              shaderDesc.entryPoint[index], 
                                              shaderDesc.target[index]
        );
        ++index;
    }
    if (shaderDesc.HavePS()) {
        mPSByteCode = D3DUtils::CompileShader(shaderDesc.shaderFile, nullptr, 
                                              shaderDesc.entryPoint[index],
                                              shaderDesc.target[index]
        );
        ++index;
    }
    BuildInputLayout(shaderDesc, vertexFormat);
}
void D3DShaderWrapper::BuildInputLayout(const stShaderDescription& shaderDesc, UINT vertexFormat) {
    mInputLayout = {
        { shaderDesc.semanticNames[0].c_str(), 0, 
          FLVertexFormat2DXGIFormat(vertexFormat), 0, 0,
          D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 
        },
        { shaderDesc.semanticNames[1].c_str(), 0, 
          FLVertexFormat2DXGIFormat(vertexFormat), 0, FLVertexFormatByteSize(vertexFormat),
          D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 
        }
    };
}
} // end namespace