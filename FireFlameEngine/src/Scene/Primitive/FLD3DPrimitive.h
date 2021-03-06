#pragma once
#include "../Shape/FLD3DMesh.h"

namespace FireFlame {
class D3DRenderer;
class D3DShaderWrapper;
class D3DPrimitive {
public:
	D3DPrimitive();
	explicit D3DPrimitive(const stRawMesh& mesh);
	D3DMesh* GetMesh() { return mMesh.get(); }

    bool Visible() const { return mVisible; }
    void SetVisible(bool visible) const { mVisible = visible; }

    void Draw(D3DRenderer* renderer);

    void SetShader(std::shared_ptr<D3DShaderWrapper> shader) { mShader = shader; }

private:
    mutable bool                              mVisible     = true;

	std::unique_ptr<D3DMesh>          mMesh        = nullptr;
    std::shared_ptr<D3DShaderWrapper> mShader      = nullptr;
};
}