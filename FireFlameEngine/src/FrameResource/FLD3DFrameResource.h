#pragma once
#include "FLFrameResource.h"
#include "..\GPUMemory\D3DUploadBuffer.h"
#include <memory>

namespace FireFlame {
class D3DFrameResource : public FrameResource {
public:
    D3DFrameResource(ID3D12Device* device);
    D3DFrameResource(const D3DFrameResource& rhs) = delete;
    D3DFrameResource& operator=(const D3DFrameResource& rhs) = delete;
    ~D3DFrameResource() = default;

    // We cannot reset the allocator until the GPU is done processing the commands.
    // So each frame needs their own allocator.
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CmdListAlloc;

    // We cannot update a cbuffer until the GPU is done processing the commands
    // that reference it.  So each frame needs their own cbuffers.
    std::unique_ptr<UploadBuffer> PassCB = nullptr;
    std::unique_ptr<UploadBuffer> ObjectCB = nullptr;

    // Fence value to mark commands up to this fence point.  This lets us
    // check if these frame resources are still in use by the GPU.
    UINT64 Fence = 0;
};
}
