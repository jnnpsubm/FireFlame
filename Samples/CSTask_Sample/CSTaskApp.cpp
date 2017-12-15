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
        "input",
        Root_Parameter_Mode::In, 0, 1,
        DESCRIPTOR_RANGE_TYPE::SRV, 0, 0, 
        ROOT_PARAMETER_TYPE::SRV
    );
    shaderDesc.AddRootParameter
    (
        "output", 
        Root_Parameter_Mode::Out, 0, 1,
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
    using namespace FireFlame;

    if (wParam == 'A')
    {
        for (size_t i = 0; i < 1024; i++)
        {
            const size_t vecNum = 12800;
            std::vector<FireFlame::Vector3f> vertices;
            vertices.resize(vecNum);
            for (auto& v : vertices)
            {
                float len = MathHelper::RandF(1.0f, 10.f);
                v = { MathHelper::RandF(),MathHelper::RandF(), MathHelper::RandF() };
                v *= len;
            }
            std::string taskName("ComputeVectorLen");
            taskName += std::to_string(i);
            /*mEngine.GetScene()->SetCSRootParamData
            (
                taskName, "VectorLen", "input",
                { FireFlame::Resource_Dimension::BUFFER },
                vertices.size() * sizeof(FireFlame::Vector3f),
                reinterpret_cast<std::uint8_t*>(vertices.data())
            );
            mEngine.GetScene()->SetCSRootParamData
            (
                taskName, "VectorLen", "output",
                { FireFlame::Resource_Dimension::BUFFER },
                vertices.size() * sizeof(float),
                nullptr
            );*/

            CSTaskDesc<std::function<void(const std::string&, void*)>> taskDesc
            (
                taskName,
                "VectorLen",
                "default",
                (unsigned)std::ceilf((float)vecNum / 64.f), 1, 1,
                std::bind(&CSTaskApp::TaskDone, this, std::placeholders::_1, std::placeholders::_2)
            );
            mEngine.GetScene()->AddCSTask(taskDesc);
        }
    }
}

void CSTaskApp::TaskDone(const std::string& taskName, void* data)
{

}