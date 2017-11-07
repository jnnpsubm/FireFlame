#include "FLD3DShaderWrapper.h"
#include "..\FLD3DUtils.h"
#include "ShaderConstBuffer\FLShaderConstBuffer.h"

namespace FireFlame {
void D3DShaderWrapper::BuildPSO(ID3D12Device* device, DXGI_FORMAT backBufferFormat, 
                                DXGI_FORMAT DSFormat,CRef_MSAADesc_Vec msaaVec)
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
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = backBufferFormat;
    psoDesc.DSVFormat = DSFormat;
    for (size_t i = 0; i < msaaVec.size(); ++i) {
        const auto& msaaDesc = msaaVec[i];
        psoDesc.SampleDesc.Count = msaaDesc.sampleCount;
        psoDesc.SampleDesc.Quality = msaaDesc.qualityLevels - 1;
        // todo:no patch now, because no tes
        for (int ptype = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
                 ptype < D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH; ++ptype)
        {
            psoDesc.PrimitiveTopologyType = (D3D12_PRIMITIVE_TOPOLOGY_TYPE)ptype;
            PSO_ComPtr pso = nullptr;
            ThrowIfFailed
            (
                device->CreateGraphicsPipelineState
                (
                    &psoDesc,
                    IID_PPV_ARGS(pso.GetAddressOf())
                )
            );
            stPSODesc key{ (UINT)i,psoDesc.PrimitiveTopologyType };
            mPSO.emplace(key, pso);
        }
    }
}
void D3DShaderWrapper::BuildRootSignature(ID3D12Device* device){
    // Shader programs typically require resources as input (constant buffers,
    // textures, samplers).  The root signature defines the resources the shader
    // programs expect.  If we think of the shader programs as a function, and
    // the input resources as function parameters, then the root signature can be
    // thought of as defining the function signature.  

    // Root parameter can be a table, root descriptor or root constants.
    CD3DX12_ROOT_PARAMETER slotRootParameter[1];

    // Create a single descriptor table of CBVs.
    CD3DX12_DESCRIPTOR_RANGE cbvTable;
    cbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
    slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable);

    // A root signature is an array of root parameters.
    CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc
    (
        1, slotRootParameter, 0, nullptr,
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
    );

    // create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
    Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
    HRESULT hr = D3D12SerializeRootSignature
    (
        &rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
        serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf()
    );

    if (errorBlob != nullptr){
        ::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
    }
    ThrowIfFailed(hr);

    ThrowIfFailed
    (
        device->CreateRootSignature
        (
            0,
            serializedRootSig->GetBufferPointer(),
            serializedRootSig->GetBufferSize(),
            IID_PPV_ARGS(mRootSignature.ReleaseAndGetAddressOf())
        )
    );
}
void D3DShaderWrapper::BuildConstantBuffers(ID3D12Device* device, UINT CBSize){
    mShaderCB = std::make_unique<UploadBuffer>(true);
    mShaderCB->Init(device, 1, CBSize);

    UINT objCBByteSize = D3DUtils::CalcConstantBufferByteSize(CBSize);

    D3D12_GPU_VIRTUAL_ADDRESS cbAddress = mShaderCB->Resource()->GetGPUVirtualAddress();
    // Offset to the ith object constant buffer in the buffer.
    int CBIndex = 0;
    cbAddress += CBIndex*objCBByteSize;

    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
    cbvDesc.BufferLocation = cbAddress;
    cbvDesc.SizeInBytes = D3DUtils::CalcConstantBufferByteSize(sizeof(CBSize));

    device->CreateConstantBufferView
    (
        &cbvDesc,
        mCbvHeap->GetCPUDescriptorHandleForHeapStart()
    );
}
void D3DShaderWrapper::BuildCBVDescriptorHeaps(ID3D12Device* device, UINT numDescriptors){
    D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
    cbvHeapDesc.NumDescriptors = numDescriptors;
    cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    cbvHeapDesc.NodeMask = 0;
    ThrowIfFailed(device->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(mCbvHeap.ReleaseAndGetAddressOf())));
}

void D3DShaderWrapper::BuildShadersAndInputLayout(const stShaderDescription& shaderDesc) {
    for (const auto& shaderStage : shaderDesc.shaderStage) {
        Microsoft::WRL::ComPtr<ID3DBlob> byteCode = D3DUtils::CompileShader
        (
            shaderStage.file, nullptr, 
            shaderStage.entry, 
            shaderStage.target
        );
        switch (shaderStage.type){
        case Shader_Type::VS:{
            mVSByteCode = byteCode;
        }break;
        case Shader_Type::PS: {
            mPSByteCode = byteCode;
        }break;
        default:
            throw std::exception("unhandled shader stage type......");
            break;
        }
    }
    BuildInputLayout(shaderDesc);
}
void D3DShaderWrapper::BuildInputLayout(const stShaderDescription& shaderDesc) {
    // bug? pointer to temp char*
    std::unordered_map<UINT, UINT> slotOffset;
    for (size_t i = 0; i < shaderDesc.semanticNames.size(); i++){
        mInputLayout.push_back
        (
        {
            shaderDesc.semanticNames[i].name.c_str(), 
            shaderDesc.semanticNames[i].index,
            FLVertexFormat2DXGIFormat(shaderDesc.vertexFormats[i]), 
            shaderDesc.inputSlots[i],
            slotOffset[shaderDesc.inputSlots[i]],
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
        }
        );
        slotOffset[shaderDesc.inputSlots[i]] += FLVertexFormatByteSize(shaderDesc.vertexFormats[i]);
    }
}
} // end namespace