#pragma once
#include "..\Common\Demo.h"
#include "PointList.h"

class PointListDemo : public Demo {
public:
    PointListDemo(FireFlame::Engine& engine);

    //void Update(float time_elapsed) override;

private:
    PointList mPointList;
};
