#pragma once
#include <vector>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include "FireFlameHeader.h"

struct ObjectConstants
{
    ObjectConstants() = default;
    DirectX::XMFLOAT3   vLineColor = DirectX::XMFLOAT3(1.f, 0.f, 0.f);
    float               fTexScale  = 10.0f;
    DirectX::XMFLOAT3   vBackDrop = DirectX::XMFLOAT3(0.941176534f, 0.972549081f, 1.000000000f);
    float               fWidth     = 1.0f;

    float               fa         = 2.0f;
};

class Demo {
public:
    Demo(FireFlame::Engine& engine);

    void Update(float time_elapsed);
    void UseShader(const std::string& name) {
        mCurrShader = name;
    }
    void UseGeometry(const std::string& name) {
        mCurrGeo = name;
    }
    void SetMainWnd(HWND hwnd) { mhMainWnd = hwnd; }

    void AddShaders();
    void AddGeometry();

    void OnGameWindowResized(int w, int h);
    void OnMouseDown(WPARAM btnState, int x, int y);
    void OnMouseUp(WPARAM btnState, int x, int y);
    void OnMouseMove(WPARAM btnState, int x, int y);
    void OnKeyUp(WPARAM wParam, LPARAM lParam);
    void OnKeyDown(WPARAM wParam, LPARAM lParam);

private:
    FireFlame::Engine& mEngine;

    HWND        mhMainWnd;
    std::string mCurrShader;
    std::string mCurrGeo;

    ObjectConstants mShaderConstants;

    struct stShaderFun {
        stShaderFun(const std::string& name, const std::wstring& file)
            : shaderName(name),shaderPSFile(file){}
        std::string  shaderName;
        std::wstring shaderPSFile;
    };
    std::vector<stShaderFun> mShaders;
};
