#include "Game.h"
#include <fstream>
#include <random>

Game::Game(FireFlame::Engine& engine) :mEngine(engine) {
	std::ofstream fileIn("NoiseGrad.txt");
	std::ofstream fileInGrad2D("NoiseGrad2D.txt");
	std::ofstream fileInPerm("NoisePerm.txt");
	int size = 512;
	std::default_random_engine rng1;
	std::default_random_engine rng2;
	std::default_random_engine rng3;
	std::uniform_real_distribution<float> dist1(-1.0f,1.0f);
	std::uniform_int_distribution<int> dist2(0, size-1);
	std::uniform_real_distribution<float> dist3(-1.0f, 1.0f);
	for (int i = 0; i < size; i++)
	{
		float x = dist1(rng1);
		float y = dist1(rng1);
		float z = dist1(rng1);
		DirectX::XMFLOAT3 stored(x, y, z);
		DirectX::XMVECTOR vec3 = DirectX::XMLoadFloat3(&stored);
		vec3 = DirectX::XMVector3Normalize(vec3);
		DirectX::XMStoreFloat3(&stored, vec3);
		fileIn << "float3(" << stored.x << " ," << stored.y << " ," << stored.z << "),";
		if (i % 5 == 0) fileIn << std::endl;

		fileInPerm << dist2(rng2) << ",";
		if (i % 15 == 0) fileInPerm << std::endl;

		// grad 2D
		x = dist3(rng3);
		y = dist3(rng3);
		DirectX::XMFLOAT2 stored2(x, y);
		DirectX::XMVECTOR vec2 = DirectX::XMLoadFloat2(&stored2);
		vec2 = DirectX::XMVector2Normalize(vec2);
		DirectX::XMStoreFloat2(&stored2, vec2);
		fileInGrad2D << "float2(" << stored.x << " ," << stored.y << "),";
		if (i % 5 == 0) fileInGrad2D << std::endl;
	}
}
void Game::OnGameWindowResized(int w, int h) {
   
}
void Game::Update(float time_elapsed) {
    //mEngine.GetScene()->UpdateShaderCBData(mCurrShader, 0, mShaderConstants);
    mEngine.GetScene()->UpdateRenderItemCBData(mRenderItem, sizeof(ObjectConstants), &mShaderConstants);
}
void Game::OnMouseDown(WPARAM btnState, int x, int y) {
   
}
void Game::OnMouseUp(WPARAM btnState, int x, int y) {
   
}
void Game::OnMouseMove(WPARAM btnState, int x, int y) {
   
}

void Game::OnKeyUp(WPARAM wParam, LPARAM lParam) {
   
}
void Game::OnKeyDown(WPARAM wParam, LPARAM lParam) {
	std::wstring strDebug(L"================");
    if ((int)wParam == 'A')
    {
        mShaderConstants.TexScale += 1.0f;
		strDebug += L"TexScale = " + std::to_wstring(mShaderConstants.TexScale);
    }
    else if ((int)wParam == 'J')
    {
        mShaderConstants.TexScale -= 1.0f;
		strDebug += L"TexScale = " + std::to_wstring(mShaderConstants.TexScale);
    }
    else if ((int)wParam == 'B')
    {
        mShaderConstants.Turbulence = !mShaderConstants.Turbulence;
		strDebug += L"Turbulence Mode = " + std::to_wstring(mShaderConstants.Turbulence);
    }
    else if ((int)wParam == 'M')
    {
        mShaderConstants.ManualOctave = !mShaderConstants.ManualOctave;
		strDebug += L"ManualOctave Mode = " + std::to_wstring(mShaderConstants.ManualOctave);
    }
    else if ((int)wParam == 'O')
    {
		if (mShaderConstants.Octave <= 0) mShaderConstants.Octave = 1;
        mShaderConstants.Octave += 1;
		strDebug += L"Octave = " + std::to_wstring(mShaderConstants.Octave);
    }
	else if ((int)wParam == 'P')
	{
		if (mShaderConstants.Octave > 0) mShaderConstants.Octave -= 1;
		strDebug += L"Octave = " + std::to_wstring(mShaderConstants.Octave);
	}
	else if ((int)wParam == '2')
	{
		mShaderConstants.UseNoise2D = 1;
		strDebug += L"UseNoise2D = " + std::to_wstring(mShaderConstants.UseNoise2D);
	}
	else if ((int)wParam == '3')
	{
		mShaderConstants.UseNoise2D = 0;
		strDebug += L"UseNoise2D = " + std::to_wstring(mShaderConstants.UseNoise2D);
	}
	else if ((int)wParam == 'R')
	{
		mShaderConstants.UseRandomGrad = !mShaderConstants.UseRandomGrad;
		strDebug += L"UseRandomGrad = " + std::to_wstring(mShaderConstants.UseRandomGrad);
	}
	strDebug += L"================\n";
	OutputDebugString(strDebug.c_str());
}