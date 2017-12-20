#include "FLD3DSobelFilter.h"
#include "..\FLD3DUtils.h"
#include <assert.h>

namespace FireFlame {
D3DSobelFilter::D3DSobelFilter
(
    ID3D12Device* device, Mode mode,
    UINT width, UINT height,
    DXGI_FORMAT format
):
    md3dDevice(device),
    mode(mode),
    mWidth(width),
    mHeight(height),
    mFormat(format)
{
    BuildResources();
    BuildDescriptors();
    BuildRootSignature();
    BuildPSOs();
}

ID3D12Resource* D3DSobelFilter::GetResultResource()
{
    return mOutTexture.Get();
}

void D3DSobelFilter::OnResize(UINT sampleCount, UINT sampleQuality, UINT newWidth, UINT newHeight)
{
    if ((mWidth != newWidth) || (mHeight != newHeight))
    {
        mWidth = newWidth;
        mHeight = newHeight;
        BuildResources();
        BuildDescriptors();
    }
}

void D3DSobelFilter::Go
(
    ID3D12GraphicsCommandList* cmdList,
    ID3D12Resource* inputResource,
    ID3D12DescriptorHeap* heap,
    D3D12_GPU_DESCRIPTOR_HANDLE descriptor
)
{
    cmdList->SetComputeRootSignature(mRootSignature.Get());
    if (mode == modeEdge)
        cmdList->SetPipelineState(mPSOEdge.Get());
    else
        cmdList->SetPipelineState(mPSOCartoon.Get());

    cmdList->SetDescriptorHeaps(1, &heap);
    cmdList->SetComputeRootDescriptorTable(0, descriptor);
    cmdList->SetDescriptorHeaps(1, mDescriptorHeap.GetAddressOf());
    cmdList->SetComputeRootDescriptorTable(1, mDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

    cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mOutTexture.Get(),
        D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));

    // How many groups do we need to dispatch to cover image, where each
    // group covers 16x16 pixels.
    UINT numGroupsX = (UINT)ceilf(mWidth / 16.0f);
    UINT numGroupsY = (UINT)ceilf(mHeight / 16.0f);
    cmdList->Dispatch(numGroupsX, numGroupsY, 1);

    cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mOutTexture.Get(),
        D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_GENERIC_READ));
}

void D3DSobelFilter::BuildResources()
{
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
            IID_PPV_ARGS(mOutTexture.ReleaseAndGetAddressOf())
        )
    );
}

void D3DSobelFilter::BuildDescriptors()
{
    D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
    srvHeapDesc.NumDescriptors = 1;
    srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    ThrowIfFailed(md3dDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(mDescriptorHeap.GetAddressOf())));

    D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
    uavDesc.Format = mFormat;
    uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
    uavDesc.Texture2D.MipSlice = 0;
    md3dDevice->CreateUnorderedAccessView(mOutTexture.Get(), nullptr, &uavDesc, mDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
}

void D3DSobelFilter::BuildRootSignature()
{
    CD3DX12_DESCRIPTOR_RANGE srvTable;
    srvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
    CD3DX12_DESCRIPTOR_RANGE uavTable;
    uavTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);

    // Root parameter can be a table, root descriptor or root constants.
    CD3DX12_ROOT_PARAMETER slotRootParameter[2];
    // Performance TIP: Order from most frequent to least frequent.
    slotRootParameter[0].InitAsDescriptorTable(1, &srvTable);
    slotRootParameter[1].InitAsDescriptorTable(1, &uavTable);
    // A root signature is an array of root parameters.
    CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(2, slotRootParameter,
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

void D3DSobelFilter::BuildPSOs()
{
    std::string shaderData = 
        "//=============================================================================\n"
        "// Performs edge detection using Sobel operator.\n"
        "//=============================================================================\n"
        "\n"
        "Texture2D gInput            : register(t0);\n"
        "RWTexture2D<float4> gOutput : register(u0);\n"
        "\n"
        "\n"
        "// Approximates luminance (\"brightness\") from an RGB value.  These weights are derived from\n"
        "// experiment based on eye sensitivity to different wavelengths of light.\n"
        "float CalcLuminance(float3 color)\n"
        "{\n"
        "    return dot(color, float3(0.299f, 0.587f, 0.114f));\n"
        "}\n"
        "\n"
        "[numthreads(16, 16, 1)]\n"
        "void SobelCS(int3 dispatchThreadID : SV_DispatchThreadID)\n"
        "{\n"
        "    // Sample the pixels in the neighborhood of this pixel.\n"
        "	float4 c[3][3];\n"
        "	for(int i = 0; i < 3; ++i)\n"
        "	{\n"
        "		for(int j = 0; j < 3; ++j)\n"
        "		{\n"
        "			int2 xy = dispatchThreadID.xy + int2(-1 + j, -1 + i);\n"
        "			c[i][j] = gInput[xy]; \n"
        "		}\n"
        "	}\n"
        "\n"
        "	// For each color channel, estimate partial x derivative using Sobel scheme.\n"
        "	float4 Gx = -1.0f*c[0][0] - 2.0f*c[1][0] - 1.0f*c[2][0] + 1.0f*c[0][2] + 2.0f*c[1][2] + 1.0f*c[2][2];\n"
        "\n"
        "	// For each color channel, estimate partial y derivative using Sobel scheme.\n"
        "	float4 Gy = -1.0f*c[2][0] - 2.0f*c[2][1] - 1.0f*c[2][1] + 1.0f*c[0][0] + 2.0f*c[0][1] + 1.0f*c[0][2];\n"
        "\n"
        "	// Gradient is (Gx, Gy).  For each color channel, compute magnitude to get maximum rate of change.\n"
        "	float4 mag = sqrt(Gx*Gx + Gy*Gy);\n"
        "\n"
        "	// Make edges black, and nonedges white.\n"
        "	mag = 1.0f - saturate(CalcLuminance(mag.rgb));\n"
        "#ifdef COMPOSITE\n"
        "	gOutput[dispatchThreadID.xy] = mag*gInput[dispatchThreadID.xy];\n"
        "#else\n"
        "    gOutput[dispatchThreadID.xy] = mag;\n"
        "#endif\n"
        "}\n";
    std::vector<D3D_SHADER_MACRO> macros(2);
    macros[0].Name = "COMPOSITE";
    macros[0].Definition = "1";
    macros[1].Name = nullptr;
    macros[1].Definition = nullptr;
    auto edgeCS = D3DUtils::CompileShader(shaderData, nullptr, "SobelCS", "cs_5_0");
    auto cartoonCS = D3DUtils::CompileShader(shaderData, macros.data(), "SobelCS", "cs_5_0");

    D3D12_COMPUTE_PIPELINE_STATE_DESC edgePSO = {};
    edgePSO.pRootSignature = mRootSignature.Get();
    edgePSO.CS =
    {
        reinterpret_cast<BYTE*>(edgeCS->GetBufferPointer()),
        edgeCS->GetBufferSize()
    };
    edgePSO.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
    ThrowIfFailed(md3dDevice->CreateComputePipelineState(&edgePSO, IID_PPV_ARGS(mPSOEdge.GetAddressOf())));

    D3D12_COMPUTE_PIPELINE_STATE_DESC cartoonPSO = {};
    cartoonPSO.pRootSignature = mRootSignature.Get();
    cartoonPSO.CS =
    {
        reinterpret_cast<BYTE*>(cartoonCS->GetBufferPointer()),
        cartoonCS->GetBufferSize()
    };
    cartoonPSO.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
    ThrowIfFailed(md3dDevice->CreateComputePipelineState(&cartoonPSO, IID_PPV_ARGS(mPSOCartoon.GetAddressOf())));
}
} // end FireFlame