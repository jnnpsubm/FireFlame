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
    /*psoDesc.VS = {
        reinterpret_cast<BYTE*>(mVSByteCode->GetBufferPointer()),
        mVSByteCode->GetBufferSize()
    };
    psoDesc.PS = {
        reinterpret_cast<BYTE*>(mPSByteCode->GetBufferPointer()),
        mPSByteCode->GetBufferSize()
    };*/
    psoDesc.RTVFormats[0] = backBufferFormat;
    psoDesc.DSVFormat = DSFormat;
    for (const auto& vs : mVSByteCodes)
    {
        psoDesc.VS = {
            reinterpret_cast<BYTE*>(vs.second->GetBufferPointer()),
            vs.second->GetBufferSize()
        };
        for (const auto& ps : mPSByteCodes)
        {
            psoDesc.PS = {
                reinterpret_cast<BYTE*>(ps.second->GetBufferPointer()),
                ps.second->GetBufferSize()
            };
            auto shaderMacros = ShaderMacros2String(vs.first, ps.first, "", "");
            Engine::GetEngine()->GetPSOManager()->AddPSO(mName, shaderMacros, psoDesc);
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
    CD3DX12_ROOT_PARAMETER slotRootParameter[5];

    CD3DX12_DESCRIPTOR_RANGE texTable;
    texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, mTexSrvDescriptorTableSize, 0);

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

    slotRootParameter[0].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_ALL);
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
    auto& shaderRes = Engine::GetEngine()->GetRenderer()->GetCurrFrameResource()->ShaderResources[mName];
    auto currObjectCB = shaderRes.ObjectCB.get();
    currObjectCB->CopyData(index, size, data);
}
void D3DShaderWrapper::UpdateMultiObjCBData(unsigned int index, size_t size, const void* data)
{
    index -= mMultiObjCbvOffset;
    auto& shaderRes = Engine::GetEngine()->GetRenderer()->GetCurrFrameResource()->ShaderResources[mName];
    auto currMultiObjCB = shaderRes.MultiObjectCB.get();
    currMultiObjCB->CopyData(index, size, data);
}
void D3DShaderWrapper::UpdatePassCBData(unsigned int index, size_t size, const void* data)
{
    index -= mPassCbvOffset;
    auto& shaderRes = Engine::GetEngine()->GetRenderer()->GetCurrFrameResource()->ShaderResources[mName];
    auto currPassCB = shaderRes.PassCB.get();
    currPassCB->CopyData(index, size, data);
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

#ifdef TEX_SRV_USE_CB_HEAP
    CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(mCbvHeap->GetCPUDescriptorHandleForHeapStart());
    hDescriptor.Offset(index+mTexSrvOffset, renderer->GetCbvSrvUavDescriptorSize());
#else
    CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(mTexSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
    hDescriptor.Offset(index, renderer->GetCbvSrvUavDescriptorSize());
#endif
    
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

UINT D3DShaderWrapper::CreateTexSRV(const std::vector<ID3D12Resource *>& vecRes)
{
    auto renderer = Engine::GetEngine()->GetRenderer();
    auto device = renderer->GetDevice();

    if (mTexSrvHeapFreeList.empty())
        throw std::exception("todo : dynamically grow size of texture shader resource view");
    UINT index = mTexSrvHeapFreeList.front();
    // todo : material cbv management
    mTexSrvHeapFreeList.pop_front();

    for (size_t i = 0; i < vecRes.size(); i++)
    {
        CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(mCbvHeap->GetCPUDescriptorHandleForHeapStart());
        hDescriptor.Offset(index + mTexSrvOffset + (UINT)i, renderer->GetCbvSrvUavDescriptorSize());

        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Format = vecRes[i]->GetDesc().Format;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.MipLevels = vecRes[i]->GetDesc().MipLevels;
        srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

        device->CreateShaderResourceView(vecRes[i], &srvDesc, hDescriptor);
    }
    return index;
}

void D3DShaderWrapper::BuildFrameCBResources
(
    UINT objConstSize, UINT maxObjConstCount,
    UINT passConstSize, UINT maxPassConstCount,
    UINT matConstSize, UINT maxMatConstCount,
    UINT texSRVTableSize, UINT texSRVCount,
    UINT multiObjConstSize, UINT maxMultiObjConstCount
)
{
    auto renderer = Engine::GetEngine()->GetRenderer();
    auto device = renderer->GetDevice();
    auto frameResources = renderer->GetFrameResources();
    for (const auto& frameRes : frameResources){
        auto& shaderRes = frameRes->ShaderResources[mName];
        if (objConstSize) shaderRes.ObjectCB->Init(device, maxObjConstCount, objConstSize);
        if (passConstSize) shaderRes.PassCB->Init(device, maxPassConstCount, passConstSize);
        if (matConstSize) shaderRes.MaterialCB->Init(device, maxMatConstCount, matConstSize);
        if (multiObjConstSize) shaderRes.MultiObjectCB->Init(device, maxMultiObjConstCount, multiObjConstSize);
    }

    UINT numFrameResources = (UINT)frameResources.size();
    UINT objCount = maxObjConstCount;

    // Need a CBV descriptor for each object for each frame resource,
    // +1 for the perPass CBV for each frame resource.
    UINT numDescriptors = (objCount + maxPassConstCount + maxMatConstCount) * numFrameResources;
    numDescriptors += maxMultiObjConstCount * numFrameResources;
    numDescriptors += texSRVCount;
    
    // Save an offset to the start of the pass CBVs.  These are the last 3 descriptors.
    mPassCbvOffset = objCount * numFrameResources;
    mMaterialCbvOffset = mPassCbvOffset + maxPassConstCount*numFrameResources;
    mMultiObjCbvOffset = mMaterialCbvOffset + maxMatConstCount*numFrameResources;
    mTexSrvOffset = mMultiObjCbvOffset + maxMultiObjConstCount * numFrameResources;

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
        auto& shaderRes = renderer->GetFrameResources()[frameIndex]->ShaderResources[mName];
        auto objectCB = shaderRes.ObjectCB->Resource();
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
        auto& shaderRes = renderer->GetFrameResources()[frameIndex]->ShaderResources[mName];
        auto passCB = shaderRes.PassCB->Resource();
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
        auto& shaderRes = renderer->GetFrameResources()[frameIndex]->ShaderResources[mName];
        auto matCB = shaderRes.MaterialCB->Resource();
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

    // multiObject  CBV
    UINT multiObjCBByteSize = D3DUtils::CalcConstantBufferByteSize(multiObjConstSize);
    for (UINT frameIndex = 0; frameIndex < numFrameResources; ++frameIndex) {
        auto& shaderRes = renderer->GetFrameResources()[frameIndex]->ShaderResources[mName];
        auto multiObjCB = shaderRes.MultiObjectCB->Resource();
        for (UINT i = 0; i < maxMultiObjConstCount; ++i) {
            D3D12_GPU_VIRTUAL_ADDRESS cbAddress = multiObjCB->GetGPUVirtualAddress();

            // Offset to the ith object constant buffer in the buffer.
            cbAddress += i*multiObjCBByteSize;

            // Offset to the mat cbv in the descriptor heap.
            int heapIndex = mMultiObjCbvOffset + frameIndex*maxMultiObjConstCount + i;
            auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(mCbvHeap->GetCPUDescriptorHandleForHeapStart());
            handle.Offset(heapIndex, renderer->GetCbvSrvUavDescriptorSize());

            D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
            cbvDesc.BufferLocation = cbAddress;
            cbvDesc.SizeInBytes = multiObjCBByteSize;

            device->CreateConstantBufferView(&cbvDesc, handle);
        }
    }
    for (UINT i = 0; i < maxMultiObjConstCount; ++i) {
        mMultiObjCbvHeapFreeList.push_front(i);
    }

#ifdef TEX_SRV_USE_CB_HEAP
    mTexSrvDescriptorTableSize = texSRVTableSize;
    assert(0 == texSRVCount%mTexSrvDescriptorTableSize);
    for (int i = texSRVCount-mTexSrvDescriptorTableSize; i >= 0; i -= mTexSrvDescriptorTableSize) {
        mTexSrvHeapFreeList.push_front(i);
    }
#endif

    mPassCbvMaxCount = maxPassConstCount;
    mObjCbvMaxCount  = maxObjConstCount;
    mMultiObjCbvMaxCount = maxMultiObjConstCount;
    mMatCbvMaxCount = maxMatConstCount;
}
void D3DShaderWrapper::BuildShadersAndInputLayout(const stShaderDescription& shaderDesc) {
    for (const auto& shaderStage : shaderDesc.shaderStage) {
        Microsoft::WRL::ComPtr<ID3DBlob> byteCode = nullptr;
        D3D_SHADER_MACRO* defines = nullptr; 
        if (!shaderStage.Macros.empty())
        {
            defines = new D3D_SHADER_MACRO[shaderStage.Macros.size() + 1];
            for (size_t i = 0; i < shaderStage.Macros.size(); ++i)
            {
                defines[i].Name = shaderStage.Macros[i].first.c_str();
                defines[i].Definition = shaderStage.Macros[i].second.c_str();
            }
            defines[shaderStage.Macros.size()].Name = NULL;
            defines[shaderStage.Macros.size()].Definition = NULL;
        }
        if (!shaderStage.file.empty())
        {
            byteCode = D3DUtils::CompileShader
            (
                shaderStage.file, defines,
                shaderStage.entry,
                shaderStage.target
            );
        }
        else
        {
            byteCode = D3DUtils::CompileShader
            (
                shaderStage.data, defines,
                shaderStage.entry,
                shaderStage.target
            );
        }
        delete[] defines;

        switch (shaderStage.type){
        case Shader_Type::VS:{
            mVSByteCodes[shaderStage.Macros2String()] = byteCode;
        }break;
        case Shader_Type::GS: {
            mGSByteCodes[shaderStage.Macros2String()] = byteCode;
        }break;
        case Shader_Type::PS: {
            mPSByteCodes[shaderStage.Macros2String()] = byteCode;
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
    mSemanticNames.reserve(shaderDesc.semanticNames.size()); // must not grow size and reallocation
    for (size_t i = 0; i < shaderDesc.semanticNames.size(); i++){
        mSemanticNames.push_back(shaderDesc.semanticNames[i].name);
        mInputLayout.push_back
        (
        {
            mSemanticNames.back().data(), 
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