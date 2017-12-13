#include "CSTaskApp.h"

void CSTaskApp::PreInitialize()
{
    mEngine.SetDefaultClearColor({ 1.f,1.f,1.f,1.f });
}

void CSTaskApp::Initialize()
{
    AddShaders();
    AddPSOs();

    mPasses.push_back("DefaultPass");
    mEngine.GetScene()->AddPass(mPasses[0]);
}

void CSTaskApp::Update(float time_elapsed)
{
    
}

void CSTaskApp::UpdateMainPassCB(float time_elapsed)
{
    
}

void CSTaskApp::AddShaders()
{
    AddShaderVectorLen();
}

void CSTaskApp::AddShaderVectorLen()
{
    using namespace FireFlame;

    auto& shaderDesc = mShaderDescs["main"];
    shaderDesc.name = "main";

    shaderDesc.addDefaultSamplers = true;
    shaderDesc.AddRootParameter
    (
        "textures", 1, 20, DESCRIPTOR_RANGE_TYPE::SRV, 0, 0, 
        ROOT_PARAMETER_TYPE::DESCRIPTOR_TABLE, 1
    );
    shaderDesc.useRootParamDescription = true;
    shaderDesc.AddShaderStage(L"Shaders\\VectorLen.hlsl", Shader_Type::CS, "main", "cs_5_1");

    mEngine.GetScene()->AddShader(shaderDesc);
}

void CSTaskApp::AddPSOs()
{
    using namespace FireFlame;

    PSODesc descDepthComplexity(mShaderDescs["DepthComplexity"].name);
    descDepthComplexity.stencilEnable = true;
    descDepthComplexity.stencilFunc = COMPARISON_FUNC::EQUAL;
    mEngine.GetScene()->AddPSO("depth_complexity_default", descDepthComplexity);
}

void CSTaskApp::OnKeyUp(WPARAM wParam, LPARAM lParam)
{
    
}