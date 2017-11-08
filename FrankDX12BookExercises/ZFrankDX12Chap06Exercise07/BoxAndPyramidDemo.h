#pragma once
#include "..\Common\Demo.h"
#include "BoxAndPyramidMesh.h"

class BoxAndPyramidDemo : public Demo {
public:
    BoxAndPyramidDemo(FireFlame::Engine& e);

    void OnKeyUp(WPARAM wParam, LPARAM lParam) override;

private:
    BoxAndPyramidMesh mBoxAndPyramid;
};
