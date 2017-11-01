#include "FLD3DMesh.h"
#include "..\..\FLD3DUtils.h"

namespace FireFlame {
D3DMesh::D3DMesh() = default;

D3DMesh::D3DMesh(const stRawMesh& rawMesh) : mName(rawMesh.name){
	const UINT vbByteSize = rawMesh.vertexCount * rawMesh.vertexSize;
	const UINT ibByteSize = rawMesh.indexCount * IndexFormatByteLength(rawMesh.indexFormat);

	ThrowIfFailed(D3DCreateBlob(vbByteSize, mVertexBufferCPU.ReleaseAndGetAddressOf()));
	CopyMemory(mVertexBufferCPU->GetBufferPointer(), rawMesh.vertices, vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, mIndexBufferCPU.ReleaseAndGetAddressOf()));
	CopyMemory(mIndexBufferCPU->GetBufferPointer(), rawMesh.indices, ibByteSize);

	/*mVertexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(),
	mCommandList.Get(), vertices.data(), vbByteSize, mBoxGeo->VertexBufferUploader);

	mBoxGeo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(),
	mCommandList.Get(), indices.data(), ibByteSize, mBoxGeo->IndexBufferUploader);*/

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