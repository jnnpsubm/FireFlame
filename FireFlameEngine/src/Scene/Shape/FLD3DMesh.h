#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <Windows.h>
#include <d3d12.h>
#include <d3dcompiler.h>
#include <DirectXCollision.h>
#include <wrl.h>
#include "..\..\header\FLMatrix.h"
#include "..\..\FLTypeDefs.h"

namespace FireFlame {
class D3DMesh{
private:
	struct D3DSubMesh {
		UINT indexCount = 0;
		UINT startIndexLocation = 0;
		INT  baseVertexLocation = 0;
		DirectX::BoundingBox boundBox;
	};

public:
	D3DMesh();
	explicit D3DMesh(const stRawMesh& rawMesh);
	std::string mName;
	bool        mResideInGPU = false;

	void AddSubMesh(const stRawMesh::stSubMesh& subMesh);

	// System memory copies.  Use Blobs because the vertex/index format can be generic.
	// It is up to the client to cast appropriately.  
	Microsoft::WRL::ComPtr<ID3DBlob> mVertexBufferCPU = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> mIndexBufferCPU  = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> mVertexBufferGPU = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> mIndexBufferGPU  = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> mVertexBufferUploader = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> mIndexBufferUploader  = nullptr;

	// Data about the buffers.
	UINT        mVertexByteStride = 0;
	UINT        mVertexBufferByteSize = 0;
	DXGI_FORMAT mIndexFormat = DXGI_FORMAT_R16_UINT;
	UINT        mIndexBufferByteSize = 0;

	// A MeshGeometry may store multiple geometries in one vertex/index buffer.
	// Use this container to define the Submesh geometries so we can draw
	// the Submeshes individually.
	std::unordered_map<std::string, D3DSubMesh> mDrawArgs;

	D3D12_VERTEX_BUFFER_VIEW VertexBufferView() const {
		D3D12_VERTEX_BUFFER_VIEW vbv;
		vbv.BufferLocation = mVertexBufferGPU->GetGPUVirtualAddress();
		vbv.StrideInBytes = mVertexByteStride;
		vbv.SizeInBytes = mVertexBufferByteSize;
		return vbv;
	}

	D3D12_INDEX_BUFFER_VIEW IndexBufferView() const {
		D3D12_INDEX_BUFFER_VIEW ibv;
		ibv.BufferLocation = mIndexBufferGPU->GetGPUVirtualAddress();
		ibv.Format = mIndexFormat;
		ibv.SizeInBytes = mIndexBufferByteSize;
		return ibv;
	}

	// We can free this memory after we finish upload to the GPU.
	void DisposeUploaders(){
		mVertexBufferUploader = nullptr;
		mIndexBufferUploader = nullptr;
	}
};
}