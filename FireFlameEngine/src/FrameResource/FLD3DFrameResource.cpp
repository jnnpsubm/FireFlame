#include "FLD3DFrameResource.h"

namespace FireFlame {
D3DFrameResource::D3DFrameResource(ID3D12Device* device) {
    ThrowIfFailed
    (
        device->CreateCommandAllocator(
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            IID_PPV_ARGS(CmdListAlloc.GetAddressOf()))
    );

    PassCB = std::make_unique<UploadBuffer>(true);
    ObjectCB = std::make_unique<UploadBuffer>(true);
}
}