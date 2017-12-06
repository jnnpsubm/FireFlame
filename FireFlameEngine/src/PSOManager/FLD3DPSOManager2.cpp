#include "FLD3DPSOManager2.h"
#include "..\src\3rd_utils\spdlog\spdlog.h"
#include "..\FLD3DUtils.h"
#include "..\Engine\FLEngine.h"
#include "..\Renderer\FLD3DRenderer.h"
#include "..\ShaderWrapper\FLD3DShaderWrapper.h"

namespace FireFlame {
void D3DPSOManager2::AddPSO(const std::string& name, const PSODesc& desc)
{
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
    ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

    auto shader = Engine::GetEngine()->GetScene()->GetShader(desc.shaderName);
    if (!shader)
    {
        spdlog::get("console")->critical("cannot find shader {0} in AddPSO", desc.shaderName.c_str());
        return;
    }
      
    auto inputLayout = shader->GetInputLayout();
    psoDesc.InputLayout = { (D3D12_INPUT_ELEMENT_DESC*)inputLayout.second, (UINT)inputLayout.first };
    psoDesc.pRootSignature = shader->GetRootSignature();

    auto vs = shader->GetVS(desc.shaderMacroVS);
    if (!vs.first)
    {
        spdlog::get("console")->critical
        (
            "cannot find vs with macro {0} in shader {1} in AddPSO",
            desc.shaderMacroVS.c_str(),
            desc.shaderName.c_str()
        );
        return;
    }
    psoDesc.VS = {
        reinterpret_cast<BYTE*>(vs.first),
        vs.second
    };

    auto ps = shader->GetPS(desc.shaderMacroPS);
    if (!ps.first)
    {
        spdlog::get("console")->critical
        (
            "cannot find ps with macro {0} in shader {1} in AddPSO",
            desc.shaderMacroPS.c_str(),
            desc.shaderName.c_str()
        );
        return;
    }
    psoDesc.PS = {
        reinterpret_cast<BYTE*>(ps.first),
        ps.second
    };
    
    auto renderer = Engine::GetEngine()->GetRenderer();
    psoDesc.RTVFormats[0] = renderer->GetBackBufferFormat();
    psoDesc.DSVFormat = renderer->GetDepthStencilFormat();

    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.NumRenderTargets = 1;

    auto msaaVec = renderer->GetMSAASupported();
    for (UINT i = 0; i < msaaVec.size(); ++i) {
        const auto& msaaDesc = msaaVec[i];
        psoDesc.SampleDesc.Count = msaaDesc.sampleCount;
        psoDesc.SampleDesc.Quality = msaaDesc.qualityLevels - 1;
        
        psoDesc.PrimitiveTopologyType = D3DPrimitiveType(FLPrimitiveTop2D3DPrimitiveTop(desc.topology));
        psoDesc.RasterizerState.CullMode = FLCullMode2D3DCullMode(desc.cullMode);
        for (UINT fill = D3D12_FILL_MODE_WIREFRAME;
            fill <= D3D12_FILL_MODE_SOLID; ++fill)
        {
            psoDesc.RasterizerState.FillMode = (D3D12_FILL_MODE)fill;
            psoDesc.RasterizerState.FrontCounterClockwise = desc.frontCounterClockwise;

            psoDesc.BlendState.AlphaToCoverageEnable = desc.alpha2Coverage;
            psoDesc.BlendState.IndependentBlendEnable = FALSE;
            psoDesc.BlendState.RenderTarget[0].BlendEnable = !desc.opaque;
            psoDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
            psoDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
            psoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
            psoDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
            psoDesc.BlendState.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
            psoDesc.BlendState.RenderTarget[0].LogicOpEnable = FALSE;
            psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = FLColorWriteMask2D3DColorWriteMask(desc.colorWriteEnable[0]);
            psoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
            psoDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;

            psoDesc.DepthStencilState.DepthEnable = desc.depthEnable;
            psoDesc.DepthStencilState.DepthWriteMask = FLDepthWriteMask2D3DDepthWriteMask(desc.depthWriteMask);
            psoDesc.DepthStencilState.StencilEnable = desc.stencilEnable;
            psoDesc.DepthStencilState.StencilReadMask = desc.stencilReadMask;
            psoDesc.DepthStencilState.StencilWriteMask = desc.stencilWriteMask;

            psoDesc.DepthStencilState.FrontFace.StencilFailOp = FLStencilOp2D3DStencilOp(desc.stencilFailOp);
            psoDesc.DepthStencilState.FrontFace.StencilDepthFailOp = FLStencilOp2D3DStencilOp(desc.stencilDepthFailOp);
            psoDesc.DepthStencilState.FrontFace.StencilPassOp = FLStencilOp2D3DStencilOp(desc.stencilPassOp);
            psoDesc.DepthStencilState.FrontFace.StencilFunc = FLCompareFunc2D3DCompareFunc(desc.stencilFunc);

            psoDesc.DepthStencilState.BackFace.StencilFailOp = FLStencilOp2D3DStencilOp(desc.stencilFailOp);
            psoDesc.DepthStencilState.BackFace.StencilDepthFailOp = FLStencilOp2D3DStencilOp(desc.stencilDepthFailOp);
            psoDesc.DepthStencilState.BackFace.StencilPassOp = FLStencilOp2D3DStencilOp(desc.stencilPassOp);
            psoDesc.DepthStencilState.BackFace.StencilFunc = FLCompareFunc2D3DCompareFunc(desc.stencilFunc);

            PSO_TRAIT PSOTrait(name, i, psoDesc.RasterizerState.FillMode);
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
            mNames.insert(name);
        }
    }

    /*spdlog::get("console")->info
    (
        "PSO Created, shader:{0}, VSMacro:{1}, PSMacro:{2}, topology:{3:d}, cull:{4:d}, opaque:{5}",
        desc.shaderName,
        desc.shaderMacroVS,
        desc.shaderMacroPS,
        (int)desc.topology,
        (int)desc.cullMode,
        desc.opaque
    );*/
    spdlog::get("console")->info
    (
        "PSO Created, Total Count Now:{0:d}",
        mPSOs.size()
    );
}
}