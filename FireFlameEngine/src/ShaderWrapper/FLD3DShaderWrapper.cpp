#include "PCH.h"
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
    CD3DX12_ROOT_PARAMETER slotRootParameter[5];

    CD3DX12_DESCRIPTOR_RANGE texTable;
    texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

    // Object constants index 1
    CD3DX12_DESCRIPTOR_RANGE cbvTable0;
    cbvTable0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);

    // Material constants index 2
    CD3DX12_DESCRIPTOR_RANGE cbvTable1;
    cbvTable1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);

    // Pass constants index 3
    CD3DX12_DESCRIPTOR_RANGE cbvTable2;
    cbvTable2.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 2);

    // index 4 not used
    CD3DX12_DESCRIPTOR_RANGE cbvTable3;
    cbvTable3.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 3);

    slotRootParameter[0].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_PIXEL);
    slotRootParameter[1].InitAsDescriptorTable(1, &cbvTable0);
    slotRootParameter[2].InitAsDescriptorTable(1, &cbvTable1);
    slotRootParameter[3].InitAsDescriptorTable(1, &cbvTable2);
    slotRootParameter[4].InitAsDescriptorTable(1, &cbvTable3);

    auto staticSamplers = GetStaticSamplers();

    // A root signature is an array of root parameters.
    CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc
    (
        5, slotRootParameter, 
        (UINT)staticSamplers.size(), staticSamplers.data(),
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

std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> D3DShaderWrapper::GetStaticSamplers()
{
    // Applications usually only need a handful of samplers.  So just define them all up front
    // and keep them available as part of the root signature.  

    const CD3DX12_STATIC_SAMPLER_DESC pointWrap(
        0, // shaderRegister
        D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
        D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

    const CD3DX12_STATIC_SAMPLER_DESC pointClamp(
        1, // shaderRegister
        D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

    const CD3DX12_STATIC_SAMPLER_DESC linearWrap(
        2, // shaderRegister
        D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
        D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

    const CD3DX12_STATIC_SAMPLER_DESC linearClamp(
        3, // shaderRegister
        D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

    const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(
        4, // shaderRegister
        D3D12_FILTER_ANISOTROPIC, // filter
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressW
        0.0f,                             // mipLODBias
        8);                               // maxAnisotropy

    const CD3DX12_STATIC_SAMPLER_DESC anisotropicClamp(
        5, // shaderRegister
        D3D12_FILTER_ANISOTROPIC, // filter
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressW
        0.0f,                              // mipLODBias
        8);                                // maxAnisotropy

    return {
        pointWrap, pointClamp,
        linearWrap, linearClamp,
        anisotropicWrap, anisotropicClamp };
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
    index -= mPassCbvOffset;
    auto currPassCB = Engine::GetEngine()->GetRenderer()->GetCurrFrameResource()->PassCB.get();
    currPassCB->CopyData(index, size, data);
}

void D3DShaderWrapper::BuildTexSRVHeap(UINT maxDescriptor)
{
    auto device = Engine::GetEngine()->GetRenderer()->GetDevice();

    D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
    srvHeapDesc.NumDescriptors = maxDescriptor;
    srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    srvHeapDesc.NodeMask = 0;
    ThrowIfFailed(device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&mTexSrvDescriptorHeap)));
    for (UINT i = 0; i < maxDescriptor; ++i)
    {
        mTexSrvHeapFreeList.push_front(i);
    }
}

UINT D3DShaderWrapper::CreateTexSRV(ID3D12Resource* res)
{
    auto renderer = Engine::GetEngine()->GetRenderer();
    auto device = renderer->GetDevice();

    if (mTexSrvHeapFreeList.empty())
        throw std::exception("todo : dynamically grow size of texture shader resource view");
    UINT index = mTexSrvHeapFreeList.front();
    // todo : material cbv management
    mTexSrvHeapFreeList.pop_front();

    CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(mTexSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
    hDescriptor.Offset(index, renderer->GetCbvSrvUavDescriptorSize());

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = res->GetDesc().Format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = res->GetDesc().MipLevels;
    srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

    device->CreateShaderResourceView(res, &srvDesc, hDescriptor);
    return index;
}

#ifdef TEX_SRV_USE_CB_HEAP
void D3DShaderWrapper::BuildFrameCBResources
(
    UINT objConstSize, UINT maxObjConstCount,
    UINT passConstSize, UINT maxPassConstCount,
    UINT matConstSize, UINT maxMatConstCount,
    UINT texSRVCount
)
#else
void D3DShaderWrapper::BuildFrameCBResources
(
    UINT objConstSize, UINT maxObjConstCount,
    UINT passConstSize, UINT maxPassConstCount,
    UINT matConstSize, UINT maxMatConstCount
)
#endif
{
    auto renderer = Engine::GetEngine()->GetRenderer();
    auto device = renderer->GetDevice();
    auto frameResources = renderer->GetFrameResources();
    for (const auto& frameRes : frameResources){
        if (objConstSize) frameRes->ObjectCB->Init(device, maxObjConstCount, objConstSize);
        if (passConstSize) frameRes->PassCB->Init(device, maxPassConstCount, passConstSize);
        if (matConstSize) frameRes->MaterialCB->Init(device, maxMatConstCount, matConstSize);
    }

    UINT numFrameResources = (UINT)frameResources.size();
    UINT objCount = maxObjConstCount;

    // Need a CBV descriptor for each object for each frame resource,
    // +1 for the perPass CBV for each frame resource.
    UINT numDescriptors = (objCount + maxPassConstCount + maxMatConstCount) * numFrameResources;

#ifdef TEX_SRV_USE_CB_HEAP
    numFrameResources += texSRVCount;
#endif

    // Save an offset to the start of the pass CBVs.  These are the last 3 descriptors.
    mPassCbvOffset = objCount * numFrameResources;
    mMaterialCbvOffset = mPassCbvOffset + maxPassConstCount*numFrameResources;
#ifdef TEX_SRV_USE_CB_HEAP
    mTexSrvOffset = mMaterialCbvOffset + maxMatConstCount * numFrameResources;
#endif

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

    UINT objCBByteSize = D3DUtils::CalcConstantBufferByteSize(objConstSize);

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

    // material  CBV
    UINT matCBByteSize = D3DUtils::CalcConstantBufferByteSize(matConstSize);
    for (UINT frameIndex = 0; frameIndex < numFrameResources; ++frameIndex) {
        auto matCB = frameResources[frameIndex]->MaterialCB->Resource();
        for (UINT i = 0; i < maxMatConstCount; ++i) {
            D3D12_GPU_VIRTUAL_ADDRESS cbAddress = matCB->GetGPUVirtualAddress();

            // Offset to the ith object constant buffer in the buffer.
            cbAddress += i*matCBByteSize;

            // Offset to the mat cbv in the descriptor heap.
            int heapIndex = mMaterialCbvOffset + frameIndex*maxMatConstCount + i;
            auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(mCbvHeap->GetCPUDescriptorHandleForHeapStart());
            handle.Offset(heapIndex, renderer->GetCbvSrvUavDescriptorSize());

            D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
            cbvDesc.BufferLocation = cbAddress;
            cbvDesc.SizeInBytes = matCBByteSize;

            device->CreateConstantBufferView(&cbvDesc, handle);
        }
    }
    for (UINT i = 0; i < maxMatConstCount; ++i) {
        mMatCbvHeapFreeList.push_front(i);
    }

    mPassCbvMaxCount = maxPassConstCount;
    mObjCbvMaxCount  = maxObjConstCount;
    mMatCbvMaxCount = maxMatConstCount;
}
void D3DShaderWrapper::BuildShadersAndInputLayout(const stShaderDescription& shaderDesc) {
    for (const auto& shaderStage : shaderDesc.shaderStage) {
        Microsoft::WRL::ComPtr<ID3DBlob> byteCode = nullptr;
        if (!shaderStage.file.empty())
        {
            byteCode = D3DUtils::CompileShader
            (
                shaderStage.file, nullptr,
                shaderStage.entry,
                shaderStage.target
            );
        }
        else
        {
            byteCode = D3DUtils::CompileShader
            (
                shaderStage.data, nullptr,
                shaderStage.entry,
                shaderStage.target
            );
        }

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