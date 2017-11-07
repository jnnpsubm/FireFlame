#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <Windows.h>
#include <d3d12.h>
#include <d3dcompiler.h>
#include <DirectXCollision.h>
#include <wrl.h>
#include "..\..\Matrix\FLMatrix4X4.h"
#include "..\..\FLTypeDefs.h"
#include "..\..\FLD3DUtils.h"

namespace FireFlame {
class D3DMesh{
private:
    typedef Microsoft::WRL::ComPtr<ID3DBlob> ComPtrBlob;
    typedef Microsoft::WRL::ComPtr<ID3D12Resource> ComPtrRes;

public:
	struct D3DSubMesh {
		UINT indexCount = 0;
		UINT startIndexLocation = 0;
		INT  baseVertexLocation = 0;
		DirectX::BoundingBox boundBox;
	};
    typedef std::unordered_map<std::string, D3DSubMesh> SubMeshMap;

public:
	D3DMesh();
	explicit D3DMesh(const stRawMesh& rawMesh);

	bool ResidentOnGPU() const { return mResideInGPU; }
	void MakeResident2GPU(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);
	void EvictFromGPU(ID3D12Device* device);

	void AddSubMesh(const stRawMesh::stSubMesh& subMesh);

    // Get methods
    UINT                          GetNumVertexBufferView()   const { return (UINT)mVertexBufferGPU.size(); }
    D3D_PRIMITIVE_TOPOLOGY        GetPrimitiveTopology()     const { return mPrimitiveTopology; }
    const SubMeshMap&             GetSubMeshs()              const { return mDrawArgs; }
    D3D12_PRIMITIVE_TOPOLOGY_TYPE GetPrimitiveTopologyType() const 
    { 
        return D3DPrimitiveType(mPrimitiveTopology);
    }

    D3D12_VERTEX_BUFFER_VIEW VertexBufferView(size_t index) const {
        D3D12_VERTEX_BUFFER_VIEW vbv;
        vbv.BufferLocation = mVertexBufferGPU[index]->GetGPUVirtualAddress();
        vbv.StrideInBytes = mVertexByteStride[index];
        vbv.SizeInBytes = mVertexBufferByteSize[index];
        return vbv;
    }
    std::vector<D3D12_VERTEX_BUFFER_VIEW> VertexBufferViews() const {
        std::vector<D3D12_VERTEX_BUFFER_VIEW> vbvs;
        vbvs.resize(mVertexBufferGPU.size());
        for (size_t i = 0; i < vbvs.size(); i++){
            vbvs[i].BufferLocation = mVertexBufferGPU[i]->GetGPUVirtualAddress();
            vbvs[i].StrideInBytes = mVertexByteStride[i];
            vbvs[i].SizeInBytes = mVertexBufferByteSize[i];
        }
        return vbvs;
    }
    D3D12_INDEX_BUFFER_VIEW IndexBufferView() const {
        D3D12_INDEX_BUFFER_VIEW ibv;
        ibv.BufferLocation = mIndexBufferGPU->GetGPUVirtualAddress();
        ibv.Format = mIndexFormat;
        ibv.SizeInBytes = mIndexBufferByteSize;
        return ibv;
    }
    // We can free this memory after we finish upload to the GPU.
    void DisposeUploaders() {
        for (auto& ptr : mVertexBufferUploader) {
            ptr = nullptr;
        }
        mIndexBufferUploader = nullptr;
    }

    void GetPageableResources(std::vector<ID3D12Pageable*>& vec) {
        vec.clear();
        size_t resNum = mVertexBufferGPU.size() + 1;
        vec.resize(resNum);
        for (size_t i = 0; i < mVertexBufferGPU.size(); i++){
            vec[i] = mVertexBufferGPU[i].Get();
        }
        vec[resNum - 1] = mIndexBufferGPU.Get();
    }

private:
    std::string mName;
    bool        mResideInGPU = false;

	// System memory copies.  Use Blobs because the vertex/index format can be generic.
	// It is up to the client to cast appropriately.  
    std::vector<ComPtrBlob>          mVertexBufferCPU;
	Microsoft::WRL::ComPtr<ID3DBlob> mIndexBufferCPU  = nullptr;

    std::vector<ComPtrRes>                 mVertexBufferGPU;
	Microsoft::WRL::ComPtr<ID3D12Resource> mIndexBufferGPU  = nullptr;

    std::vector<ComPtrRes>                 mVertexBufferUploader;
	Microsoft::WRL::ComPtr<ID3D12Resource> mIndexBufferUploader  = nullptr;

	// Data about the buffers.
    D3D12_PRIMITIVE_TOPOLOGY mPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	std::vector<UINT> mVertexByteStride;
	std::vector<UINT> mVertexBufferByteSize;
	DXGI_FORMAT       mIndexFormat = DXGI_FORMAT_R16_UINT;
	UINT              mIndexBufferByteSize = 0;

	// A MeshGeometry may store multiple geometries in one vertex/index buffer.
	// Use this container to define the Submesh geometries so we can draw
	// the Submeshes individually.
    SubMeshMap  mDrawArgs;
};
}