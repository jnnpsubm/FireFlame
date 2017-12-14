#include "CSTaskApp.h"

void CSTaskApp::PreInitialize()
{
    mEngine.SetDefaultClearColor({ 1.f,1.f,1.f,1.f });
}

void CSTaskApp::Initialize()
{
    AddShaders();
    AddPSOs();
}

void CSTaskApp::AddShaders()
{
    AddShaderVectorLen();
}

void CSTaskApp::AddShaderVectorLen()
{
    using namespace FireFlame;

    ComputeShaderDescription shaderDesc("VectorLen");
    shaderDesc.AddRootParameter
    (
        "input", 0, 1, 
        DESCRIPTOR_RANGE_TYPE::SRV, 0, 0, 
        ROOT_PARAMETER_TYPE::SRV
    );
    shaderDesc.AddRootParameter
    (
        "output", 0, 1,
        DESCRIPTOR_RANGE_TYPE::UAV, 0, 0,
        ROOT_PARAMETER_TYPE::UAV
    );
    shaderDesc.AddShaderStage(L"Shaders\\VectorLen.hlsl", Shader_Type::CS, "main", "cs_5_1");

    mEngine.GetScene()->AddComputeShader(shaderDesc);
}

void CSTaskApp::AddPSOs()
{
    using namespace FireFlame;

    ComputePSODesc desc("VectorLen");
    mEngine.GetScene()->AddComputePSO("default", desc);
}

void CSTaskApp::OnKeyUp(WPARAM wParam, LPARAM lParam)
{
    
}