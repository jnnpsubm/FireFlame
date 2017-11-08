#pragma once
#include "..\Common\Demo.h"
#include "PyramidMesh.h"

class PyramidDemo : public Demo {
public:
    PyramidDemo(FireFlame::Engine& e);

private:
    PyramidMesh mPyramid;
};
