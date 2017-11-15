#pragma once
#include <DirectXMath.h>
#include "FireFlameHeader.h"

struct ObjectConstants
{
    ObjectConstants() = default;
    float               TexScale = 16.0f;
    int                 Turbulence = 0;
    int                 ManualOctave = false;
    int                 Octave = 1;
	int                 UseNoise2D = 1;
	int                 UseRandomGrad = 1;
};

class Game {
public:
    Game(FireFlame::Engine& engine);

    void Update(float time_elapsed);
    void SetRenderItem(const std::string& name) {
        mRenderItem = name;
    }
    void SetMainWnd(HWND hwnd) { mhMainWnd = hwnd; }

    void OnGameWindowResized(int w, int h);
    void OnMouseDown(WPARAM btnState, int x, int y);
    void OnMouseUp(WPARAM btnState, int x, int y);
    void OnMouseMove(WPARAM btnState, int x, int y);
    void OnKeyUp(WPARAM wParam, LPARAM lParam);
    void OnKeyDown(WPARAM wParam, LPARAM lParam);

private:
    FireFlame::Engine& mEngine;

    HWND        mhMainWnd;
    std::string mRenderItem;

    ObjectConstants mShaderConstants;
};
