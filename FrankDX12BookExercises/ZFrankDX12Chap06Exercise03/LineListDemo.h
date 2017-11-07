#pragma once
#include "..\Common\Demo.h"
#include "PointList.h"

class LineListDemo : public Demo {
public:
    LineListDemo(FireFlame::Engine& engine);

private:
    PointList mPointList;
};