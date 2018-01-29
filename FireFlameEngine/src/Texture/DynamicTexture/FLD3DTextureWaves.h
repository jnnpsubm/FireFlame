#pragma once
#include "..\FLD3DTexture.h"
#include "..\..\3rd_utils\d3dx12.h"

namespace FireFlame {
class D3DTextureWaves;
class D3DGpuWaves
{
    friend D3DTextureWaves;
public:
    // Note that m,n should be divisible by 16 so there is no 
    // remainder when we divide into thread groups.
    D3DGpuWaves
    (
        ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, 
        int m, int n, float dx, float dt, float speed, float damping, unsigned disturbCount
    );
    D3DGpuWaves(const D3DGpuWaves& rhs) = delete;
    D3DGpuWaves& operator=(const D3DGpuWaves& rhs) = delete;
    ~D3DGpuWaves() = default;

    UINT RowCount() const { return mNumRows; }
    UINT ColumnCount() const { return mNumCols; }
    UINT VertexCount() const { return mVertexCount; };
    UINT TriangleCount() const { return mTriangleCount; };
    float Width() const { return mNumCols*mSpatialStep; };
    float Depth() const { return mNumRows*mSpatialStep; };
    float SpatialStep() const { return mSpatialStep; };

    void Go(ID3D12GraphicsCommandList* cmdList);

private:
    void Update(ID3D12GraphicsCommandList* cmdList);
    void Disturb(ID3D12GraphicsCommandList* cmdList);

    void BuildResources(ID3D12GraphicsCommandList* cmdList);
    void BuildDescriptors();

    void BuildRootSignature();
    void BuildPSO();

    void RotateResources() {
        auto resTemp = mPrevSol;
        mPrevSol = mCurrSol;
        mCurrSol = mNextSol;
        mNextSol = resTemp;
        auto srvTemp = mPrevSolSrv;
        mPrevSolSrv = mCurrSolSrv;
        mCurrSolSrv = mNextSolSrv;
        mNextSolSrv = srvTemp;
        auto uavTemp = mPrevSolUav;
        mPrevSolUav = mCurrSolUav;
        mCurrSolUav = mNextSolUav;
        mNextSolUav = uavTemp;
    }

    UINT mNumRows;
    UINT mNumCols;

    UINT mVertexCount;
    UINT mTriangleCount;

    // Simulation constants we can precompute.
    float mK[3];

    float mTimeStep;
    float mSpatialStep;

    unsigned mDisturbCount = 1;
    float    mWaveLow = 0.4f;
    float    mWaveHigh = 1.2f;

    ID3D12Device* md3dDevice = nullptr;

    Microsoft::WRL::ComPtr<ID3D12RootSignature>  mRootSignature  = nullptr;
    Microsoft::WRL::ComPtr<ID3D12PipelineState>  mUpdatePSO      = nullptr;
    Microsoft::WRL::ComPtr<ID3D12PipelineState>  mDisturbPSO     = nullptr;

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDescriptorHeap = nullptr;
    CD3DX12_GPU_DESCRIPTOR_HANDLE mPrevSolSrv;
    CD3DX12_GPU_DESCRIPTOR_HANDLE mCurrSolSrv;
    CD3DX12_GPU_DESCRIPTOR_HANDLE mNextSolSrv;

    CD3DX12_GPU_DESCRIPTOR_HANDLE mPrevSolUav;
    CD3DX12_GPU_DESCRIPTOR_HANDLE mCurrSolUav;
    CD3DX12_GPU_DESCRIPTOR_HANDLE mNextSolUav;

    // Two for ping-ponging the textures.
    Microsoft::WRL::ComPtr<ID3D12Resource> mPrevSol = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource> mCurrSol = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource> mNextSol = nullptr;

    Microsoft::WRL::ComPtr<ID3D12Resource> mPrevUploadBuffer = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource> mCurrUploadBuffer = nullptr;
};

class D3DTextureWaves : public D3DTextureBase
{
public:
    D3DTextureWaves
    (
        const std::string& name,
        ID3D12Device* device, ID3D12GraphicsCommandList* cmdList,
        int m, int n, float dx, float dt, float speed, float damping, unsigned disturbCount
    ) :D3DTextureBase(name), mWaves(device, cmdList, m, n, dx, dt, speed, damping, disturbCount) {}
    ID3D12Resource* Resource() override final { return mWaves.mCurrSol.Get(); }
    void Update(ID3D12GraphicsCommandList* cmdList, Scene* scene) override final {
        mWaves.Go(cmdList);
    }
    void ClearUploadBuffer() { 
        mWaves.mCurrUploadBuffer = nullptr; 
        mWaves.mPrevUploadBuffer = nullptr; 
    }

private:
    D3DGpuWaves mWaves;
};
} // end FireFlame
