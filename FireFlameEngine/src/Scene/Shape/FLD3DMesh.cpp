#include "FLD3DMesh.h"
#include "..\..\FLD3DUtils.h"

namespace FireFlame {
D3DMesh::D3DMesh() = default;

D3DMesh::D3DMesh(const std::string& name,
				 unsigned int vertexSize,
				 unsigned int vertexCount,
				 unsigned int Vertex_Format,
				 const void* vertices,
				 Index_Format indexFormat,
				 unsigned int indexCount,
				 const void* indices) : mName(name)
{
	const UINT vbByteSize = vertexCount * vertexSize;
	const UINT ibByteSize = indexCount * IndexFormatByteLength(indexFormat);

	ThrowIfFailed(D3DCreateBlob(vbByteSize, mVertexBufferCPU.ReleaseAndGetAddressOf()));
	CopyMemory(mVertexBufferCPU->GetBufferPointer(), vertices, vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, mIndexBufferCPU.ReleaseAndGetAddressOf()));
	CopyMemory(mIndexBufferCPU->GetBufferPointer(), indices, ibByteSize);

	/*mVertexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(),
	mCommandList.Get(), vertices.data(), vbByteSize, mBoxGeo->VertexBufferUploader);

	mBoxGeo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(),
	mCommandList.Get(), indices.data(), ibByteSize, mBoxGeo->IndexBufferUploader);*/

	mVertexByteStride = vertexSize;
	mVertexBufferByteSize = vbByteSize;
	mIndexFormat = DXGI_FORMAT_R16_UINT;
	mIndexBufferByteSize = ibByteSize;

	/*D3DSubMesh submesh;
	submesh.indexCount = (UINT)indices.size();
	submesh.startIndexLocation = 0;
	submesh.baseVertexLocation = 0;

	mDrawArgs["box"] = submesh;*/
}
}