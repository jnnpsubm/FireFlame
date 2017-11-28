#include "FLD3DPSOManager.h"
#include "..\FLD3DUtils.h"
#include "..\Engine\FLEngine.h"
#include "..\Renderer\FLD3DRenderer.h"

namespace FireFlame {
bool D3DPSOManager::AddPSO
(
    const std::string& shaderName, 
    const std::string& shaderMacros,
    D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc
)
{
    auto renderer = Engine::GetEngine()->GetRenderer();
    auto msaaVec = renderer->GetMSAASupported();

    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
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
                    AddOpaquePSO(renderer.get(), shaderName, shaderMacros, i, psoDesc);
                    AddTransparentPSO(renderer.get(), shaderName, shaderMacros, i, psoDesc);
                }
            }
        }
    }
    return true;
}

void D3DPSOManager::AddOpaquePSO
(
    D3DRenderer* renderer,
    const std::string& shaderName, 
    const std::string& shaderMacros,
    UINT MSAAMode, 
    D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc
)
{
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    PSO_TRAIT PSOTrait
    (
        shaderName,
        shaderMacros,
        MSAAMode,
        true,
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
#ifdef _DEBUG
    //std::cout << "Add PSO : " << 
#endif
    mPSOs.emplace(PSOTrait, pso);
}

void D3DPSOManager::AddTransparentPSO
(
    D3DRenderer* renderer, 
    const std::string& shaderName, 
    const std::string& shaderMacros,
    UINT MSAAMode, 
    D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc
)
{
    psoDesc.BlendState.AlphaToCoverageEnable = TRUE;
    psoDesc.BlendState.IndependentBlendEnable = FALSE;
    psoDesc.BlendState.RenderTarget[0].BlendEnable = TRUE;
    psoDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
    psoDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
    psoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
    psoDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
    psoDesc.BlendState.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
    psoDesc.BlendState.RenderTarget[0].LogicOpEnable = FALSE;
    psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
    psoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
    psoDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
    PSO_TRAIT PSOTrait
    (
        shaderName,
        shaderMacros,
        MSAAMode,
        false,
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