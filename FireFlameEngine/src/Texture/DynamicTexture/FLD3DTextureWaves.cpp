#include "FLD3DTextureWaves.h"
#include <assert.h>
#include "..\..\Engine\FLEngine.h"
#include "..\..\FLD3DUtils.h"
#include "..\..\MathHelper\FLMathHelper.h"

namespace FireFlame {
D3DGpuWaves::D3DGpuWaves
(
    ID3D12Device* device, ID3D12GraphicsCommandList* cmdList,
    int m, int n, float dx, float dt, float speed, float damping, unsigned disturbCount
) : md3dDevice(device),mNumRows(m),mNumCols(n),mDisturbCount(disturbCount)
{
    assert((m*n) % 256 == 0);

    mVertexCount = m*n;
    mTriangleCount = (m - 1)*(n - 1) * 2;

    mTimeStep = dt;
    mSpatialStep = dx;

    float d = damping*dt + 2.0f;
    float e = (speed*speed)*(dt*dt) / (dx*dx);
    mK[0] = (damping*dt - 2.0f) / d;
    mK[1] = (4.0f - 8.0f*e) / d;
    mK[2] = (2.0f*e) / d;

    BuildResources(cmdList);
    BuildDescriptors();

    BuildRootSignature();
    BuildPSO();
}

void D3DGpuWaves::Go(ID3D12GraphicsCommandList* cmdList)
{
    ID3D12DescriptorHeap* heaps[1] = { mDescriptorHeap.Get() };
    cmdList->SetDescriptorHeaps(1, heaps);
    Disturb(cmdList);
    Update(cmdList);
}

void D3DGpuWaves::BuildResources(ID3D12GraphicsCommandList* cmdList)
{
    // All the textures for the wave simulation will be bound as a shader resource and
    // unordered access view at some point since we ping-pong the buffers.
    D3D12_RESOURCE_DESC texDesc;
    ZeroMemory(&texDesc, sizeof(D3D12_RESOURCE_DESC));
    texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    texDesc.Alignment = 0;
    texDesc.Width = mNumCols;
    texDesc.Height = mNumRows;
    texDesc.DepthOrArraySize = 1;
    texDesc.MipLevels = 1;
    texDesc.Format = DXGI_FORMAT_R32_FLOAT;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

    ThrowIfFailed
    (
        md3dDevice->CreateCommittedResource
        (
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &texDesc,
            D3D12_RESOURCE_STATE_COMMON,
            nullptr,
            IID_PPV_ARGS(&mPrevSol)
        )
    );
    ThrowIfFailed
    (
        md3dDevice->CreateCommittedResource
        (
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &texDesc,
            D3D12_RESOURCE_STATE_COMMON,
            nullptr,
            IID_PPV_ARGS(&mCurrSol)
        )
    );
    ThrowIfFailed
    (
        md3dDevice->CreateCommittedResource
        (
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &texDesc,
            D3D12_RESOURCE_STATE_COMMON,
            nullptr,
            IID_PPV_ARGS(&mNextSol)
        )
    );

    const UINT num2DSubresources = texDesc.DepthOrArraySize * texDesc.MipLevels;
    const UINT64 uploadBufferSize = GetRequiredIntermediateSize(mCurrSol.Get(), 0, num2DSubresources);
    ThrowIfFailed
    (
        md3dDevice->CreateCommittedResource
        (
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(mPrevUploadBuffer.GetAddressOf())
        )
    );
    ThrowIfFailed
    (
        md3dDevice->CreateCommittedResource
        (
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(mCurrUploadBuffer.GetAddressOf())
        )
    );
    // Describe the data we want to copy into the default buffer.
    std::vector<float> initData(mNumRows*mNumCols, 0.0f);
    memset(initData.data(), 0, initData.size() * sizeof(float));

    D3D12_SUBRESOURCE_DATA subResourceData = {};
    subResourceData.pData = initData.data();
    subResourceData.RowPitch = mNumCols * sizeof(float);
    subResourceData.SlicePitch = subResourceData.RowPitch * mNumRows;

    cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mPrevSol.Get(),
        D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));
    UpdateSubresources(cmdList, mPrevSol.Get(), mPrevUploadBuffer.Get(), 0, 0, num2DSubresources, &subResourceData);
    cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mPrevSol.Get(),
        D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));

    cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mCurrSol.Get(),
        D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));
    UpdateSubresources(cmdList, mCurrSol.Get(), mCurrUploadBuffer.Get(), 0, 0, num2DSubresources, &subResourceData);
    cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mCurrSol.Get(),
        D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));

    cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mNextSol.Get(),
        D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));
}

void D3DGpuWaves::BuildDescriptors()
{
    D3D12_DESCRIPTOR_HEAP_DESC heapdesc;
    heapdesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    heapdesc.NodeMask = 0;
    heapdesc.NumDescriptors = 6;
    heapdesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    md3dDevice->CreateDescriptorHeap(&heapdesc, IID_PPV_ARGS(mDescriptorHeap.GetAddressOf()));

    UINT descriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuDescriptor = CD3DX12_CPU_DESCRIPTOR_HANDLE
    (
        mDescriptorHeap->GetCPUDescriptorHandleForHeapStart()
    );
    CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuDescriptor = CD3DX12_GPU_DESCRIPTOR_HANDLE
    (
        mDescriptorHeap->GetGPUDescriptorHandleForHeapStart()
    );

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;

    D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
    uavDesc.Format = DXGI_FORMAT_R32_FLOAT;
    uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
    uavDesc.Texture2D.MipSlice = 0;

    md3dDevice->CreateShaderResourceView(mPrevSol.Get(), &srvDesc, hCpuDescriptor);
    md3dDevice->CreateShaderResourceView(mCurrSol.Get(), &srvDesc, hCpuDescriptor.Offset(1, descriptorSize));
    md3dDevice->CreateShaderResourceView(mNextSol.Get(), &srvDesc, hCpuDescriptor.Offset(1, descriptorSize));

    md3dDevice->CreateUnorderedAccessView(mPrevSol.Get(), nullptr, &uavDesc, hCpuDescriptor.Offset(1, descriptorSize));
    md3dDevice->CreateUnorderedAccessView(mCurrSol.Get(), nullptr, &uavDesc, hCpuDescriptor.Offset(1, descriptorSize));
    md3dDevice->CreateUnorderedAccessView(mNextSol.Get(), nullptr, &uavDesc, hCpuDescriptor.Offset(1, descriptorSize));

    // Save references to the GPU descriptors. 
    mPrevSolSrv = hGpuDescriptor;
    mCurrSolSrv = hGpuDescriptor.Offset(1, descriptorSize);
    mNextSolSrv = hGpuDescriptor.Offset(1, descriptorSize);
    mPrevSolUav = hGpuDescriptor.Offset(1, descriptorSize);
    mCurrSolUav = hGpuDescriptor.Offset(1, descriptorSize);
    mNextSolUav = hGpuDescriptor.Offset(1, descriptorSize);
}

void D3DGpuWaves::Disturb(ID3D12GraphicsCommandList* cmdList)
{
    cmdList->SetPipelineState(mDisturbPSO.Get());
    cmdList->SetComputeRootSignature(mRootSignature.Get());
    cmdList->SetComputeRootDescriptorTable(3, mCurrSolUav);
    // The current solution is in the GENERIC_READ state so it can be read by the vertex shader.
    // Change it to UNORDERED_ACCESS for the compute shader.  Note that a UAV can still be
    // read in a compute shader.
    cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mCurrSol.Get(),
        D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));

    for (size_t d = 0; d < mDisturbCount; d++)
    {
        UINT i = MathHelper::Rand(4, RowCount() - 5);
        UINT j = MathHelper::Rand(4, ColumnCount() - 5);
        float magnitude = MathHelper::RandF(mWaveLow, mWaveHigh);

        // Set the disturb constants.
        UINT disturbIndex[2] = { j, i };
        cmdList->SetComputeRoot32BitConstants(0, 1, &magnitude, 3);
        cmdList->SetComputeRoot32BitConstants(0, 2, disturbIndex, 4);
        // One thread group kicks off one thread, which displaces the height of one
        // vertex and its neighbors.
        cmdList->Dispatch(1, 1, 1);
    }
}

void D3DGpuWaves::Update(ID3D12GraphicsCommandList* cmdList)
{
    static float t = 0.0f;
    t += Engine::GetEngine()->DeltaTime();

    cmdList->SetPipelineState(mUpdatePSO.Get());
    cmdList->SetComputeRootSignature(mRootSignature.Get());

    // Only update the simulation at the specified time step.
    if (t >= mTimeStep)
    {
        // Set the update constants.
        cmdList->SetComputeRoot32BitConstants(0, 3, mK, 0);
        cmdList->SetComputeRootDescriptorTable(1, mPrevSolUav);
        cmdList->SetComputeRootDescriptorTable(2, mCurrSolUav);
        cmdList->SetComputeRootDescriptorTable(3, mNextSolUav);

        // How many groups do we need to dispatch to cover the wave grid.  
        // Note that mNumRows and mNumCols should be divisible by 16
        // so there is no remainder.
        UINT numGroupsX = mNumCols / 16;
        UINT numGroupsY = mNumRows / 16;
        cmdList->Dispatch(numGroupsX, numGroupsY, 1);

        // Ping-pong buffers in preparation for the next update.
        // The previous solution is no longer needed and 
        // becomes the target of the next solution in the next update.
        // The current solution becomes the previous solution.
        // The next solution becomes the current solution.
        RotateResources();

        t = 0.0f; // reset time
    }
    // The current solution needs to be able to be read by the vertex shader, 
    // so change its state to GENERIC_READ.
    cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mCurrSol.Get(),
        D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_GENERIC_READ));
}

void D3DGpuWaves::BuildRootSignature()
{
    CD3DX12_DESCRIPTOR_RANGE uavTable0;
    uavTable0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);
    CD3DX12_DESCRIPTOR_RANGE uavTable1;
    uavTable1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 1);
    CD3DX12_DESCRIPTOR_RANGE uavTable2;
    uavTable2.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 2);

    // Root parameter can be a table, root descriptor or root constants.
    CD3DX12_ROOT_PARAMETER slotRootParameter[4];
    // Performance TIP: Order from most frequent to least frequent.
    slotRootParameter[0].InitAsConstants(6, 0);
    slotRootParameter[1].InitAsDescriptorTable(1, &uavTable0);
    slotRootParameter[2].InitAsDescriptorTable(1, &uavTable1);
    slotRootParameter[3].InitAsDescriptorTable(1, &uavTable2);
    // A root signature is an array of root parameters.
    CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc
    (
        4, slotRootParameter,
        0, nullptr,
        D3D12_ROOT_SIGNATURE_FLAG_NONE
    );

    // create a root signature with a single slot which points to a descriptor range 
    // consisting of a single constant buffer
    Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
    HRESULT hr = D3D12SerializeRootSignature
    (
        &rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
        serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf()
    );

    if (errorBlob != nullptr)
    {
        ::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
    }
    ThrowIfFailed(hr);
    ThrowIfFailed
    (
        md3dDevice->CreateRootSignature
        (
            0,
            serializedRootSig->GetBufferPointer(),
            serializedRootSig->GetBufferSize(),
            IID_PPV_ARGS(mRootSignature.GetAddressOf())
        )
    );
}

void D3DGpuWaves::BuildPSO()
{
    std::string shaderData = 
        "//=============================================================================\n"
        "// WaveSim.hlsl by Frank Luna (C) 2011 All Rights Reserved.\n"
        "//\n"
        "// UpdateWavesCS(): Solves 2D wave equation using the compute shader.\n"
        "//\n"
        "// DisturbWavesCS(): Runs one thread to disturb a grid height and its\n"
        "//     neighbors to generate a wave. \n"
        "//=============================================================================\n"
        "\n"
        "// For updating the simulation.\n"
        "cbuffer cbUpdateSettings\n"
        "{\n"
        "    float gWaveConstant0;\n"
        "    float gWaveConstant1;\n"
        "    float gWaveConstant2;\n"
        "\n"
        "    float gDisturbMag;\n"
        "    int2 gDisturbIndex;\n"
        "};\n"
        "\n"
        "RWTexture2D<float> gPrevSolInput : register(u0);\n"
        "RWTexture2D<float> gCurrSolInput : register(u1);\n"
        "RWTexture2D<float> gOutput       : register(u2);\n"
        "\n"
        "[numthreads(16, 16, 1)]\n"
        "void UpdateWavesCS(int3 dispatchThreadID : SV_DispatchThreadID)\n"
        "{\n"
        "    // We do not need to do bounds checking because:\n"
        "    //	 *out-of-bounds reads return 0, which works for us--it just means the boundary of \n"
        "    //    our water simulation is clamped to 0 in local space.\n"
        "    //   *out-of-bounds writes are a no-op.\n"
        "    int x = dispatchThreadID.x;\n"
        "    int y = dispatchThreadID.y;\n"
        "\n"
        "    gOutput[int2(x, y)] =\n"
        "        gWaveConstant0 * gPrevSolInput[int2(x, y)].r +\n"
        "        gWaveConstant1 * gCurrSolInput[int2(x, y)].r +\n"
        "        gWaveConstant2 *\n"
        "        (\n"
        "            gCurrSolInput[int2(x, y + 1)].r +\n"
        "            gCurrSolInput[int2(x, y - 1)].r +\n"
        "            gCurrSolInput[int2(x + 1, y)].r +\n"
        "            gCurrSolInput[int2(x - 1, y)].r\n"
        "        );\n"
        "}\n"
        "\n"
        "[numthreads(1, 1, 1)]\n"
        "void DisturbWavesCS\n"
        "(   \n"
        "    int3 groupThreadID : SV_GroupThreadID,\n"
        "    int3 dispatchThreadID : SV_DispatchThreadID\n"
        ")\n"
        "{\n"
        "    // We do not need to do bounds checking because:\n"
        "    //	 *out-of-bounds reads return 0, which works for us--it just means the boundary of \n"
        "    //    our water simulation is clamped to 0 in local space.\n"
        "    //   *out-of-bounds writes are a no-op.\n"
        "    int x = gDisturbIndex.x;\n"
        "    int y = gDisturbIndex.y;\n"
        "    float halfMag = 0.5f*gDisturbMag;\n"
        "\n"
        "    // Buffer is RW so operator += is well defined.\n"
        "    gOutput[int2(x, y)] += gDisturbMag;\n"
        "    gOutput[int2(x + 1, y)] += halfMag;\n"
        "    gOutput[int2(x - 1, y)] += halfMag;\n"
        "    gOutput[int2(x, y + 1)] += halfMag;\n"
        "    gOutput[int2(x, y - 1)] += halfMag;\n"
        "}\n";

    auto updateCS = D3DUtils::CompileShader(shaderData, nullptr, "UpdateWavesCS", "cs_5_0");
    auto disturbCS = D3DUtils::CompileShader(shaderData, nullptr, "DisturbWavesCS", "cs_5_0");

    D3D12_COMPUTE_PIPELINE_STATE_DESC updatePSO = {};
    updatePSO.pRootSignature = mRootSignature.Get();
    updatePSO.CS =
    {
        reinterpret_cast<BYTE*>(updateCS->GetBufferPointer()),
        updateCS->GetBufferSize()
    };
    updatePSO.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
    ThrowIfFailed(md3dDevice->CreateComputePipelineState(&updatePSO, IID_PPV_ARGS(mUpdatePSO.ReleaseAndGetAddressOf())));

    D3D12_COMPUTE_PIPELINE_STATE_DESC disturbPSO = {};
    disturbPSO.pRootSignature = mRootSignature.Get();
    disturbPSO.CS =
    {
        reinterpret_cast<BYTE*>(disturbCS->GetBufferPointer()),
        disturbCS->GetBufferSize()
    };
    disturbPSO.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
    ThrowIfFailed(md3dDevice->CreateComputePipelineState(&disturbPSO, IID_PPV_ARGS(mDisturbPSO.ReleaseAndGetAddressOf())));
}

} // end FireFlame