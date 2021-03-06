#include "FLD3DPSOManager2.h"
#include "..\src\3rd_utils\spdlog\spdlog.h"
#include "..\FLD3DUtils.h"
#include "..\Engine\FLEngine.h"
#include "..\Renderer\FLD3DRenderer.h"
#include "..\ShaderWrapper\FLD3DShaderWrapper.h"
#include "..\ShaderWrapper\FLD3DComputeShaderWrapper.h"

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

    // vs
    auto vs = shader->GetVS(desc.shaderMacroVS);
    if (!vs.first)
    {
        spdlog::get("console")->warn
        (
            "cannot find vs with macro {0} in shader {1} in AddPSO",
            desc.shaderMacroVS.c_str(),
            desc.shaderName.c_str()
        );
    }
    psoDesc.VS = {
        reinterpret_cast<BYTE*>(vs.first),
        vs.second
    };

    // hs
    auto hs = shader->GetHS(desc.shaderMacroHS);
    if (hs.first)
    {
        psoDesc.HS = {
            reinterpret_cast<BYTE*>(hs.first),
            hs.second
        };
    }
    // ds
    auto ds = shader->GetDS(desc.shaderMacroDS);
    if (ds.first)
    {
        psoDesc.DS = {
            reinterpret_cast<BYTE*>(ds.first),
            ds.second
        };
    }

    auto gs = shader->GetGS(desc.shaderMacroGS);
    if (gs.first)
    {
        psoDesc.GS = {
            reinterpret_cast<BYTE*>(gs.first),
            gs.second
        };
    }

    auto ps = shader->GetPS(desc.shaderMacroPS);
    if (!ps.first)
    {
        spdlog::get("console")->warn
        (
            "cannot find ps with macro {0} in shader {1} in AddPSO",
            desc.shaderMacroPS.c_str(),
            desc.shaderName.c_str()
        );
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
        
        psoDesc.PrimitiveTopologyType = FLPrimitiveType2D3DPrimitiveType(desc.topologyType);
        psoDesc.RasterizerState.CullMode = FLCullMode2D3DCullMode(desc.cullMode);
        for (UINT fill = D3D12_FILL_MODE_WIREFRAME;
            fill <= D3D12_FILL_MODE_SOLID; ++fill)
        {
            psoDesc.RasterizerState.FillMode = (D3D12_FILL_MODE)fill;
            psoDesc.RasterizerState.FrontCounterClockwise = desc.frontCounterClockwise;

            psoDesc.BlendState.AlphaToCoverageEnable = desc.alpha2Coverage;
            psoDesc.BlendState.IndependentBlendEnable = FALSE;
            psoDesc.BlendState.RenderTarget[0].BlendEnable = !desc.opaque;
            psoDesc.BlendState.RenderTarget[0].BlendOp = FLBlendOp2D3DBlendOp(desc.blendOp);
            psoDesc.BlendState.RenderTarget[0].SrcBlend = FLBlend2D3DBlend(desc.srcBlend);
            psoDesc.BlendState.RenderTarget[0].DestBlend = FLBlend2D3DBlend(desc.destBlend);
            psoDesc.BlendState.RenderTarget[0].BlendOpAlpha = FLBlendOp2D3DBlendOp(desc.blendOpAlpha);
            psoDesc.BlendState.RenderTarget[0].SrcBlendAlpha = FLBlend2D3DBlend(desc.srcBlendAlpha);
            psoDesc.BlendState.RenderTarget[0].DestBlendAlpha = FLBlend2D3DBlend(desc.destBlendAlpha);
            psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = FLColorWriteMask2D3DColorWriteMask(desc.colorWriteEnable[0]);
            psoDesc.BlendState.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
            psoDesc.BlendState.RenderTarget[0].LogicOpEnable = FALSE;

            psoDesc.DepthStencilState.DepthEnable = desc.depthEnable;
            psoDesc.DepthStencilState.DepthFunc = FLCompareFunc2D3DCompareFunc(desc.depthFunc);
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

void D3DPSOManager2::AddComputePSO(const std::string& name, const ComputePSODesc& desc)
{
    D3D12_COMPUTE_PIPELINE_STATE_DESC PSODesc = {};
    auto shader = Engine::GetEngine()->GetScene()->GetComputeShader(desc.shaderName);
    if (!shader)
    {
        spdlog::get("console")->critical("cannot find shader {0} in AddComputePSO", desc.shaderName);
        return;
    }
    PSODesc.pRootSignature = shader->GetRootSignature();

    auto cs = shader->GetCS(desc.shaderMacroCS);
    if (!cs.first)
    {
        spdlog::get("console")->warn
        (
            "cannot find CS with macro {0} in shader {1} in AddComputePSO",
            desc.shaderMacroCS.c_str(),
            desc.shaderName.c_str()
        );
    }
    PSODesc.CS = {
        reinterpret_cast<BYTE*>(cs.first),
        cs.second
    };

    PSODesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
    //PSODesc.Flags = D3D12_PIPELINE_STATE_FLAG_TOOL_DEBUG;

    auto device = Engine::GetEngine()->GetRenderer()->GetDevice();
    PSO_ComPtr PSOadd = nullptr;
    ThrowIfFailed
    (
        device->CreateComputePipelineState(&PSODesc, IID_PPV_ARGS(PSOadd.GetAddressOf()))
    );
    mComputePSOs[name] = PSOadd;
}

void D3DPSOManager2::PrintAllPSOs()
{
    std::cout << "PSO Count:" << mPSOs.size() << std::endl;
    for (const auto& itPSO : mPSOs)
    {
        std::cout << "   " 
            << std::get<0>(itPSO.first) 
            << "_MSAA" << std::get<1>(itPSO.first) 
            << "_FILL" << std::get<2>(itPSO.first)
            << ":0x" << std::hex << itPSO.second.Get() << std::endl;
    }

    std::cout << "Compute PSO Count:" << mComputePSOs.size() << std::endl;
    for (const auto& itPSO : mComputePSOs)
    {
        std::cout << "   "
            << itPSO.first
            << ":0x" << std::hex << itPSO.second.Get() << std::endl;
    }
}
}