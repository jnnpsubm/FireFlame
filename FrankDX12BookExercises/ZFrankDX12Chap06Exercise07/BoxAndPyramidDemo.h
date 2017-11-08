#pragma once
#include "..\Common\Demo.h"
#include "BoxAndPyramidMesh.h"

class BoxAndPyramidDemo : public Demo {
public:
    BoxAndPyramidDemo(FireFlame::Engine& e);

private:
    BoxAndPyramidMesh mBoxAndPyramid;
};
