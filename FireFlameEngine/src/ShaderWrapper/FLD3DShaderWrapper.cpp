#include "FLD3DShaderWrapper.h"
#include "..\FLD3DUtils.h"
#include "ShaderConstBuffer\FLShaderConstBuffer.h"
#include "..\Engine\FLEngine.h"
#include "..\PSOManager\FLD3DPSOManager.h"
#include "..\Renderer\FLD3DRenderer.h"

namespace FireFlame {
void D3DShaderWrapper::BuildPSO(ID3D12Device* device, DXGI_FORMAT backBufferFormat, DXGI_FORMAT DSFormat)
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
    psoDesc.RTVFormats[0] = backBufferFormat;
    psoDesc.DSVFormat = DSFormat;
    Engine::GetEngine()->GetPSOManager()->AddPSO(mName, psoDesc);
}
void D3DShaderWrapper::BuildRootSignature(ID3D12Device* device){
    // Shader programs typically require resources as input (constant buffers,
    // textures, samplers).  The root signature defines the resources the shader
    // programs expect.  If we think of the shader programs as a function, and
    // the input resources as function parameters, then the root signature can be
    // thought of as defining the function signature.  

    // Root parameter can be a table, root descriptor or root constants.
    CD3DX12_ROOT_PARAMETER slotRootParameter[2];

    // Create a single descriptor table of CBVs.
    CD3DX12_DESCRIPTOR_RANGE cbvTable0;
    cbvTable0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);

    CD3DX12_DESCRIPTOR_RANGE cbvTable1;
    cbvTable1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);

    slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable0);
    slotRootParameter[1].InitAsDescriptorTable(1, &cbvTable1);

    // A root signature is an array of root parameters.
    CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc
    (
        2, slotRootParameter, 0, nullptr,
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
//void D3DShaderWrapper::BuildConstantBuffers(ID3D12Device* device, UINT CBSize){
//    mShaderCB = std::make_unique<UploadBuffer>(true);
//    mShaderCB->Init(device, 1, CBSize);
//
//    UINT objCBByteSize = D3DUtils::CalcConstantBufferByteSize(CBSize);
//
//    D3D12_GPU_VIRTUAL_ADDRESS cbAddress = mShaderCB->Resource()->GetGPUVirtualAddress();
//    // Offset to the ith object constant buffer in the buffer.
//    int CBIndex = 0;
//    cbAddress += CBIndex*objCBByteSize;
//
//    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
//    cbvDesc.BufferLocation = cbAddress;
//    cbvDesc.SizeInBytes = D3DUtils::CalcConstantBufferByteSize(sizeof(CBSize));
//
//    device->CreateConstantBufferView
//    (
//        &cbvDesc,
//        mCbvHeap->GetCPUDescriptorHandleForHeapStart()
//    );
//}
//void D3DShaderWrapper::BuildCBVDescriptorHeaps(ID3D12Device* device, UINT numDescriptors){
//    D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
//    cbvHeapDesc.NumDescriptors = numDescriptors;
//    cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//    cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
//    cbvHeapDesc.NodeMask = 0;
//    ThrowIfFailed(device->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(mCbvHeap.ReleaseAndGetAddressOf())));
//}
void D3DShaderWrapper::UpdateObjCBData(unsigned int index, size_t size, const void* data) 
{
    auto currObjectCB = Engine::GetEngine()->GetRenderer()->GetCurrFrameResource()->ObjectCB.get();
    currObjectCB->CopyData(index, size, data);
}
void D3DShaderWrapper::UpdatePassCBData(unsigned int index, size_t size, const void* data)
{
    auto currPassCB = Engine::GetEngine()->GetRenderer()->GetCurrFrameResource()->PassCB.get();
    currPassCB->CopyData(index, size, data);
}
void D3DShaderWrapper::BuildFrameCBResources
(
    UINT objConstSize, UINT passConstSize,
    UINT maxObjConstCount, UINT maxPassConstCount
)
{
    auto renderer = Engine::GetEngine()->GetRenderer();
    auto device = renderer->GetDevice();
    auto frameResources = renderer->GetFrameResources();
    for (const auto& frameRes : frameResources){
        if (objConstSize) frameRes->ObjectCB->Init(device, maxObjConstCount, objConstSize);
        if (passConstSize) frameRes->PassCB->Init(device, maxPassConstCount, passConstSize);
    }

    UINT numFrameResources = (UINT)frameResources.size();
    UINT objCount = maxObjConstCount;

    // Need a CBV descriptor for each object for each frame resource,
    // +1 for the perPass CBV for each frame resource.
    UINT numDescriptors = (objCount + maxPassConstCount) * numFrameResources;

    // Save an offset to the start of the pass CBVs.  These are the last 3 descriptors.
    mPassCbvOffset = objCount * numFrameResources;

    D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
    cbvHeapDesc.NumDescriptors = numDescriptors;
    cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    cbvHeapDesc.NodeMask = 0;
    ThrowIfFailed
    (
        device->CreateDescriptorHeap
        (
            &cbvHeapDesc,
            IID_PPV_ARGS(&mCbvHeap)
        )
    );

    UINT objCBByteSize = D3DUtils::CalcConstantBufferByteSize(sizeof(objConstSize));

    // Need a CBV descriptor for each object for each frame resource.
    for (UINT frameIndex = 0; frameIndex < numFrameResources; ++frameIndex){
        auto objectCB = frameResources[frameIndex]->ObjectCB->Resource();
        for (UINT i = 0; i < objCount; ++i){
            D3D12_GPU_VIRTUAL_ADDRESS cbAddress = objectCB->GetGPUVirtualAddress();

            // Offset to the ith object constant buffer in the buffer.
            cbAddress += i*objCBByteSize;

            // Offset to the object cbv in the descriptor heap.
            int heapIndex = frameIndex*objCount + i;
            auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(mCbvHeap->GetCPUDescriptorHandleForHeapStart());
            handle.Offset(heapIndex, renderer->GetCbvSrvUavDescriptorSize());

            D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
            cbvDesc.BufferLocation = cbAddress;
            cbvDesc.SizeInBytes = objCBByteSize;

            device->CreateConstantBufferView(&cbvDesc, handle);
        }
    }
    for (UINT i = 0; i < objCount; ++i) {
        mObjCbvHeapFreeList.push_front(i);
    }

    UINT passCBByteSize = D3DUtils::CalcConstantBufferByteSize(passConstSize);
    // Last three descriptors are the pass CBVs for each frame resource.
    for (UINT frameIndex = 0; frameIndex < numFrameResources; ++frameIndex){
        auto passCB = frameResources[frameIndex]->PassCB->Resource();
        for (UINT i = 0; i < maxPassConstCount; ++i) {
            D3D12_GPU_VIRTUAL_ADDRESS cbAddress = passCB->GetGPUVirtualAddress();

            // Offset to the ith object constant buffer in the buffer.
            cbAddress += i*passCBByteSize;

            // Offset to the pass cbv in the descriptor heap.
            int heapIndex = mPassCbvOffset + frameIndex*maxPassConstCount + i;
            auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(mCbvHeap->GetCPUDescriptorHandleForHeapStart());
            handle.Offset(heapIndex, renderer->GetCbvSrvUavDescriptorSize());

            D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
            cbvDesc.BufferLocation = cbAddress;
            cbvDesc.SizeInBytes = passCBByteSize;

            device->CreateConstantBufferView(&cbvDesc, handle);
        }
    }
    for (UINT i = 0; i < maxPassConstCount; ++i) {
        mPassCbvHeapFreeList.push_front(i);
    }
    mPassCbvMaxCount = maxPassConstCount;
    mObjCbvMaxCount  = maxObjConstCount;
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