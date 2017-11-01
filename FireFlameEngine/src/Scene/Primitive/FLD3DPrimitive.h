#pragma once
#include "../Shape/FLD3DMesh.h"

namespace FireFlame {
class D3DShaderWrapper;
class D3DPrimitive {
public:
	D3DPrimitive();
	explicit D3DPrimitive(const stRawMesh& mesh);
	std::unique_ptr<D3DMesh>& GetMesh() { return mMesh; }

private:
	std::unique_ptr<D3DMesh>          mMesh;
    std::shared_ptr<D3DShaderWrapper> mShader;
	DirectX::XMFLOAT4X4               mLocal2World;
};
}