#include "FLD3DPSOManager.h"
#include "..\FLD3DUtils.h"
#include "..\Engine\FLEngine.h"
#include "..\Renderer\FLD3DRenderer.h"

namespace FireFlame {
bool D3DPSOManager::AddPSO(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc){
    auto renderer = Engine::GetEngine()->GetRenderer();
    auto msaaVec = renderer->GetMSAASupported();

    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.NumRenderTargets = 1;

    for (UINT i = 0; i < msaaVec.size(); ++i) {
        const auto& msaaDesc = msaaVec[i];
        psoDesc.SampleDesc.Count = msaaDesc.sampleCount;
        psoDesc.SampleDesc.Quality = msaaDesc.qualityLevels - 1;
        // todo:no patch now, because no tes
        for (UINT ptype = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
            ptype < D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH; ++ptype)
        {
            psoDesc.PrimitiveTopologyType = (D3D12_PRIMITIVE_TOPOLOGY_TYPE)ptype;
            for (UINT cull = D3D12_CULL_MODE_NONE;
                cull <= D3D12_CULL_MODE_BACK; ++cull)
            {
                psoDesc.RasterizerState.CullMode = (D3D12_CULL_MODE)cull;
                for (UINT fill = D3D12_FILL_MODE_WIREFRAME;
                    fill <= D3D12_FILL_MODE_SOLID; ++fill)
                {
                    psoDesc.RasterizerState.FillMode = (D3D12_FILL_MODE)fill;
                    PSO_TRAIT PSOTrait
                    (
                        i,
                        psoDesc.PrimitiveTopologyType,
                        psoDesc.RasterizerState.CullMode,
                        psoDesc.RasterizerState.FillMode
                    );
                    PSO_ComPtr pso = nullptr;
                    ThrowIfFailed
                    (
                        renderer->GetDevice()->CreateGraphicsPipelineState
                        (
                            &psoDesc,
                            IID_PPV_ARGS(pso.GetAddressOf())
                        )
                    );
                    mPSOs.emplace(PSOTrait, pso);
                }
            }
        }
    }
    return true;
}
}