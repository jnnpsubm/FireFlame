//***************************************************************************************
// BlurFilter.cpp by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************
#include "FLD3DGaussBlurFilter.h"
#include "..\FLD3DUtils.h"
#include <assert.h>

namespace FireFlame {
D3DGaussBlurFilter::D3DGaussBlurFilter
(
    ID3D12Device* device,
    UINT width, UINT height,
    DXGI_FORMAT format,
    int blurCount,
    float sigma
):
    md3dDevice(device),
    mWidth(width),
    mHeight(height),
    mFormat(format),
    mBlurCount(blurCount),
    mSigma(sigma)
{
    BuildRootSignature();
    BuildPSOs();
}

ID3D12Resource* D3DGaussBlurFilter::GetResultResource()
{
    return mBlurTexture0.Get();
}

void D3DGaussBlurFilter::BuildRootSignature()
{
    CD3DX12_DESCRIPTOR_RANGE srvTable;
    srvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

    CD3DX12_DESCRIPTOR_RANGE uavTable;
    uavTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);

    // Root parameter can be a table, root descriptor or root constants.
    CD3DX12_ROOT_PARAMETER slotRootParameter[3];

    // Performance TIP: Order from most frequent to least frequent.
    slotRootParameter[0].InitAsConstants(12, 0);
    slotRootParameter[1].InitAsDescriptorTable(1, &srvTable);
    slotRootParameter[2].InitAsDescriptorTable(1, &uavTable);

    // A root signature is an array of root parameters.
    CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(3, slotRootParameter,
        0, nullptr,
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    // create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
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
        md3dDevice->CreateRootSignature(
        0,
        serializedRootSig->GetBufferPointer(),
        serializedRootSig->GetBufferSize(),
        IID_PPV_ARGS(mRootSignature.GetAddressOf()))
    );
}

void D3DGaussBlurFilter::BuildPSOs()
{
    std::string shaderData = 
        "//=============================================================================\n"
        "// Performs a separable Gaussian blur with a blur radius up to 5 pixels.\n"
        "//=============================================================================\n"
        "\n"
        "cbuffer cbSettings : register(b0)\n"
        "{\n"
        "    // We cannot have an array entry in a constant buffer that gets mapped onto\n"
        "    // root constants, so list each element.  \n"
        "\n"
        "    int gBlurRadius;\n"
        "\n"
        "    // Support up to 11 blur weights.\n"
        "    float w0;\n"
        "    float w1;\n"
        "    float w2;\n"
        "    float w3;\n"
        "    float w4;\n"
        "    float w5;\n"
        "    float w6;\n"
        "    float w7;\n"
        "    float w8;\n"
        "    float w9;\n"
        "    float w10;\n"
        "};\n"
        "\n"
        "static const int gMaxBlurRadius = 5;\n"
        "\n"
        "\n"
        "Texture2D gInput            : register(t0);\n"
        "RWTexture2D<float4> gOutput : register(u0);\n"
        "\n"
        "#define N 256\n"
        "#define CacheSize (N + 2*gMaxBlurRadius)\n"
        "groupshared float4 gCache[CacheSize];\n"
        "\n"
        "[numthreads(N, 1, 1)]\n"
        "void HorzBlurCS(int3 groupThreadID : SV_GroupThreadID,\n"
        "    int3 dispatchThreadID : SV_DispatchThreadID)\n"
        "{\n"
        "    // Put in an array for each indexing.\n"
        "    float weights[11] = { w0, w1, w2, w3, w4, w5, w6, w7, w8, w9, w10 };\n"
        "\n"
        "    //\n"
        "    // Fill local thread storage to reduce bandwidth.  To blur \n"
        "    // N pixels, we will need to load N + 2*BlurRadius pixels\n"
        "    // due to the blur radius.\n"
        "    //\n"
        "\n"
        "    // This thread group runs N threads.  To get the extra 2*BlurRadius pixels, \n"
        "    // have 2*BlurRadius threads sample an extra pixel.\n"
        "    if (groupThreadID.x < gBlurRadius)\n"
        "    {\n"
        "        // Clamp out of bound samples that occur at image borders.\n"
        "        int x = max(dispatchThreadID.x - gBlurRadius, 0);\n"
        "        gCache[groupThreadID.x] = gInput[int2(x, dispatchThreadID.y)];\n"
        "    }\n"
        "    if (groupThreadID.x >= N - gBlurRadius)\n"
        "    {\n"
        "        // Clamp out of bound samples that occur at image borders.\n"
        "        int x = min(dispatchThreadID.x + gBlurRadius, gInput.Length.x - 1);\n"
        "        gCache[groupThreadID.x + 2 * gBlurRadius] = gInput[int2(x, dispatchThreadID.y)];\n"
        "    }\n"
        "\n"
        "    // Clamp out of bound samples that occur at image borders.\n"
        "    gCache[groupThreadID.x + gBlurRadius] = gInput[min(dispatchThreadID.xy, gInput.Length.xy - 1)];\n"
        "\n"
        "    // Wait for all threads to finish.\n"
        "    GroupMemoryBarrierWithGroupSync();\n"
        "\n"
        "    //\n"
        "    // Now blur each pixel.\n"
        "    //\n"
        "\n"
        "    float4 blurColor = float4(0, 0, 0, 0);\n"
        "\n"
        "    for (int i = -gBlurRadius; i <= gBlurRadius; ++i)\n"
        "    {\n"
        "        int k = groupThreadID.x + gBlurRadius + i;\n"
        "\n"
        "        blurColor += weights[i + gBlurRadius] * gCache[k];\n"
        "    }\n"
        "\n"
        "    gOutput[dispatchThreadID.xy] = blurColor;\n"
        "}\n"
        "\n"
        "[numthreads(1, N, 1)]\n"
        "void VertBlurCS(int3 groupThreadID : SV_GroupThreadID,\n"
        "    int3 dispatchThreadID : SV_DispatchThreadID)\n"
        "{\n"
        "    // Put in an array for each indexing.\n"
        "    float weights[11] = { w0, w1, w2, w3, w4, w5, w6, w7, w8, w9, w10 };\n"
        "\n"
        "    //\n"
        "    // Fill local thread storage to reduce bandwidth.  To blur \n"
        "    // N pixels, we will need to load N + 2*BlurRadius pixels\n"
        "    // due to the blur radius.\n"
        "    //\n"
        "\n"
        "    // This thread group runs N threads.  To get the extra 2*BlurRadius pixels, \n"
        "    // have 2*BlurRadius threads sample an extra pixel.\n"
        "    if (groupThreadID.y < gBlurRadius)\n"
        "    {\n"
        "        // Clamp out of bound samples that occur at image borders.\n"
        "        int y = max(dispatchThreadID.y - gBlurRadius, 0);\n"
        "        gCache[groupThreadID.y] = gInput[int2(dispatchThreadID.x, y)];\n"
        "    }\n"
        "    if (groupThreadID.y >= N - gBlurRadius)\n"
        "    {\n"
        "        // Clamp out of bound samples that occur at image borders.\n"
        "        int y = min(dispatchThreadID.y + gBlurRadius, gInput.Length.y - 1);\n"
        "        gCache[groupThreadID.y + 2 * gBlurRadius] = gInput[int2(dispatchThreadID.x, y)];\n"
        "    }\n"
        "\n"
        "    // Clamp out of bound samples that occur at image borders.\n"
        "    gCache[groupThreadID.y + gBlurRadius] = gInput[min(dispatchThreadID.xy, gInput.Length.xy - 1)];\n"
        "\n"
        "\n"
        "    // Wait for all threads to finish.\n"
        "    GroupMemoryBarrierWithGroupSync();\n"
        "\n"
        "    //\n"
        "    // Now blur each pixel.\n"
        "    //\n"
        "\n"
        "    float4 blurColor = float4(0, 0, 0, 0);\n"
        "\n"
        "    for (int i = -gBlurRadius; i <= gBlurRadius; ++i)\n"
        "    {\n"
        "        int k = groupThreadID.y + gBlurRadius + i;\n"
        "\n"
        "        blurColor += weights[i + gBlurRadius] * gCache[k];\n"
        "    }\n"
        "\n"
        "    gOutput[dispatchThreadID.xy] = blurColor;\n"
        "}\n";
    auto HorzBlurCS = D3DUtils::CompileShader(shaderData, nullptr, "HorzBlurCS", "cs_5_0");
    auto VertBlurCS = D3DUtils::CompileShader(shaderData, nullptr, "VertBlurCS", "cs_5_0");

    D3D12_COMPUTE_PIPELINE_STATE_DESC horzBlurPSO = {};
    horzBlurPSO.pRootSignature = mRootSignature.Get();
    horzBlurPSO.CS =
    {
        reinterpret_cast<BYTE*>(HorzBlurCS->GetBufferPointer()),
        HorzBlurCS->GetBufferSize()
    };
    horzBlurPSO.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
    ThrowIfFailed(md3dDevice->CreateComputePipelineState(&horzBlurPSO, IID_PPV_ARGS(mPSOHorz.GetAddressOf())));

    D3D12_COMPUTE_PIPELINE_STATE_DESC vertBlurPSO = {};
    vertBlurPSO.pRootSignature = mRootSignature.Get();
    vertBlurPSO.CS =
    {
        reinterpret_cast<BYTE*>(VertBlurCS->GetBufferPointer()),
        VertBlurCS->GetBufferSize()
    };
    vertBlurPSO.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
    ThrowIfFailed(md3dDevice->CreateComputePipelineState(&vertBlurPSO, IID_PPV_ARGS(mPSOVert.GetAddressOf())));
}

void D3DGaussBlurFilter::BuildDescriptors(UINT descriptorSize)
{
    D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
    srvHeapDesc.NumDescriptors = 4;
    srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    ThrowIfFailed(md3dDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(mDescriptorHeap.GetAddressOf())));

    // Save references to the descriptors. 
    CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuDescriptor(mDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
    mBlur0CpuSrv = hCpuDescriptor;
    mBlur0CpuUav = hCpuDescriptor.Offset(1, descriptorSize);
    mBlur1CpuSrv = hCpuDescriptor.Offset(1, descriptorSize);
    mBlur1CpuUav = hCpuDescriptor.Offset(1, descriptorSize);

    CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuDescriptor(mDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
    mBlur0GpuSrv = hGpuDescriptor;
    mBlur0GpuUav = hGpuDescriptor.Offset(1, descriptorSize);
    mBlur1GpuSrv = hGpuDescriptor.Offset(1, descriptorSize);
    mBlur1GpuUav = hGpuDescriptor.Offset(1, descriptorSize);

    BuildDescriptors();
}

void D3DGaussBlurFilter::OnResize(UINT sampleCount, UINT sampleQuality, UINT newWidth, UINT newHeight)
{
    if ((mWidth != newWidth) || (mHeight != newHeight))
    {
        mWidth = newWidth;
        mHeight = newHeight;
        BuildResources(sampleCount, sampleQuality);
        // New resource, so we need new descriptors to that resource.
        BuildDescriptors();
    }
}

void D3DGaussBlurFilter::Go
(
    ID3D12GraphicsCommandList* cmdList,
    ID3D12Resource* inputResource
)
{
    auto weights = CalcGaussWeights();
    int blurRadius = (int)weights.size() / 2;

    cmdList->SetDescriptorHeaps(1, mDescriptorHeap.GetAddressOf());
    cmdList->SetComputeRootSignature(mRootSignature.Get());

    cmdList->SetComputeRoot32BitConstants(0, 1, &blurRadius, 0);
    cmdList->SetComputeRoot32BitConstants(0, (UINT)weights.size(), weights.data(), 1);

    /*cmdList->ResourceBarrier
    (
        1,
        &CD3DX12_RESOURCE_BARRIER::Transition
        (
            inputResource,
            D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_SOURCE
        )
    );*/
    cmdList->ResourceBarrier
    (
        1, 
        &CD3DX12_RESOURCE_BARRIER::Transition
        (
            mBlurTexture0.Get(),
            D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST
        )
    );
    cmdList->CopyResource(mBlurTexture0.Get(), inputResource);

    cmdList->ResourceBarrier
    (
        1, 
        &CD3DX12_RESOURCE_BARRIER::Transition
        (
            mBlurTexture0.Get(),
            D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ
        )
    );
    for (int i = 0; i < mBlurCount; ++i)
    {
        // Horizontal Blur pass.
        cmdList->SetPipelineState(mPSOHorz.Get());

        cmdList->SetComputeRootDescriptorTable(1, mBlur0GpuSrv);
        cmdList->SetComputeRootDescriptorTable(2, mBlur1GpuUav);

        // How many groups do we need to dispatch to cover a row of pixels, where each
        // group covers 256 pixels (the 256 is defined in the ComputeShader).
        UINT numGroupsX = (UINT)ceilf(mWidth / 256.0f);
        cmdList->Dispatch(numGroupsX, mHeight, 1);
        cmdList->ResourceBarrier
        (
            1, 
            &CD3DX12_RESOURCE_BARRIER::Transition
            (
                mBlurTexture0.Get(),
                D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS
            )
        );
        cmdList->ResourceBarrier
        (
            1, 
            &CD3DX12_RESOURCE_BARRIER::Transition
            (
                mBlurTexture1.Get(),
                D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_GENERIC_READ
            )
        );

        // Vertical Blur pass.
        cmdList->SetPipelineState(mPSOVert.Get());

        cmdList->SetComputeRootDescriptorTable(1, mBlur1GpuSrv);
        cmdList->SetComputeRootDescriptorTable(2, mBlur0GpuUav);

        // How many groups do we need to dispatch to cover a column of pixels, where each
        // group covers 256 pixels  (the 256 is defined in the ComputeShader).
        UINT numGroupsY = (UINT)ceilf(mHeight / 256.0f);
        cmdList->Dispatch(mWidth, numGroupsY, 1);
        cmdList->ResourceBarrier
        (
            1, 
            &CD3DX12_RESOURCE_BARRIER::Transition
            (
                mBlurTexture0.Get(),
                D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_GENERIC_READ
            )
        );
        cmdList->ResourceBarrier
        (
            1, 
            &CD3DX12_RESOURCE_BARRIER::Transition
            (
                mBlurTexture1.Get(),
                D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS
            )
        );
    }
}

std::vector<float> D3DGaussBlurFilter::CalcGaussWeights()
{
    float twoSigma2 = 2.0f*mSigma*mSigma;

    // Estimate the blur radius based on sigma since sigma controls the "width" of the bell curve.
    // For example, for sigma = 3, the width of the bell curve is 
    int blurRadius = (int)ceil(2.0f * mSigma);

    assert(blurRadius <= mMaxBlurRadius);

    std::vector<float> weights;
    weights.resize(2 * blurRadius + 1);

    float weightSum = 0.0f;

    for (int i = -blurRadius; i <= blurRadius; ++i)
    {
        float x = (float)i;

        weights[i + blurRadius] = expf(-x*x / twoSigma2);

        weightSum += weights[i + blurRadius];
    }

    // Divide by the sum so all the weights add up to 1.0.
    for (int i = 0; i < weights.size(); ++i)
    {
        weights[i] /= weightSum;
    }

    return weights;
}

void D3DGaussBlurFilter::BuildDescriptors()
{
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = mFormat;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;

    D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
    uavDesc.Format = mFormat;
    uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
    uavDesc.Texture2D.MipSlice = 0;

    md3dDevice->CreateShaderResourceView(mBlurTexture0.Get(), &srvDesc, mBlur0CpuSrv);
    md3dDevice->CreateUnorderedAccessView(mBlurTexture0.Get(), nullptr, &uavDesc, mBlur0CpuUav);

    md3dDevice->CreateShaderResourceView(mBlurTexture1.Get(), &srvDesc, mBlur1CpuSrv);
    md3dDevice->CreateUnorderedAccessView(mBlurTexture1.Get(), nullptr, &uavDesc, mBlur1CpuUav);
}

void D3DGaussBlurFilter::BuildResources(UINT sampleCount, UINT sampleQuality)
{
    // Note, compressed formats cannot be used for UAV.  We get error like:
    // ERROR: ID3D11Device::CreateTexture2D: The format (0x4d, BC3_UNORM) 
    // cannot be bound as an UnorderedAccessView, or cast to a format that
    // could be bound as an UnorderedAccessView.  Therefore this format 
    // does not support D3D11_BIND_UNORDERED_ACCESS.
    D3D12_RESOURCE_DESC texDesc;
    ZeroMemory(&texDesc, sizeof(D3D12_RESOURCE_DESC));
    texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    texDesc.Alignment = 0;
    texDesc.Width = mWidth;
    texDesc.Height = mHeight;
    texDesc.DepthOrArraySize = 1;
    texDesc.MipLevels = 1;
    texDesc.Format = mFormat;
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
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(mBlurTexture0.ReleaseAndGetAddressOf())
        )
    );
    ThrowIfFailed
    (
        md3dDevice->CreateCommittedResource
        (
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &texDesc,
            D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
            nullptr,
            IID_PPV_ARGS(mBlurTexture1.ReleaseAndGetAddressOf())
        )
    );
}
} // end FireFlame