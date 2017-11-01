#include "FLD3DShaderWrapper.h"
#include "..\FLD3DUtils.h"
#include "ShaderConstBuffer\FLShaderConstBuffer.h"

namespace FireFlame {
void D3DShaderWrapper::BuildPSO(ID3D12Device* device, DXGI_FORMAT backBufferFormat, 
    DXGI_FORMAT DSFormat,bool MSAAOn, UINT sampleCount, UINT MSAAQuality)
{
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
    ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
    psoDesc.InputLayout = { mInputLayout.data(), (UINT)mInputLayout.size() };
    psoDesc.pRootSignature = mRootSignature.Get();
    psoDesc.VS = {
        reinterpret_cast<BYTE*>(mVSByteCode->GetBufferPointer()),
        mVSByteCode->GetBufferSize()
    };
    psoDesc.PS = {
        reinterpret_cast<BYTE*>(mPSByteCode->GetBufferPointer()),
        mPSByteCode->GetBufferSize()
    };
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = backBufferFormat;
    psoDesc.SampleDesc.Count = MSAAOn ? sampleCount : 1;
    psoDesc.SampleDesc.Quality = MSAAOn ? (MSAAQuality - 1) : 0;
    psoDesc.DSVFormat = DSFormat;
    ThrowIfFailed(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPSO)));
}
void D3DShaderWrapper::BuildConstantBuffers(ID3D12Device* device, UINT CBSize){
    mShaderCB = std::make_unique<UploadBuffer>(true);
    mShaderCB->Init(device, 1, CBSize);

    UINT objCBByteSize = D3DUtils::CalcConstantBufferByteSize(CBSize);

    D3D12_GPU_VIRTUAL_ADDRESS cbAddress = mShaderCB->Resource()->GetGPUVirtualAddress();
    // Offset to the ith object constant buffer in the buffer.
    int boxCBufIndex = 0;
    cbAddress += boxCBufIndex*objCBByteSize;

    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
    cbvDesc.BufferLocation = cbAddress;
    cbvDesc.SizeInBytes = D3DUtils::CalcConstantBufferByteSize(sizeof(CBSize));

    device->CreateConstantBufferView(
        &cbvDesc,
        mCbvHeap->GetCPUDescriptorHandleForHeapStart());
}
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
    // bug? pointer to temp char*
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