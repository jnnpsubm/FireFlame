#include "FLD3DPrimitive.h"
#include "..\..\Renderer\FLD3DRenderer.h"
#include "..\..\ShaderWrapper\FLD3DShaderWrapper.h"
#include "..\..\Engine\FLEngine.h"

namespace FireFlame {
D3DPrimitive::D3DPrimitive() = default;
D3DPrimitive::D3DPrimitive(const stRawMesh& mesh) : mMesh(std::make_unique<D3DMesh>(mesh)){}
}