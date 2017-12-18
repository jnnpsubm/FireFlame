#include "CSTaskApp.h"
#include <fstream>

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
    shaderDesc.AddShaderStage(L"Shaders\\VectorLen.hlsl", Shader_Type::CS, "main", "cs_5_0");

    mEngine.GetScene()->AddComputeShader(shaderDesc);

    /*ComputeShaderDescription shaderDesc3("VectorLen3");
    shaderDesc3.AddRootParameter
    (
        "input",
        Root_Parameter_Mode::In, 0, 1,
        DESCRIPTOR_RANGE_TYPE::UAV, 0, 0,
        ROOT_PARAMETER_TYPE::UAV
    );
    shaderDesc3.AddRootParameter
    (
        "output",
        Root_Parameter_Mode::Out, 0, 1,
        DESCRIPTOR_RANGE_TYPE::UAV, 1, 0,
        ROOT_PARAMETER_TYPE::UAV
    );
    shaderDesc3.AddShaderStage(L"Shaders\\VectorLen3.hlsl", Shader_Type::CS, "main", "cs_5_1");

    mEngine.GetScene()->AddComputeShader(shaderDesc3);*/
}

void CSTaskApp::AddPSOs()
{
    using namespace FireFlame;

    ComputePSODesc desc("VectorLen");
    mEngine.GetScene()->AddComputePSO("default", desc);

    /*ComputePSODesc desc3("VectorLen3");
    mEngine.GetScene()->AddComputePSO("default3", desc3);*/
}

void CSTaskApp::OnKeyUp(WPARAM wParam, LPARAM lParam)
{
    using namespace FireFlame;

    if (wParam == 'A')
    {
        using namespace std::placeholders;
        for (size_t i = 0; i < 1; i++)
        {
            const size_t vecNum = 64;
            std::vector<FireFlame::Vector3f> vertices;
            vertices.resize(vecNum);
            for (auto& v : vertices)
            {
                float len = MathHelper::RandF(1.0f, 10.f);
                spdlog::get("console")->info("length:{0:f}", len);

                v = { MathHelper::RandF(),MathHelper::RandF(), MathHelper::RandF() };
                v.Normalize();
                v *= len;
            }
            std::string taskName("ComputeVectorLen_");
            taskName += std::to_string(i);
            mEngine.GetScene()->SetCSRootParamData
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
            );

            CSTaskDesc taskDesc
            (
                taskName,
                "VectorLen",
                "default",
                (unsigned)std::ceilf((float)vecNum / 64.f), 1, 1,
                std::bind(&CSTaskApp::TaskDone, this, _1, _2, _3)
            );
            mEngine.GetScene()->AddCSTask(taskDesc);
        }
    }
    else if (wParam == 'B')
    {
        /*using namespace std::placeholders;
        for (size_t i = 0; i < 1; i++)
        {
            const size_t vecNum = 64;
            std::vector<FireFlame::Vector3f> vertices;
            vertices.resize(vecNum);
            for (auto& v : vertices)
            {
                float len = MathHelper::RandF(1.0f, 10.f);
                spdlog::get("console")->info("length:{0:f}", len);

                v = { MathHelper::RandF(),MathHelper::RandF(), MathHelper::RandF() };
                v.Normalize();
                v *= len;
            }
            std::string taskName("ComputeVectorLen3_");
            taskName += std::to_string(i);
            mEngine.GetScene()->SetCSRootParamData
            (
                taskName, "VectorLen3", "input",
                { FireFlame::Resource_Dimension::BUFFER },
                vertices.size() * sizeof(FireFlame::Vector3f),
                reinterpret_cast<std::uint8_t*>(vertices.data())
            );
            mEngine.GetScene()->SetCSRootParamData
            (
                taskName, "VectorLen3", "output",
                { FireFlame::Resource_Dimension::BUFFER },
                vertices.size() * sizeof(float),
                nullptr
            );

            CSTaskDesc taskDesc
            (
                taskName,
                "VectorLen3",
                "default3",
                (unsigned)std::ceilf((float)vecNum / 64.f), 1, 1,
                std::bind(&CSTaskApp::TaskDone, this, _1, _2, _3)
            );
            mEngine.GetScene()->AddCSTask(taskDesc);
        }*/
    }
}

void CSTaskApp::TaskDone(const std::string& taskName, void* data, unsigned size)
{
    using namespace std::placeholders;
    std::vector<float> results(size / sizeof(float));
    memcpy(results.data(), data, size);
    mSaveThreads[taskName] = std::async(std::launch::async, [=]() {return Save2File(taskName, results.data(), (unsigned)results.size()); });
    /*for (auto v : results)
    {
        spdlog::get("console")->info("length:{0:f}", v);
    }*/
}

int CSTaskApp::Save2File(const std::string& taskName, const float* data, unsigned size)
{
    std::ofstream out(taskName + ".txt");
    for (unsigned i = 0; i < size; i++)
    {
        out << data[i] << '\n';
    }
    return 0;
}