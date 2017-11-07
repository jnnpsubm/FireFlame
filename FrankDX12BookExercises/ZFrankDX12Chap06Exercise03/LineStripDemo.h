#pragma once
#include "..\Common\Demo.h"
#include "PointList.h"

class LineStripDemo : public Demo {
public:
    LineStripDemo(FireFlame::Engine& engine);

private:
    PointList mPointList;
};