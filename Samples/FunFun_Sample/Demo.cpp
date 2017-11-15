#include "Demo.h"
#include "FireFlameHeader.h"
#include "SquareMesh.h"

Demo::Demo(FireFlame::Engine& engine) :mEngine(engine) {
    mShaders.emplace_back("CubicParabola",      L"Shaders\\CubicParabolaPixelShader.hlsl");
    mShaders.emplace_back("SemicubicParabola",  L"Shaders\\SemicubicParabolaPixelShader.hlsl");
    //mShaders.emplace_back("ProbabilityCurve",   L"Shaders\\ProbabilityCurvePixelShader.hlsl");
    //mShaders.emplace_back("WitchofAgnesi",      L"Shaders\\WitchofAgnesiPixelShader.hlsl");
    //mShaders.emplace_back("Cissoid",            L"Shaders\\CissoidPixelShader.hlsl");
    //mShaders.emplace_back("FoliumOfDescartes",  L"Shaders\\FoliumOfDescartesPixelShader.hlsl");
    //// todo:
    //mShaders.emplace_back("Tetracuspid",        L"Shaders\\TetracuspidPixelShader.hlsl");
    //mShaders.emplace_back("Cycloid",            L"Shaders\\CycloidPixelShader.hlsl");
    //mShaders.emplace_back("Cardioid",           L"Shaders\\CardioidPixelShader.hlsl");
    mShaders.emplace_back("SpiralOfArchimedes", L"Shaders\\SpiralOfArchimedesPixelShader.hlsl");
    //mShaders.emplace_back("LogarithmicSpira",   L"Shaders\\LogarithmicSpiralPixelShader.hlsl");
    //mShaders.emplace_back("FourLeafedRose",     L"Shaders\\FourLeafedRosePixelShader.hlsl");
}
void Demo::AddShaders() {
    using namespace FireFlame;
    stShaderDescription shader
    (
        "",
        { VERTEX_FORMAT_FLOAT3 , VERTEX_FORMAT_FLOAT2 },
        { {"POSITION"},{"TEXCOORD"} },
        sizeof(ObjectConstants), 1
    );
    shader.AddShaderStage
    (
        L"Shaders\\FFVertexShader.hlsl",
        Shader_Type::VS, "VS", "vs_5_0"
    );

    for (const auto& shaderFun : mShaders) {
        shader.name = shaderFun.shaderName;
        shader.AddShaderStage
        (
            shaderFun.shaderPSFile,
            Shader_Type::PS, "main", "ps_5_0"
        );
        mEngine.GetScene()->AddShader(shader);
    }
}
void Demo::AddGeometry() {
    using namespace FireFlame;
    SquareMesh           squareMesh;
    stRawMesh            meshDesc("SquareMesh");
    stRawMesh::stSubMesh subMesh("Square");
    squareMesh.GetMeshDesc(meshDesc);
    squareMesh.GetSubMeshDesc(subMesh);
    mEngine.GetScene()->AddPrimitive(meshDesc, mShaders[0].shaderName);
    mEngine.GetScene()->PrimitiveAddSubMesh(meshDesc.name, subMesh);

    FireFlame::stRenderItemDesc RItemDesc;
    RItemDesc.name = meshDesc.name;
    RItemDesc.subMesh = subMesh;
    RItemDesc.topology = FireFlame::Primitive_Topology::TriangleList;
    mEngine.GetScene()->AddRenderItem(meshDesc.name, mShaders[0].shaderName, RItemDesc);

    UseShader(mShaders[0].shaderName);
    UseGeometry(meshDesc.name);
}
void Demo::AddRenderItem() {
    
}
void Demo::OnGameWindowResized(int w, int h) {
    // Build the view matrix.
    DirectX::XMVECTOR pos = DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 1.0f);
    DirectX::XMVECTOR target = DirectX::XMVectorZero();
    DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(pos, target, up);
    DirectX::XMStoreFloat4x4(&mView, view);

    // The window resized, so update the aspect ratio and recompute the projection matrix.
    DirectX::XMMATRIX P = DirectX::XMMatrixPerspectiveFovLH
    (
        0.25f*DirectX::XMVectorGetX(DirectX::g_XMPi),
        (float)w / h, 1.0f, 1000.0f
    );
    DirectX::XMStoreFloat4x4(&mProj, P);
}
void Demo::Update(float time_elapsed) {
    // Update the constant buffer with the latest worldViewProj matrix.
    //DirectX::XMMATRIX ViewProj;
    //DirectX::XMStoreFloat4x4(&mShaderConstants.ViewProj, DirectX::XMMatrixTranspose(ViewProj));
    //mObjectCB->CopyData(0, objConstants);

    //mEngine.GetScene()->UpdateShaderCBData(mCurrShader, 0, mShaderConstants);
    mEngine.GetScene()->UpdateRenderItemCBData(mCurrGeo, sizeof(ObjectConstants), &mShaderConstants);
}
void Demo::OnMouseDown(WPARAM btnState, int x, int y) {
   
}
void Demo::OnMouseUp(WPARAM btnState, int x, int y) {
   
}
void Demo::OnMouseMove(WPARAM btnState, int x, int y) {
   
}

void Demo::OnKeyUp(WPARAM wParam, LPARAM lParam) {
   
}
void Demo::OnKeyDown(WPARAM wParam, LPARAM lParam) {
	std::wstring strDebug(L"================");
    if ((int)wParam == 'S')
    {
        mShaderConstants.fTexScale *= 1.5f;
		strDebug += L"TexScale = " + std::to_wstring(mShaderConstants.fTexScale);
    }
    else if ((int)wParam == 'D')
    {
        mShaderConstants.fTexScale /= 1.5f;
        strDebug += L"TexScale = " + std::to_wstring(mShaderConstants.fTexScale);
    }
    else if ((int)wParam == 'W')
    {
        mShaderConstants.fWidth *= 1.5f;
        strDebug += L"fWidth = " + std::to_wstring(mShaderConstants.fWidth);
    }
    else if ((int)wParam == 'E')
    {
        mShaderConstants.fWidth /= 1.5f;
        strDebug += L"fWidth = " + std::to_wstring(mShaderConstants.fWidth);
    }
    else if ((int)wParam == 'M')
    {
        static size_t mode = 0;
        if (++mode < mShaders.size()) {
            
        }
        else {
            mode = 0;
        }
        UseShader(mShaders[mode].shaderName);
        //mEngine.GetScene()->PrimitiveUseShader(mCurrGeo, mCurrShader);
        mEngine.GetScene()->RenderItemChangeShader(mCurrGeo, mCurrShader);
        strDebug += L"Shader:" + FireFlame::AnsiToWString(mCurrShader);
    }
    else if ((int)wParam == 'K')
    {
        mShaderConstants.fa += 1.0f;
        strDebug += L"a = " + std::to_wstring(mShaderConstants.fa);
    }
    else if ((int)wParam == 'L')
    {
        mShaderConstants.fa -= 1.0f;
        strDebug += L"a = " + std::to_wstring(mShaderConstants.fa);
    }
	strDebug += L"================\n";
	OutputDebugString(strDebug.c_str());
}