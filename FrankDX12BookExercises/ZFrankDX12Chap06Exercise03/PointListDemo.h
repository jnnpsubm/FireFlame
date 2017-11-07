#pragma once
#include "..\Common\Demo.h"
#include "PointList.h"

struct ShaderConsts {
    DirectX::XMFLOAT4X4 WorldViewProj = FireFlame::Matrix4X4();
};

class PointListDemo : public Demo {
public:
    PointListDemo(FireFlame::Engine& engine);

    void Update(float time_elapsed) override;

private:
    PointList mPointList;
};
