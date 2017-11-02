#pragma once
#include <DirectXMath.h>
#include "FireFlameHeader.h"

struct ObjectConstants
{
    DirectX::XMFLOAT4X4 WorldViewProj = FireFlame::Matrix4X4();
};

class Game {
public:
    Game(FireFlame::Engine& engine);
    void Update(float time_elapsed);

private:
    FireFlame::Engine& mEngine;
};
