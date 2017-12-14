#pragma once
#include "FLCSTask.h"

namespace FireFlame {
struct D3DCSTask : CSTask
{
    D3DCSTask
    (
        const std::string& shaderName,
        const std::string& PSOName,
        unsigned X,
        unsigned Y,
        unsigned Z,
        const std::function<void(void*)>& callback1
    )
        :CSTask(shaderName, PSOName, X, Y, Z, callback1)
    {}
    D3DCSTask
    (
        const std::string& shaderName,
        const std::string& PSOName,
        unsigned X,
        unsigned Y,
        unsigned Z,
        const std::function<void(void*)>& callback1,
        const std::function<void(void*, void*)>& callback2
    )
        :CSTask(shaderName, PSOName, X, Y, Z, callback1, callback2)
    {}
};
} // end FireFlame
