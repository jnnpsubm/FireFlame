#pragma once
#include "..\Common\Demo.h"
#include "PointList.h"

class TriangleStripDemo : public Demo {
public:
    TriangleStripDemo(FireFlame::Engine& engine);

private:
    PointList mPointList;
};