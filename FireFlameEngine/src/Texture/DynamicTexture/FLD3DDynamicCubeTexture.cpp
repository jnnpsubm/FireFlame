#include "FLD3DDynamicCubeTexture.h"
#include "../../Renderer/FLD3DRenderer.h"
#include "../../Scene/FLScene.h"
#include "../../Scene/Pass/FLPass.h"
#include "../../FLD3DUtils.h"
#include "../../Engine/FLEngine.h"
#include "../../ShaderWrapper/ShaderConstBuffer/FLShaderConstBuffer.h"

namespace FireFlame {
D3DDynamicCubeTexture::D3DDynamicCubeTexture
(
    const std::string& name,
    ID3D12Device* device, UINT width, UINT height
) :D3DTextureBase(name)
{
    md3dDevice = device;

    mWidth = width;
    mHeight = height;

    mViewport = { 0.0f, 0.0f, (float)width, (float)height, 0.0f, 1.0f };
    mScissorRect = { 0, 0, (int)width, (int)height };

    BuildResource();
    BuildDescriptorHeap();

    needUpdate = true;
}

void D3DDynamicCubeTexture::Render(ID3D12GraphicsCommandList* cmdList, Scene* scene)
{
    BuildCubeFaceCamera(0.0f, 2.0f, 0.0f);
    UpdatePassCBs(scene);

    cmdList->RSSetViewports(1, &Viewport());
    cmdList->RSSetScissorRects(1, &ScissorRect());

    // Change to RENDER_TARGET.
    cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(Resource(),
        D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET));

    UINT passCBByteSize = D3DUtils::CalcConstantBufferByteSize(sizeof(PassConstantsLight));

    auto engine = Engine::GetEngine();
    auto renderer = engine->GetRenderer();
    // For each cube map face.
    for (int i = 0; i < 6; ++i)
    {
        // Clear the back buffer and depth buffer.
        cmdList->ClearRenderTargetView(Rtv(i), renderer->GetDefaultClearColor(), 0, nullptr);
        cmdList->ClearDepthStencilView(mhCpuDsv, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

        // Specify the buffers we are going to render to.
        cmdList->OMSetRenderTargets(1, &Rtv(i), true, &mhCpuDsv);

        Pass pass;
        pass.CbvHeap = mCbvHeap;
        pass.CbvIndex = renderer->GetCurrFrameResIndex()*6 + i;
        pass.MSAAMode = 0;
        scene->DrawPass(cmdList, &pass);
    }

    // Change back to GENERIC_READ so we can read the texture in a shader.
    cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(Resource(),
        D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ));
}

void D3DDynamicCubeTexture::UpdatePassCBs(Scene* scene)
{
    using namespace DirectX;

    auto engine = Engine::GetEngine();
    auto renderer = engine->GetRenderer();

    PassConstantsLight passCB;
    passCB.NearZ = 1.0f;
    passCB.FarZ = 1000.0f;
    passCB.TotalTime = engine->TotalTime();
    passCB.DeltaTime = engine->DeltaTime();
    passCB.AmbientLight = { 0.25f, 0.25f, 0.35f, 1.0f };
    passCB.Lights[0].Direction = { 0.57735f, -0.57735f, 0.57735f };
    passCB.Lights[0].Strength = { 0.6f, 0.6f, 0.6f };
    passCB.Lights[1].Direction = { -0.57735f, -0.57735f, 0.57735f };
    passCB.Lights[1].Strength = { 0.3f, 0.3f, 0.3f };
    passCB.Lights[2].Direction = { 0.0f, -0.707f, -0.707f };
    passCB.Lights[2].Strength = { 0.15f, 0.15f, 0.15f };
    for (int i = 0; i < 6; ++i)
    {
        XMMATRIX view = mCubeFaceCamera[i].GetView();
        XMMATRIX proj = mCubeFaceCamera[i].GetProj();

        XMMATRIX viewProj = XMMatrixMultiply(view, proj);
        XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
        XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
        XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);

        XMStoreFloat4x4(&passCB.View, XMMatrixTranspose(view));
        XMStoreFloat4x4(&passCB.InvView, XMMatrixTranspose(invView));
        XMStoreFloat4x4(&passCB.Proj, XMMatrixTranspose(proj));
        XMStoreFloat4x4(&passCB.InvProj, XMMatrixTranspose(invProj));
        XMStoreFloat4x4(&passCB.ViewProj, XMMatrixTranspose(viewProj));
        XMStoreFloat4x4(&passCB.InvViewProj, XMMatrixTranspose(invViewProj));
        passCB.EyePosW = mCubeFaceCamera[i].GetPosition3f();
        passCB.RenderTargetSize = XMFLOAT2((float)mWidth, (float)mHeight);
        passCB.InvRenderTargetSize = XMFLOAT2(1.0f / mWidth, 1.0f / mHeight);

        auto currPassCB = mPassCBs[renderer->GetCurrFrameResIndex()].get();
        currPassCB->CopyData(i, passCB);
    }
}

void D3DDynamicCubeTexture::BuildCubeFaceCamera(float x, float y, float z)
{
    using namespace DirectX;

    // Generate the cube map about the given position.
    XMFLOAT3 center(x, y, z);
    XMFLOAT3 worldUp(0.0f, 1.0f, 0.0f);

    // Look along each coordinate axis.
    XMFLOAT3 targets[6] =
    {
        XMFLOAT3(x + 1.0f, y, z), // +X
        XMFLOAT3(x - 1.0f, y, z), // -X
        XMFLOAT3(x, y + 1.0f, z), // +Y
        XMFLOAT3(x, y - 1.0f, z), // -Y
        XMFLOAT3(x, y, z + 1.0f), // +Z
        XMFLOAT3(x, y, z - 1.0f)  // -Z
    };

    // Use world up vector (0,1,0) for all directions except +Y/-Y.  In these cases, we
    // are looking down +Y or -Y, so we need a different "up" vector.
    XMFLOAT3 ups[6] =
    {
        XMFLOAT3(0.0f, 1.0f, 0.0f),  // +X
        XMFLOAT3(0.0f, 1.0f, 0.0f),  // -X
        XMFLOAT3(0.0f, 0.0f, -1.0f), // +Y
        XMFLOAT3(0.0f, 0.0f, +1.0f), // -Y
        XMFLOAT3(0.0f, 1.0f, 0.0f),	 // +Z
        XMFLOAT3(0.0f, 1.0f, 0.0f)	 // -Z
    };

    for (int i = 0; i < 6; ++i)
    {
        mCubeFaceCamera[i].LookAt(center, targets[i], ups[i]);
        mCubeFaceCamera[i].SetLens(0.5f*XM_PI, 1.0f, 0.1f, 1000.0f);
        mCubeFaceCamera[i].UpdateViewMatrix();
    }
}

void D3DDynamicCubeTexture::BuildDescriptorHeap()
{
    D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
    srvHeapDesc.NumDescriptors = 1;
    srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    ThrowIfFailed(md3dDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&mSrvHeap)));

    mhCpuSrv = mSrvHeap->GetCPUDescriptorHandleForHeapStart();
    mhGpuSrv = mSrvHeap->GetGPUDescriptorHandleForHeapStart();

    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
    rtvHeapDesc.NumDescriptors = 6;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    rtvHeapDesc.NodeMask = 0;
    ThrowIfFailed(md3dDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(mRtvHeap.GetAddressOf())));

    auto rtvCpuStart = mRtvHeap->GetCPUDescriptorHandleForHeapStart();
    auto rtvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    for (int i = 0; i < 6; ++i)
        mhCpuRtv[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvCpuStart, i, rtvDescriptorSize);

    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
    dsvHeapDesc.NumDescriptors = 1;
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    dsvHeapDesc.NodeMask = 0;
    ThrowIfFailed(md3dDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(mDsvHeap.GetAddressOf())));

    mhCpuDsv = CD3DX12_CPU_DESCRIPTOR_HANDLE(mDsvHeap->GetCPUDescriptorHandleForHeapStart());

    D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
    cbvHeapDesc.NumDescriptors = 6*Engine::NumFrameResources();
    cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    cbvHeapDesc.NodeMask = 0;
    ThrowIfFailed(md3dDevice->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&mCbvHeap)));

    BuildDescriptors();
}

void D3DDynamicCubeTexture::OnResize(UINT newWidth, UINT newHeight)
{
    if ((mWidth != newWidth) || (mHeight != newHeight))
    {
        mWidth = newWidth;
        mHeight = newHeight;

        BuildResource();
        BuildDescriptors();
    }
}

void D3DDynamicCubeTexture::BuildDescriptors()
{
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = mFormat;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
    srvDesc.TextureCube.MostDetailedMip = 0;
    srvDesc.TextureCube.MipLevels = 1;
    srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
    // Create SRV to the entire cubemap resource.
    md3dDevice->CreateShaderResourceView(mCubeMap.Get(), &srvDesc, mhCpuSrv);

    // Create RTV to each cube face.
    for (int i = 0; i < 6; ++i)
    {
        D3D12_RENDER_TARGET_VIEW_DESC rtvDesc;
        rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
        rtvDesc.Format = mFormat;
        rtvDesc.Texture2DArray.MipSlice = 0;
        rtvDesc.Texture2DArray.PlaneSlice = 0;
        // Render target to ith element.
        rtvDesc.Texture2DArray.FirstArraySlice = i;
        // Only view one element of the array.
        rtvDesc.Texture2DArray.ArraySize = 1;
        // Create RTV to ith cubemap face.
        md3dDevice->CreateRenderTargetView(mCubeMap.Get(), &rtvDesc, mhCpuRtv[i]);
    }

    // Create descriptor to mip level 0 of entire resource using the format of the resource.
    md3dDevice->CreateDepthStencilView(mDepthStencil.Get(), nullptr, mhCpuDsv);

    auto CbvDesSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    UINT passCBByteSize = D3DUtils::CalcConstantBufferByteSize(sizeof(PassConstantsLight));
    for (UINT frameIndex = 0; frameIndex < (UINT)Engine::NumFrameResources(); ++frameIndex) {
        for (UINT i = 0; i < 6; ++i) {
            D3D12_GPU_VIRTUAL_ADDRESS cbAddress = mPassCBs[frameIndex]->Resource()->GetGPUVirtualAddress();

            // Offset to the ith object constant buffer in the buffer.
            cbAddress += i * passCBByteSize;

            // Offset to the pass cbv in the descriptor heap.
            int heapIndex = frameIndex * 6 + i;
            auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(mCbvHeap->GetCPUDescriptorHandleForHeapStart());
            handle.Offset(heapIndex, CbvDesSize);

            D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
            cbvDesc.BufferLocation = cbAddress;
            cbvDesc.SizeInBytes = passCBByteSize;
            md3dDevice->CreateConstantBufferView(&cbvDesc, handle);
        }
    }
}

void D3DDynamicCubeTexture::BuildResource()
{
    D3D12_RESOURCE_DESC texDesc;
    ZeroMemory(&texDesc, sizeof(D3D12_RESOURCE_DESC));
    texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    texDesc.Alignment = 0;
    texDesc.Width = mWidth;
    texDesc.Height = mHeight;
    texDesc.DepthOrArraySize = 6;
    texDesc.MipLevels = 1;
    texDesc.Format = mFormat;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

    auto renderer = Engine::GetEngine()->GetRenderer();
    D3D12_CLEAR_VALUE OptClearValue = {};
    OptClearValue.Format = mFormat;
    memcpy(OptClearValue.Color, renderer->GetDefaultClearColor(), sizeof(float) * 4);

    ThrowIfFailed(md3dDevice->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &texDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        &OptClearValue,
        IID_PPV_ARGS(&mCubeMap)));

    D3D12_RESOURCE_DESC depthStencilDesc;
    depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    depthStencilDesc.Alignment = 0;
    depthStencilDesc.Width = mWidth;
    depthStencilDesc.Height = mHeight;
    depthStencilDesc.DepthOrArraySize = 1;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.Format = mDepthStencilFormat;
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE optClear;
    optClear.Format = mDepthStencilFormat;
    optClear.DepthStencil.Depth = 1.0f;
    optClear.DepthStencil.Stencil = 0;
    ThrowIfFailed(md3dDevice->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &depthStencilDesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &optClear,
        IID_PPV_ARGS(mDepthStencil.GetAddressOf())));

    // Transition the resource from its initial state to be used as a depth buffer.
    /*mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mDepthStencil.Get(),
        D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));*/

    for (int i = 0; i < Engine::NumFrameResources(); ++i) {
        auto PassCB = std::make_unique<UploadBuffer>(true);
        PassCB->Init(md3dDevice, 6, sizeof(PassConstantsLight));
        mPassCBs.emplace_back(std::move(PassCB));
    }
}
}