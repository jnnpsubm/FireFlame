#include "FLD3DMesh.h"
#include "..\..\FLD3DUtils.h"
#include "..\..\Engine\FLEngine.h"
#include "..\..\Renderer\FLD3DRenderer.h"
#include <assert.h>

namespace FireFlame {
D3DMesh::D3DMesh() = default;

void D3DMesh::MakeResident2GPU(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList) {
    if (!mVertexBufferGPU.size() && !mVertexBufferInFrameRes) {
        mVertexBufferGPU.resize(mVertexBufferCPU.size(), nullptr);
        for (size_t i = 0; i < mVertexBufferCPU.size(); i++)
        {
            mVertexBufferGPU[i] = D3DUtils::CreateDefaultBuffer(device, cmdList,
                mVertexBufferCPU[i]->GetBufferPointer(), mVertexBufferByteSize[i], mVertexBufferUploader[i]);
        }
    }
    if (!mIndexBufferGPU) {
        mIndexBufferGPU = D3DUtils::CreateDefaultBuffer(device, cmdList,
            mIndexBufferCPU->GetBufferPointer(), mIndexBufferByteSize, mIndexBufferUploader);
    }
    std::vector<ID3D12Pageable*> vecRes;
    GetPageableResources(vecRes);
    ThrowIfFailed(device->MakeResident((UINT)vecRes.size(), &vecRes[0]));
    mResideInGPU = true;
}
void D3DMesh::EvictFromGPU(ID3D12Device* device) {
    if (mResideInGPU) {
        std::vector<ID3D12Pageable*> vecRes;
        GetPageableResources(vecRes);
        if (!vecRes.empty())
            ThrowIfFailed(device->Evict((UINT)vecRes.size(), &vecRes[0]));
    }
}

D3DMesh::D3DMesh(const stRawMesh& rawMesh) : mName(rawMesh.name){
    const UINT ibByteSize = rawMesh.indexCount * IndexFormatByteLength(rawMesh.indexFormat);
    ThrowIfFailed(D3DCreateBlob(ibByteSize, mIndexBufferCPU.ReleaseAndGetAddressOf()));
    CopyMemory(mIndexBufferCPU->GetBufferPointer(), rawMesh.indices, ibByteSize);
    mIndexFormat = FLIndexFormat2DXGIFormat(rawMesh.indexFormat);
    mIndexBufferByteSize = ibByteSize;

    if (rawMesh.vertexInFrameRes)
    {
        mVertexBufferInFrameRes = true;
        mVertexBufferGPU.emplace_back(nullptr);
        mVertexByteStride.resize(1, 0);
        mVertexBufferByteSize.resize(1, 0);
        const UINT vbByteSize = rawMesh.vertexDataCount[0] * rawMesh.vertexDataSize[0];
        mVertexByteStride[0] = rawMesh.vertexDataSize[0];
        mVertexBufferByteSize[0] = vbByteSize;

        auto renderer = Engine::GetEngine()->GetRenderer();
        auto device = renderer->GetDevice();
        auto frameResources = renderer->GetFrameResources();
        for (const auto& frameRes : frameResources) {
            //if (objConstSize) frameRes->ObjectCB->Init(device, maxObjConstCount, objConstSize);
            //if (passConstSize) frameRes->PassCB->Init(device, maxPassConstCount, passConstSize);
            auto VBBuffer = std::make_unique<UploadBuffer>(false);
            // todo : only support one vertex buffer for now...
            if (rawMesh.vertexDataCount.size() > 1)
                throw std::exception("todo : only support one vertex buffer in frame resource for now...");
            VBBuffer->Init(device, rawMesh.vertexDataCount[0], rawMesh.vertexDataSize[0]);
            frameRes->VBResources.emplace(rawMesh.name, std::move(VBBuffer));
        }
    }
    else
    {
#ifdef _DEBUG
        assert
        (
            (rawMesh.vertexData.size() == rawMesh.vertexDataSize.size()) &&
            (rawMesh.vertexDataSize.size() == rawMesh.vertexDataCount.size())
        );
#endif
        size_t inputSlot = rawMesh.vertexData.size();
        mVertexBufferCPU.resize(inputSlot, nullptr);
        mVertexByteStride.resize(inputSlot, 0);
        mVertexBufferByteSize.resize(inputSlot, 0);
        mVertexBufferUploader.resize(inputSlot, nullptr);

        for (size_t i = 0; i < inputSlot; ++i) {
            const UINT vbByteSize = rawMesh.vertexDataCount[i] * rawMesh.vertexDataSize[i];
            ThrowIfFailed(D3DCreateBlob(vbByteSize, mVertexBufferCPU[i].ReleaseAndGetAddressOf()));
            CopyMemory(mVertexBufferCPU[i]->GetBufferPointer(), rawMesh.vertexData[i], vbByteSize);
            mVertexByteStride[i] = rawMesh.vertexDataSize[i];
            mVertexBufferByteSize[i] = vbByteSize;
        }
    }

    for (const auto& submesh : rawMesh.subMeshs) {
        AddSubMesh(submesh);
    }

    mPrimitiveTopology = FLPrimitiveTop2D3DPrimitiveTop(rawMesh.primitiveTopology);
}
void D3DMesh::AddSubMesh(const stRawMesh::stSubMesh& subMesh) {
	D3DSubMesh submesh;
	submesh.indexCount = subMesh.indexCount;
	submesh.startIndexLocation = subMesh.startIndexLocation;
	submesh.baseVertexLocation = subMesh.baseVertexLocation;
	mDrawArgs[subMesh.name] = submesh;
}
}