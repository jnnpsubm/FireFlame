#pragma once
#include "../FLD3DTexture.h"
#include "../../3rd_utils/d3dx12.h"
#include "../../GPUMemory/D3DUploadBuffer.h"
#include "../../Camera/FLD3DCamera.h"
#include <memory>

namespace FireFlame{
class D3DDynamicCubeTexture : public D3DTextureBase{
public:
    D3DDynamicCubeTexture(const std::string& name,
        ID3D12Device* device, UINT width, UINT height);

    void Render(ID3D12GraphicsCommandList* cmdList, Scene* scene) override final;
    ID3D12Resource* Resource() override final {
        return mCubeMap.Get();
    }
    void OnResize(UINT newWidth, UINT newHeight);

private:
    void BuildResource();
    void BuildDescriptorHeap();
    void BuildDescriptors();

    void BuildCubeFaceCamera(float x, float y, float z);

    CD3DX12_GPU_DESCRIPTOR_HANDLE Srv() { return mhGpuSrv; }
    CD3DX12_CPU_DESCRIPTOR_HANDLE Rtv(int faceIndex) { return mhCpuRtv[faceIndex]; }
    CD3DX12_CPU_DESCRIPTOR_HANDLE Dsv() { return mhCpuDsv; }

    D3D12_VIEWPORT Viewport() const { return mViewport; }
    D3D12_RECT ScissorRect() const { return mScissorRect; }

    void UpdatePassCBs(Scene* scene);

private:
    ID3D12Device * md3dDevice = nullptr;

    D3D12_VIEWPORT mViewport;
    D3D12_RECT     mScissorRect;

    UINT        mWidth = 0;
    UINT        mHeight = 0;
    DXGI_FORMAT mFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

    CD3DX12_CPU_DESCRIPTOR_HANDLE mhCpuSrv;
    CD3DX12_GPU_DESCRIPTOR_HANDLE mhGpuSrv;
    CD3DX12_CPU_DESCRIPTOR_HANDLE mhCpuRtv[6];
    CD3DX12_CPU_DESCRIPTOR_HANDLE mhCpuDsv;

    D3DCamera mCubeFaceCamera[6];

    // todo : to not use heap
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mCbvHeap      = nullptr;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvHeap      = nullptr;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvHeap      = nullptr;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDsvHeap      = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource>       mCubeMap      = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource>       mDepthStencil = nullptr;

    std::vector<std::unique_ptr<UploadBuffer>> mPassCBs;
};
}