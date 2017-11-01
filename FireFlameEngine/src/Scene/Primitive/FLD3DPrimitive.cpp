#include "FLD3DPrimitive.h"

namespace FireFlame {
D3DPrimitive::D3DPrimitive() = default;
D3DPrimitive::D3DPrimitive(const stRawMesh& mesh) : mMesh(std::make_unique<D3DMesh>(mesh)){
	mLocal2World = mesh.LocalToWorld;
}
}