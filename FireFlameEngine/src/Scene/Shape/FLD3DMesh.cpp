#include "FLD3DMesh.h"
#include "..\..\FLD3DUtils.h"

namespace FireFlame {
D3DMesh::D3DMesh() = default;

void D3DMesh::MakeResident2GPU(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList) {
    if (!mVertexBufferGPU) {
        mVertexBufferGPU = D3DUtils::CreateDefaultBuffer(device, cmdList,
            mVertexBufferCPU->GetBufferPointer(), mVertexBufferByteSize, mVertexBufferUploader);
    }
    if (!mIndexBufferGPU) {
        mIndexBufferGPU = D3DUtils::CreateDefaultBuffer(device, cmdList,
            mIndexBufferCPU->GetBufferPointer(), mIndexBufferByteSize, mIndexBufferUploader);
    }
    ID3D12Pageable* resource[2] = { mVertexBufferGPU.Get() , mIndexBufferGPU.Get() };
    device->MakeResident(2, resource);
    mResideInGPU = true;
}
void D3DMesh::EvictFromGPU(ID3D12Device* device) {
    if (mResideInGPU) {
        ID3D12Pageable* resource[2] = { mVertexBufferGPU.Get() , mIndexBufferGPU.Get() };
        device->Evict(2, resource);
    }
}

D3DMesh::D3DMesh(const stRawMesh& rawMesh) : mName(rawMesh.name){
	const UINT vbByteSize = rawMesh.vertexCount * rawMesh.vertexSize;
	const UINT ibByteSize = rawMesh.indexCount * IndexFormatByteLength(rawMesh.indexFormat);

	ThrowIfFailed(D3DCreateBlob(vbByteSize, mVertexBufferCPU.ReleaseAndGetAddressOf()));
	CopyMemory(mVertexBufferCPU->GetBufferPointer(), rawMesh.vertices, vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, mIndexBufferCPU.ReleaseAndGetAddressOf()));
	CopyMemory(mIndexBufferCPU->GetBufferPointer(), rawMesh.indices, ibByteSize);

	mVertexByteStride = rawMesh.vertexSize;
	mVertexBufferByteSize = vbByteSize;
	mIndexFormat = FLIndexFormat2DXGIFormat(rawMesh.indexFormat);
	mIndexBufferByteSize = ibByteSize;
}
void D3DMesh::AddSubMesh(const stRawMesh::stSubMesh& subMesh) {
	D3DSubMesh submesh;
	submesh.indexCount = subMesh.indexCount;
	submesh.startIndexLocation = subMesh.startIndexLocation;
	submesh.baseVertexLocation = subMesh.baseVertexLocation;
	mDrawArgs[subMesh.name] = submesh;
}
}