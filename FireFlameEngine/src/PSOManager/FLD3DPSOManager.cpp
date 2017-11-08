#include "FLD3DPSOManager.h"
#include "..\FLD3DUtils.h"
#include "..\Engine\FLEngine.h"
#include "..\Renderer\FLD3DRenderer.h"

namespace FireFlame {
bool D3DPSOManager::AddPSO
(
    UINT          MSAAMode,
    const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc
)
{
    PSO_TRAIT PSOTrait
    (
        MSAAMode,
        desc.PrimitiveTopologyType,
        desc.RasterizerState.CullMode,
        desc.RasterizerState.FillMode
    );
    PSO_ComPtr pso = nullptr;
    ThrowIfFailed
    (
        Engine::GetEngine()->GetRenderer()->GetDevice()->CreateGraphicsPipelineState
        (
            &desc,
            IID_PPV_ARGS(pso.GetAddressOf())
        )
    );
    mPSOs.emplace(PSOTrait, pso);
    return true;
}
}