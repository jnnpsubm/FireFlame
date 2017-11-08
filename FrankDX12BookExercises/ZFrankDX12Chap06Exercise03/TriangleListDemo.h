#pragma once
#include "..\Common\Demo.h"
#include "PointList.h"

class TriangleListDemo : public Demo {
public:
    TriangleListDemo(FireFlame::Engine& engine);

private:
    PointList mPointList;
};